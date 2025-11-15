#include <string.h>
#include "logging.h"
#include "debugging.h"
#include "str.h"
#include "http_parser.h"
#include "network.h"
#include "server.h"

#define LISTEN_BACKLOG_MAX       2048
#define RECV_BUF_SIZE            8192
#define SEND_BUF_SIZE            16384
#define MSG_BUF_SIZE             8192

struct server_mgr server_mgr;
struct logger_mgr logger_mgr;


void* listener_fn(void* param) {
  const char* port = (char*)param;

  union qnode_d log_data = { 0 };
  struct addrinfo hints;
  struct addrinfo *server_info = NULL;
  int client_sockfd = 0;
  struct sockaddr_storage client_addr;
  socklen_t client_addr_size = sizeof(struct sockaddr_storage);
  char ip4_buffer[INET_ADDRSTRLEN];
  char ip6_buffer[INET6_ADDRSTRLEN];
  /*pthread_t tid = pthread_self();*/
  memset(&hints, 0, sizeof(struct addrinfo));
  
  hints.ai_family = AF_UNSPEC; 
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags = AI_PASSIVE;

  ASSERT("GETADDRINFO", getaddrinfo(NULL, port, &hints, &server_info) == 0);
  /*show_addrinfo(false, server_info);*/
  int sockfd = use_addr(server_info);
  ASSERT("SOCKET", sockfd > 0);
  int optval = 1;
  ASSERT("SETSOCKOPT", setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) >= 0);
  ASSERT("BIND", bind(sockfd, server_info->ai_addr, server_info->ai_addrlen) == 0);
  ASSERT("LISTEN", listen(sockfd, LISTEN_BACKLOG_MAX) == 0);

  pthread_mutex_lock(&logger_mgr.logger_lock);
  DEBUG_LOG_REDI(logger_mgr.holder_buf, DTYPE_STR_MAX_LEN, "LISTEN", "Listening on PORT (%s)", port);
  memcpy(log_data.d_str, logger_mgr.holder_buf, DTYPE_STR_MAX_LEN);
  enqueue(&logger_mgr.logger_queue, log_data);
  pthread_mutex_unlock(&logger_mgr.logger_lock);
  pthread_cond_signal(&logger_mgr.logger_queue_not_empty_cond);

  while (true) {
    sem_wait(&server_mgr.worker_thread_available);
    client_sockfd = accept(sockfd, (struct sockaddr *)&client_addr, &client_addr_size);
    ASSERT("ACCEPT", client_sockfd >= 0);

    pthread_mutex_lock(&server_mgr.t_queue_lock);
    enqueue(&server_mgr.t_queue, (union qnode_d){ .d_int32 = client_sockfd });
    pthread_mutex_unlock(&server_mgr.t_queue_lock);
    pthread_cond_signal(&server_mgr.t_queue_not_empty_cond);

    if (((struct sockaddr *)&client_addr)->sa_family == AF_INET) {
      struct sockaddr_in *sa = (struct sockaddr_in *)&client_addr;
      inet_ntop(sa->sin_family, &(sa->sin_addr), ip4_buffer, INET_ADDRSTRLEN);
    } else {
      struct sockaddr_in6 *sa6 = (struct sockaddr_in6 *)&client_addr;
      inet_ntop(sa6->sin6_family, &(sa6->sin6_addr), ip6_buffer, INET6_ADDRSTRLEN);
    }
  }

  pthread_exit(NULL);
}

