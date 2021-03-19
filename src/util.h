#ifndef UTIL_H
#define UTIL_H

int debug(const char *format, ...);
int str_find(char *needle, char *haystack, long unsigned int *needle_pos);
char *str_repl(char *needle, char *haystack, char *hay);

#endif
