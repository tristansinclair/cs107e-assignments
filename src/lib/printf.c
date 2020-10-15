#include "printf.h"
#include "strings.h"
#include "printf_internal.h"
#include <stdarg.h>

#define MAX_OUTPUT_LEN 1024

/*
 * int_length
 * 
 * returns number of digits in an int
 * +1 if negative (-21 == 3)//////////////
*/
static int int_length(int num, int base)
{
    int counter = 0;
    if (num < 0)
    {
        //counter++;
        num *= -1;
    }
    while (num >= base)
    {
        counter++;
        num /= base;
    }
    counter++;

    return counter;
}

static void reverse(char *str)
{
    char *beg = str;
    char *end = beg + strlen(str) - 1;

    while (beg < end)
    {
        char temp = *end;
        *end = *beg;
        *beg = temp;
        beg++;
        end--;
    }
}

int unsigned_to_base(char *buf, size_t bufsize, unsigned int val, int base, int min_width)
{
    if (bufsize == 0) // if we aren't supposed to write here, just return 0
    {
        return 0;
    }

    int counter = 0;

    const int length = int_length(val, base);

    while ((min_width > length + counter) && (bufsize > counter + 1)) // leave space for '\n'
    {
        *buf = '0';
        buf++;
        counter++;
    }

    *buf = '\0';
    buf -= counter; //  set buf ptr back to beginning

    char str[length + 1];

    int num = 0;
    char c = 0;

    for (int i = 0; i < length; i++)
    {
        num = val % base;
        c = num < 10 ? num + 48 : num + 97 - 10;

        str[i] = c;
        val /= base;
    }
    str[length] = '\0';

    reverse(str);

    int finallength = strlcat(buf, str, bufsize);

    if (min_width > finallength)
    {
        return min_width; // if there are zeros taking up the whole string from min width
    }

    return finallength;
}

int signed_to_base(char *buf, size_t bufsize, int val, int base, int min_width)
{
    if (val < 0)
    {
        if (bufsize > 1)
        {
            *buf = '-';
            buf++;
            bufsize--;
            min_width--;
        }
        //*buf = '\0';

        val *= -1;

        return unsigned_to_base(buf, bufsize, val, base, min_width) + 1;
    }
    else
    {
        return unsigned_to_base(buf, bufsize, val, base, min_width);
    }
}

int vsnprintf(char *buf, size_t bufsize, const char *format, va_list args)
{
    // va_start(args, n);

    // va_end(args);
    return 0;
}

int snprintf(char *buf, size_t bufsize, const char *format, ...)
{
    /* TODO: Your code here */
    return 0;
}

int printf(const char *format, ...)
{
    /* TODO: Your code here */
    return 0;
}
