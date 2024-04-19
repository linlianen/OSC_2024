#include "../include/lib.h"

int strncmp(const char *a, const char *b, int len)
{
  for (int i = 0; i < len; i++)
  {
    if (a[i] != b[i])
      return 1;
  }
  return 0;
}

int my_strcmp(const char *a, const char *b)
{
  int i = 0;
  while (a[i] != '\0' && b[i] != '\0' && a[i] == b[i])
    i++;
  return a[i] - b[i];
}

void memcpy_diy(char *dst, const char *src, size_t n)
{
  while (n--)
  {
    *dst++ = *src++;
  }
}

size_t my_strlen(const char *s)
{
  size_t i = 0;
  while (s[i])
    i++;
  return i;
}


void my_strcpy(char* dst, const char* src){
  int i=0;
  while(src[i] != '\0'){
    dst[i] = src[i];
    i++;
  }
  dst[i]='\0';
}
void *my_memset(void *dest, register int val, register size_t len)
{
  register unsigned char *ptr = (unsigned char *)dest;
  while (len-- > 0)
    *ptr++ = val;
  return dest;
}

char *strstr(const char *s, const char *find)
{
  char c, sc;
  size_t len;
  if ((c == *find++) != 0)
  {
    /* code */ len = my_strlen(find);
    do
    {
      /* code */
      do
      {
        if ((sc = *s++) == 0)
          return NULL;
      } while (sc != c);

    } while (strncmp((char *)s, find, len) != 0);
  }
  return ((char *)s);
}

char *my_strtok(char *s, const char *delim)
{
  static char *last;
  return my_strtok_r(s, delim, &last);
}

int myAtoi(char* str)
{
  // Initialize result
  int res = 0;

  // Iterate through all characters
  // of input string and update result
  // take ASCII character of corresponding digit and
  // subtract the code from '0' to get numerical
  // value and multiply res by 10 to shuffle
  // digits left to update running total
  for (int i = 0; str[i] != '\0'; ++i)
    res = res * 10 + str[i] - '0';

  // return result.
  return res;
}


char *my_strtok_r(char *s, const char *delim, char **last)
{
  char *spanp;
  int c, sc;
  char *tok;
  if (s == NULL && (s = *last) == NULL)
    return (NULL);
  /*
   * Skip (span) leading delimiters (s += strspn(s, delim), sort of).
   */
cont:
  c = *s++;
  for (spanp = (char *)delim; (sc = *spanp++) != 0;)
  {
    if (c == sc)
      goto cont;
  }
  if (c == 0)
  { /* no non-delimiter characters */
    *last = NULL;
    return (NULL);
  }
  tok = s - 1;
  /*
   * Scan token (scan for delimiters: s += strcspn(s, delim), sort of).
   * Note that delim must have one NUL; we stop if we see that, too.
   */
  for (;;)
  {
    c = *s++;
    spanp = (char *)delim;
    do
    {
      if ((sc = *spanp++) == c)
      {
        if (c == 0)
          s = NULL;
        else
          s[-1] = 0;
        *last = s;
        return (tok);
      }
    } while (sc != 0);
  }
  /* NOTREACHED */
}
