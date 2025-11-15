#ifndef HTTP_PARSER_H_
#define HTTP_PARSER_H_

#include "str.h"

typedef struct {
  str_t* headers_ptr;
  size_t len;
  size_t cap;
} http_headers_t;

http_headers_t* parse_headers(const char* const str_headers);
void free_headers(http_headers_t* const headers);

#endif // HTTP_PARSER_H_


