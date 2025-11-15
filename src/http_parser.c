#include "http_parser.h"
#include "debugging.h"
#include "str.h"

#define BUF_SIZE     2048

/*size_t count_double_quote(const char* str) {*/
/*  size_t total = 0;*/
/*  for (size_t i = 0; i < strlen(str) + 1; ++i) {*/
/*    if (str[i] == '"') total += 1;*/
/*  }*/
/*  return total;*/
/*}*/

http_headers_t* parse_headers(const char* const str_headers) {
  char buf[BUF_SIZE];
  size_t buf_i = 0;
  http_headers_t* headers = malloc(sizeof(http_headers_t));
  ASSERT("MALLOC", headers != NULL);

  headers->len = 0;
  headers->cap = 4;
  headers->headers_ptr = malloc(headers->cap * sizeof(str_t));
  ASSERT("MALLOC", headers->headers_ptr != NULL);
  for (size_t i = 0; i < headers->cap; ++i) {
    headers->headers_ptr[i].ptr = NULL;
    headers->headers_ptr[i].len = 0;
    headers->headers_ptr[i].cap = 4;
  }

  memset(buf, 0, BUF_SIZE);
  for (size_t i = 0; str_headers[i] != '\0'; ++i) {
    if (str_headers[i] == '\r' && str_headers[i+1] == '\n') {
      buf[buf_i] = 'R';
      buf[buf_i + 1] = 'N';
      headers->headers_ptr[headers->len].len = strlen(buf);
      headers->headers_ptr[headers->len].cap = headers->headers_ptr[headers->len].len + 1;
      headers->headers_ptr[headers->len].ptr = malloc(headers->headers_ptr[headers->len].cap);
      ASSERT("MALLOC", headers->headers_ptr[headers->len].ptr != NULL);

      memcpy(headers->headers_ptr[headers->len].ptr, buf, headers->headers_ptr[headers->len].cap);

      if (headers->len >= headers->cap - 1) {
        headers->cap *= 2;
        headers->headers_ptr = realloc(headers->headers_ptr, headers->cap * sizeof(str_t));
        ASSERT("REALLOC", headers->headers_ptr != NULL);
      }

      headers->len++;
      memset(buf, 0, BUF_SIZE);
      ++i;
      buf_i = 0;
      continue;
    }
    if (str_headers[i] == '"') {
      buf[buf_i] = '\\';
      ++buf_i;
    }
    buf[buf_i] = str_headers[i];
    ++buf_i;
  }
  return headers;
}


void free_headers(http_headers_t* const headers) {
  for (size_t i = 0; i < headers->len; ++i) {
    free(headers->headers_ptr[i].ptr);
  }
  free(headers->headers_ptr);
  free(headers);
}

