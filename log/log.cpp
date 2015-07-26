#include "log.h"
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>

char *colors[3] = {
  "\x1B[37m", // white
  "\x1B[33m", // yellow
  "\x1B[31m" // red
};

void __log (LogLevel level, ...) {
  char *str = NULL;
  char *fmt;

  va_list var_args;
  va_start (var_args, level);
  fmt = va_arg(var_args, char *);
  int len = vasprintf(&str, fmt, var_args);
  va_end (var_args);

  printf("%s%s\n", colors[level], str);

  free(str);
}