void* worker_fn(void* param) {
  (void)param;
  size_t http_body_buf_offset = 0;
  char http_body_buf[MSG_BUF_SIZE];
  size_t item_padding = 12;

  union qnode_d log_data = { 0 };
  pthread_t tid = pthread_self();
  int recv_ret = 0;
  int send_ret = 0;
  char recv_buf[RECV_BUF_SIZE];
  char send_buf[SEND_BUF_SIZE];
  int client_sockfd;


  /* Logging related code */
  pthread_mutex_lock(&logger_mgr.logger_lock);
  INFO_LOG_REDI(logger_mgr.holder_buf, DTYPE_STR_MAX_LEN, "SELF", "tid: %lu", tid);
  memcpy(log_data.d_str, logger_mgr.holder_buf, DTYPE_STR_MAX_LEN);
  enqueue(&logger_mgr.logger_queue, log_data);
  pthread_mutex_unlock(&logger_mgr.logger_lock);
  pthread_cond_signal(&logger_mgr.logger_queue_not_empty_cond);

  while (true) {
    pthread_mutex_lock(&server_mgr.t_queue_lock);
    while (server_mgr.t_queue.count <= 0) {
      pthread_cond_wait(&server_mgr.t_queue_not_empty_cond, &server_mgr.t_queue_lock);
    }
    client_sockfd = dequeue(&server_mgr.t_queue).d_int32;
    pthread_mutex_unlock(&server_mgr.t_queue_lock);

    http_body_buf_offset = 0;
    memset(http_body_buf, 0, MSG_BUF_SIZE);
    memset(recv_buf, 0, RECV_BUF_SIZE);
    recv_ret = recv(client_sockfd, recv_buf, RECV_BUF_SIZE - 1, 0);
    ASSERT("RECV", recv_ret >= 0);

    http_headers_t* headers = parse_headers(recv_buf);
    size_t digit_offset = 0;
    for (size_t i = 0; i < headers->len; ++i) {
      if (i > 9) digit_offset = 1;
      else digit_offset = 0;
      snprintf(
        http_body_buf + http_body_buf_offset,
        headers->headers_ptr[i].len + digit_offset + ((i == headers->len - 1) ? item_padding - 1 : item_padding),
        "\"item%lu\":\"%s\",", i, headers->headers_ptr[i].ptr
      );
      http_body_buf_offset += (headers->headers_ptr[i].len + digit_offset + item_padding - 1);
    }

    if (headers->headers_ptr[0].ptr != NULL) {
      pthread_mutex_lock(&logger_mgr.logger_lock);
      INFO_LOG_REDI(logger_mgr.holder_buf, DTYPE_STR_MAX_LEN, "PATH", "[tid: %lu]", tid);
      memcpy(log_data.d_str, logger_mgr.holder_buf, DTYPE_STR_MAX_LEN);
      enqueue(&logger_mgr.logger_queue, log_data);
      pthread_mutex_unlock(&logger_mgr.logger_lock);
      pthread_cond_signal(&logger_mgr.logger_queue_not_empty_cond);

      headers->headers_ptr[0].ptr[headers->headers_ptr[0].len - 2] = '\0';
      snprintf(
        send_buf, 
        SEND_BUF_SIZE - 1,
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: application/json; charset=utf-8\r\n"
        "Content-Length: %lu\r\n"
        "Connection: keep-alive\r\n"
        "Keep-Alive: timeout=5\r\n"
        /*"Transfer-Encoding: chunked\r\n"*/
        "\r\n"
        "{%s}",
        strlen(http_body_buf) + 2,
        http_body_buf
      );
      send_ret = send(client_sockfd, send_buf, strlen(send_buf), 0);
      ASSERT("SEND", send_ret >= 0);
      free_headers(headers);
    }
    close(client_sockfd);
    sem_post(&server_mgr.worker_thread_available);
  }

  /*pthread_mutex_lock(&logger_mgr.logger_lock);*/
  /*ERROR_LOG_REDI(logger_mgr.holder_buf, DTYPE_STR_MAX_LEN, "EXIT", "tid: %lu", tid);*/
  /*memcpy(log_data.d_str, logger_mgr.holder_buf, DTYPE_STR_MAX_LEN);*/
  /*enqueue(&logger_mgr.logger_queue, log_data);*/
  /*pthread_mutex_unlock(&logger_mgr.logger_lock);*/
  /*pthread_cond_signal(&logger_mgr.logger_queue_not_empty_cond);*/

  pthread_exit(NULL);
}
