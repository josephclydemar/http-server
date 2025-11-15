#include "network.h"
#include "logging.h"


int use_addr(const struct addrinfo *addr) {
  if (addr != NULL) {
    return socket(addr->ai_family, addr->ai_socktype, addr->ai_protocol);
  }
  return -1;
}

void show_addrinfo(const bool verbose, const struct addrinfo *info) {
  char ip4_buffer[INET_ADDRSTRLEN];
  char ip6_buffer[INET6_ADDRSTRLEN];
  struct sockaddr_in *sa = NULL;
  struct sockaddr_in6 *sa6 = NULL;

  for (int i = 0; info != NULL; ++i) {
    memset(ip4_buffer, 0, INET_ADDRSTRLEN);
    memset(ip6_buffer, 0, INET6_ADDRSTRLEN);
    if (info->ai_family == AF_INET) {
      sa = (struct sockaddr_in*)(info->ai_addr);
      inet_ntop(AF_INET, &(sa->sin_addr), ip4_buffer, INET_ADDRSTRLEN);
    }
    if (info->ai_family == AF_INET6) {
      sa6 = (struct sockaddr_in6*)(info->ai_addr);
      inet_ntop(AF_INET6, &(sa6->sin6_addr), ip6_buffer, INET6_ADDRSTRLEN);
    }

    if (verbose) {
      DEBUG_LOG(
        "ADDRINFO",
        "\n"
        "    (%02d)\n"
        "    [ai_flags: %d]\n"
        "    [ai_family: %d]\n"
        "    [ai_socktype: %d]\n"
        "    [ai_protocol: %d]\n"
        "    [ai_addrlen: %u]\n"
        "    [ai_cannonname: %s]\n"
        "    [ai_addr: %s]\n"
        "    [addr: %p]\n",
        i,
        info->ai_flags,
        info->ai_family,
        info->ai_socktype,
        info->ai_protocol,
        info->ai_addrlen,
        info->ai_canonname,
        (info->ai_family == AF_INET ? ip4_buffer : (info->ai_family == AF_INET6 ? ip6_buffer : "")),
        (void*)info);
    } else {
      DEBUG_LOG(
        "ADDRINFO",
        "\n"
        "    (%02d)\n"
        "    [ai_family: %d]\n"
        "    [ai_addr: %s]\n"
        "    [addr: %p]\n",
        i,
        info->ai_family,
        (info->ai_family == AF_INET ? ip4_buffer : (info->ai_family == AF_INET6 ? ip6_buffer : "")),
        (void*)info);
    }
 
    info = info->ai_next;
  }
}
