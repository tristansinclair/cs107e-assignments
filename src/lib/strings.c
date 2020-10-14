#include "strings.h"

void *memset(void *s, int c, size_t n) // write n bytes of c to *s
{
    unsigned char *ptr = s; // initialize ptr to point to same spot as s

    while (n != 0)
    {
        *ptr = (char)c; // the byte we're looking at = c (in char form)
        ptr++;          // adds 1 byte too address in ptr
        n--;            // increment count
    }

    return s;
}

void *memcpy(void *dst, const void *src, size_t n)
{
    unsigned char *ptr = dst;        // point ptr to dst
    unsigned const char *ptr2 = src; // point ptr2 to src

    while (n != 0)
    {
        *ptr = *ptr2; // copy byte from src to dst
        ptr++;
        ptr2++;
        n--;
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
    int val = 0;
    while (*s1 != '\0' && *s2 != '\0' && *s1 == *s2) // while not at end of string and they equal eachother incremement
    {
        s2++; // incremment 1 byte
        s1++;
    }
    if (*s1 > *s2)
    {
        val = 1;
    }
    else if (*s1 < *s2)
    {
        val = -1;
    }

    return val;
}

size_t strlcat(char *dst, const char *src, size_t maxsize)
{
    int counter = 0;
    char *myptr = dst; // point to beginning of dst

    while (*myptr != '\0' && counter != maxsize)
    { // don't assume null in dst
        myptr++;
        counter++;
    }
    if (counter == maxsize)
    { // if there wasn't a null ptr
        return maxsize + strlen(src);
    }

    while (*src != '\0' && maxsize - strlen(dst) - 1 != 0)
    {
        *myptr = *src;
        myptr++;
        src++;
    }
    *myptr = '\0';

    return strlen(dst); // returns length of the FINAL string
}

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
    int factor = (hex == 0) ? 10 : 0x10;

    while (counter != 0)
    {
        if (*ptr < 58 && *ptr > 47) // if 0-9
        {
            result *= factor;
            result += *ptr % 48; // add to result
        }
        else if (hex == 1 && *ptr < 103 && *ptr >= 97) // hex is valid AND if a-f
        {
            result *= factor;
            result += *ptr % 87; // add to result (97 (a) % 87 = 10)
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