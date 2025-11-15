#ifndef STR_H_
#define STR_H_

#include "types.h"

typedef struct {
  char* ptr;
  size_t len;
  size_t cap;
} str_t;


void str_from_textfile(str_t* const str, const char* const filename);
void str_free(str_t* const str);

#endif // STR_H_

