#include <string.h>
#include <time.h>
#include <stdio.h>
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
static inline int tolower_ascii(int const c)
{
    return (c >= 'A' && c <= 'Z') ? c - 'A' + 'a' : c;
}

unsigned long AlphaHash(const char *Text, size_t Length)		/*{{{*/
{
   /* This very simple hash function for the last 8 letters gives
      very good performance on the debian package files */
   if (Length > 8)
   {
    Text += (Length - 8);
    Length = 8;
   }
   unsigned long Res = 0;
   for (size_t i = 0; i < Length; ++i)
      Res = ((unsigned long)(Text[i]) & 0xDF) ^ (Res << 1);
   return Res & 0xFF;
}
   
unsigned long AlphaHash2(const char *Text, size_t Length)
{                               /*{{{ */
    /* This very simple hash function for the last 8 letters gives
       very good performance on the debian package files */
    if (Length > 8) {
        Text += (Length - 8);
        Length = 8;
    }
    unsigned long Res = 0;
    switch (Length) {
    case 8:
        Res = ((unsigned long) (Text[8]) & 0xDF) ^ (Res << 1);
    case 7:
        Res = ((unsigned long) (Text[7]) & 0xDF) ^ (Res << 1);
    case 6:
        Res = ((unsigned long) (Text[6]) & 0xDF) ^ (Res << 1);
    case 5:
        Res = ((unsigned long) (Text[5]) & 0xDF) ^ (Res << 1);
    case 4:
        Res = ((unsigned long) (Text[4]) & 0xDF) ^ (Res << 1);
    case 3:
        Res = ((unsigned long) (Text[3]) & 0xDF) ^ (Res << 1);
    case 2:
        Res = ((unsigned long) (Text[2]) & 0xDF) ^ (Res << 1);
    case 1:
        Res = ((unsigned long) (Text[1]) & 0xDF) ^ (Res << 1);
    }

    return Res & 0xFF;
}

unsigned int DjbHash(const char *s, size_t l)
{
    unsigned int r = 5381;

    while (l-- > 0) {
        r = 33 * r + *s++;
    }

    return r;
}

unsigned int DjbCase(const char *s, size_t l)
{
    unsigned int r = 5381;

    while (l-- > 0) {
        r = 33 * r + tolower_ascii(*s++);
    }

    return r;
}

unsigned int DjbCase2(const char *s, size_t l)
{
    unsigned int r = 5381;

    while (l-- > 0) {
        r = 33 * r + ((*s++) | 32);
    }

    return r;
}
