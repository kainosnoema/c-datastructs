#include <stdio.h>
#include <stdarg.h>

void printf_pass(char *fmt, ...)
{
  va_list argp;
  printf("\033[1;32m");
  va_start(argp, fmt);
  vprintf(fmt, argp);
  va_end(argp);
  printf("\033[0m");  
}

void printf_fail(char *fmt, ...)
{
  va_list argp;
  printf("\033[1;31m");
  va_start(argp, fmt);
  vprintf(fmt, argp);
  va_end(argp);
  printf("\033[0m");  
}