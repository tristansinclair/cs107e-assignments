#include "strings.h"

void *memset(void *s, int c, size_t n) // write n bytes of c to *s
{
    char *_s = s;
    for (size_t i = 0; i < n;)
        _s[i++] = c;
    return s;
}

void *memcpy(void *dst, const void *src, size_t n)
{
    char *_dst = dst;
    const char *_src = src;
    for (size_t i = 0; i < n; i++)
    {
        _dst[i] = _src[i];
    }
    return dst;
}

size_t strlen(const char *s)
{
    /* Implementation a gift to you from lab3 */
    int n = 0;
    while (s[n] != '\0')
    {
        n++;
    }
    return n;
}

int strcmp(const char *s1, const char *s2)
{
    while (*s1 && *s2 && *s1 == *s2)
    {
        s1++;
        s2++;
    }
    return *s1 - *s2;
}

size_t strlcat(char *dst, const char *src, size_t maxsize)
{
    int counter = 0;
    char *myptr = dst; // point to beginning of dst
    int length = strlen(src);

    while (*myptr != '\0' && counter != maxsize)
    { // don't assume null in dst
        myptr++;
        counter++;
    }
    if (counter == maxsize)
    { // if there wasn't a null ptr
        return maxsize + length;
    }

    while (*src != '\0' && maxsize - strlen(dst) - 1 != 0)
    {
        *myptr = *src;
        myptr++;
        src++;
        *myptr = '\0';
    }

    return counter + length; // returns length of dst + src (regardless if there was space for src)
}

enum
{
    DECIMAL = 10,
    HEX = 16
};

unsigned int strtonum(const char *str, const char **endptr)
{
    const char *ptr = str; // initialize ptr to point to same spot as str
    int counter = strlen(str);
    int hex = 0; // bool

    // check if first 2 chars are "0x"
    if (*ptr == '0')
    {
        ptr++;
        counter--;
        if (*ptr == 'x')
        {
            hex = 1; // true
            counter--;
            ptr++;
        }
    }

    int result = 0;
    int factor = (hex == 0) ? DECIMAL : HEX;

    while (counter != 0)
    {
        if (*ptr <= '9' && *ptr >= '0') // if 0-9
        {
            result *= factor;
            result += *ptr % '0'; // add to result
        }
        else if (hex == 1 && *ptr >= 'a' && *ptr <= 'f') // hex is valid AND if a-f
        {
            result *= factor;
            result += *ptr % ('a' - 10); // add to result (97 (a) % 87 = 10)
        }
        else if (hex == 1 && *ptr >= 'A' && *ptr <= 'F') // hex is valid AND if a-f
        {
            result *= factor;
            result += *ptr % ('A' - 10); // add to result (65 (A) % 55 = 10)
        }
        else // anything else would be an invalid num therefore break
        {
            break;
        }

        counter--;
        ptr++;
    }

    if (endptr != NULL)
    {
        *endptr = ptr;
    }

    return result;
}