
#include "util.h"

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

int
debug (const char *format, ...)
{
  /* Diese Funktion stellt eine Senke für debug Nachrichten bereit, die in ein
   * logfile in dem Verzeichnis geschrieben werden, in dem die shell gestartet
   * wurde. */

  static FILE *logfile = NULL;
  if (logfile == 0)
    logfile = fopen(".fresh.log", "a");

  int res = 0;

  va_list ap;
  va_start(ap, format);
  res += vfprintf(logfile, format, ap);
  va_end(ap);

  res += fprintf(logfile, "\n");

  return res;
}

int str_find(char *needle, char *haystack, long unsigned int *needle_pos) {
  long unsigned int start;
  int found = 0;
  needle_pos = NULL;
  for(start = 0; start < (sizeof(haystack) - sizeof(needle) + 1); start++) {
    int inner_found = 1;
    for(long unsigned int pos = 0; pos < sizeof(needle); pos++) {
      if(needle[pos] != haystack[start+pos]) {
        inner_found = 0;
        break;
      }
    }
    if(inner_found) {
      found = 1;
      *needle_pos = start;
      break;
    }
  }
  return found;
}

char *str_repl(char *needle, char *haystack, char *hay) {
  long unsigned int needle_pos;
  if(!str_find(needle, haystack, &needle_pos)) {
    printf("DEBUG: No occurence found, copying string.\n");
    return strdup(haystack);
  }
  printf("DEBUG: needle_pos = %i\n", needle_pos);
  char *result = (char *) malloc(sizeof(char) * (sizeof(haystack) - sizeof(needle) + sizeof(hay)));
  char *n = needle, *hs = haystack, *h = hay, *r = result;
  printf("DEBUG: %i\n", result);
  for(long unsigned int i = 0; i < needle_pos; i++)
    *r++ = *hs++;
  for(long unsigned int i = 0; i < sizeof(hay); i++)
    *r++ = *h++;
  while(*n++) *hs++;
  for(long unsigned int i = 0; i < sizeof(haystack) - sizeof(needle) - needle_pos; i++)
    *r++ = *hs++;
  printf("DEBUG: %s\n", result);
  printf("DEBUG: %i\n", result);
  return result;
}
