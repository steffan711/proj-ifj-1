#include "types.h"

/* Abstrakcia suboru nacitaneho v pamati */
#define _FILE_IN_MEMORY_
#ifdef _FILE_IN_MEMORY_
  #undef getc
  #undef ungetc
  #undef EOF
  #define getc(x) (*(x++))
  #define ungetc(x) (*(x--))
  #define EOF 0
#endif


E_ERROR_TYPE check_file_header(char *source_file);