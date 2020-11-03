#include "console.h"
#include <stdarg.h>
#include "gl.h"
#include "font.h"
#include "printf.h"
#include "strings.h"
#include "timer.h"

static void process_char(char ch);

// struct cursor {
//     int x;
//     int y;
// }
volatile static int cur_x;
volatile static int cur_y;

void console_init(unsigned int nrows, unsigned int ncols)
{
    // TODO: implement this function
    // call `process_char` to silence the compiler's warning
    // that it's defined but not used
    // once you use `process_char` elsewhere, you can delete
    // this line of code

    // unsigned int max_heigth = gl_get_height();
    // unsigned int max_width = gl_get_width();

    // unsigned int height = (nrows * font_get_height()) < max_heigth ? (nrows * font_get_height()) : max_heigth;
    // unsigned int width = (ncols * font_get_width()) < max_width ? (ncols * font_get_width()) : max_width;

    unsigned int height = nrows * font_get_height();
    unsigned int width = ncols * font_get_width();

    cur_x = 0;
    cur_y = 0;

    gl_init(width, height, GL_DOUBLEBUFFER);
}

void console_clear(void)
{
    // TODO: implement this function
}

int console_printf(const char *format, ...)
{
    char buf[80]; // MAX_OUTPUT_LEN!!!!!?????
    va_list va;
    va_start(va, format);
    const int length = vsnprintf(buf, 1024, format, va);
    va_end(va);

    int string_length = strlen(buf);

    for (int i = 0; i < string_length; i++)
    {
        process_char(buf[i]);
    }

    //gl_draw_string(cur_x, cur_y, buf, GL_GREEN);
    printf("console printf: %s\n", buf);

    return length;
}

static void process_char(char ch)
{
    // TODO: implement this helper function (recommended)
    // if ordinary char: inserts ch into contents at current position
    // of cursor, cursor advances one position
    // if special char: (\r \n \f \b) handle according to specific function

    printf("char value = %d", (int)ch);
    if (('a' <= ch && ch <= 'z') || ('A' <= ch && ch <= 'Z'))
    {
        printf("char = %c\n", ch);
        gl_draw_char(cur_x, cur_y, ch, GL_GREEN);
        cur_x += font_get_width();
    }
    else if (ch == '\r')
    {
        //gl_draw_char(cur_x, cur_y, ch, GL_GREEN);
        printf("yay for return\n");
        cur_x = 0;
    }
    else if (ch == '\n')
    {
        printf("yay for newline\n");
        //gl_draw_char(cur_x, cur_y, ch, GL_GREEN);
        cur_x = 0;
        cur_y += font_get_height();
    }
    else if (ch == '\b')
    {
        //gl_draw_char(cur_x, cur_y, ch, GL_GREEN);
        cur_x -= font_get_width();
        //cur_y += font_get_height();
    }
}
