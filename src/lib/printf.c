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
    int counter = 0; // counter will hold the size of the string we are building
    char *ptr = buf;
    char optionalWidth[3]; // max of optional width of 99 w/ only two characters
    memset(optionalWidth, 0, 3); // set memory to be ""
    int strlength = 0; // used for calculating lengths

    if (bufsize != 0)
    {
        *ptr = '\0';
    }

    int remainingspace = bufsize;

    while (*format)
    {
        if (*format == '%')
        {
            format++;

            if (*format == '0')
            {
                format++;
                optionalWidth[0] = *format;
                format++;
                if (*format < 58 && *format > 47)
                {
                    optionalWidth[1] = *format;
                    format++;
                }
            }

            switch (*format)
            {
            case 'c':
                memset(ptr, va_arg(args, int), 1);
                ptr++;
                *ptr = '\0';
                remainingspace -= 1;
                //counter++;
                break;

            case 's':
                //string = va_arg(args, char *);
                strlength = strlcat(ptr, va_arg(args, char *), remainingspace);
                ptr += strlength;
                counter += (strlength - 1);
                remainingspace -= strlength;
                break;

            case 'd':
                strlength = signed_to_base(ptr, remainingspace, va_arg(args, int), 10, strtonum(optionalWidth, NULL));
                ptr += strlength;
                counter += (strlength - 1);
                memset(optionalWidth, 0, 4);
                break;

            case 'x':
                strlength = unsigned_to_base(ptr, remainingspace, va_arg(args, int), 16, strtonum(optionalWidth, NULL));
                ptr += strlength;
                counter += (strlength - 1);
                memset(optionalWidth, 0, 4);
                break;

            case '%':
                strlength = unsigned_to_base(ptr, remainingspace, va_arg(args, int), 16, 0);
                ptr++;
                break;

            case 'p':
                //*ptr = unsigned;
                strlcat(ptr, "0x", remainingspace);
                ptr += 2;
                strlength = unsigned_to_base(ptr, remainingspace, va_arg(args, int), 16, 0);
                ptr += strlength;
                counter += 2 + strlength - 1; // 0x + 12345678 - counter addition below
                break;
            }
        }
        else
        {
            *ptr = *format;
            ptr++;
        }

        format++;
        counter++;
        *ptr = '\0';
    }
    *ptr = '\0'; //  close the string

    return counter;
}

int snprintf(char *buf, size_t bufsize, const char *format, ...)
{

    va_list va;
    va_start(va, format);
    //char buffer[1];
    const int length = vsnprintf(buf, bufsize, format, va);
    va_end(va);
    return length;
}

int printf(const char *format, ...)
{
    /* TODO: Your code here */
    return 0;
}
