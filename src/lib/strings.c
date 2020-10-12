#include "strings.h"

void *memset(void *s, int c, size_t n) // write n bytes of c to *s
{
    unsigned char *ptr = s; // *ptr = address

    while(n != 0)
    {
        
        *ptr = (char)c; // the byte we're looking at = c (in char form)
        ptr++; // adds 1 byte too address in ptr
        n--; // increment count

    }
    return s;
}

void *memcpy(void *dst, const void *src, size_t n)
{
    unsigned char *ptr = dst;
    unsigned const char *ptr2 = src;

    while (n != 0) {
        *ptr = *ptr2;
        ptr2++;
        ptr++;
        n--;
    }
    return dst;
}

size_t strlen(const char *s)
{
    /* Implementation a gift to you from lab3 */
    int n = 0;
    while (s[n] != '\0') {
        n++;
    }
    return n;
}

int strcmp(const char *s1, const char *s2)
{
    /* TODO: Your code here */
    return 0;
}

size_t strlcat(char *dst, const char *src, size_t maxsize)
{
    /* TODO: Your code here */
    return 0;
}

unsigned int strtonum(const char *str, const char **endptr)
{
    /* TODO: Your code here */
    return 0;
}
