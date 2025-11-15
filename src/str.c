#include "debugging.h"
#include "str.h"

void str_from_textfile(str_t* const str, const char* const filename) {
  str->len = 0;
  str->cap = 4;
  str->ptr = malloc(str->cap);
  ASSERT("MALLOC", str->ptr != NULL);

  FILE* fh = fopen(filename, "r");
  ASSERT("FOPEN", fh != NULL);

  int c;
  while ((c = fgetc(fh)) != EOF) {
    str->ptr[str->len] = c;
    str->len++;
    if (str->len >= str->cap - 1) {
      str->cap *= 2;
      str->ptr = realloc(str->ptr, str->cap);
      ASSERT("REALLOC", str->ptr != NULL);
    }
  }
  str->ptr[str->len] = 0;

  ASSERT("FCLOSE", fclose(fh) == 0);
}

/*str_t* str_slice(const str_t* const str) {}*/

void str_free(str_t* const str) {
  free(str->ptr);
  str->len = 0;
  str->cap = 0;
}
