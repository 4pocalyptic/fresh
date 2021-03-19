
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

long unsigned int str_find(char *needle, char *haystack) {
  long unsigned int start = -1;
  int found = 0;
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
      break;
    }
  }
  if(!found) start = -1;
  return start;
}

char *str_repl(char *needle, char *haystack, char *hay) {
  long unsigned int needle_pos = str_find(needle, haystack);
  if(needle_pos < 0) {
    char *result = (char *) malloc(sizeof(char) * sizeof(haystack));
    strcpy(result, haystack);
    return result; 
  }
  char *result = (char *) malloc(sizeof(char) * (sizeof(haystack) - sizeof(needle) + sizeof(hay)));
  printf("DEBUG: %i\n", result);
  long unsigned int pos = 0;
  for(long unsigned int i = 0; i < needle_pos; i++) {
    result[pos] = haystack[i];
    pos++;
  }
  for(long unsigned int i = 0; i < sizeof(hay); i++) {
    result[pos] = hay[i];
    pos++;
  }
  for(long unsigned int i = sizeof(needle) + needle_pos; i < sizeof(haystack); i++) {
    result[pos] = haystack[i];
    pos++;
  }
  printf("DEBUG: %s\n", result);
  printf("DEBUG: %i\n", result);
  return result;
}
