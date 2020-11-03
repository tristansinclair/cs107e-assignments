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
static unsigned int cur_x;
static unsigned int cur_y;

static unsigned int MAX_WIDTH;
static unsigned int MAX_HEIGHT;

static unsigned int FONT_WIDTH;
static unsigned int FONT_HEIGHT;

static unsigned int current_line = 0;
static unsigned int MAX_LINES = 0;

#define PADDING 2;

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
    FONT_WIDTH = font_get_width();
    FONT_HEIGHT = font_get_height();

    MAX_WIDTH = ncols * (FONT_WIDTH);
    MAX_HEIGHT = nrows * (FONT_HEIGHT + PADDING);

    cur_x = 0;
    cur_y = 0;

    MAX_LINES = nrows;

    gl_init(MAX_WIDTH, MAX_HEIGHT, GL_DOUBLEBUFFER);
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

static void scroll_down(void)
{
    if (current_line > MAX_LINES)
    {
        unsigned int pixels_per_row = fb_get_pitch() / fb_get_depth();
        unsigned int(*fb)[pixels_per_row] = fb_get_draw_buffer();
        for (int y = 0; y < (MAX_HEIGHT - (FONT_HEIGHT + 2)); y++)
        {
            memcpy((void *)fb[y], (void *)fb[y ], size_t n)
        }
    }
}

static void process_char(char ch)
{
    // TODO: implement this helper function (recommended)
    // if ordinary char: inserts ch into contents at current position
    // of cursor, cursor advances one position
    // if special char: (\r \n \f \b) handle according to specific function

    //printf("char value = %d", (int)ch);

    if (cur_x >= MAX_WIDTH)
    {
        cur_x = 0;
        cur_y += FONT_HEIGHT + PADDING;
        current_line++;
    }
    if (cur_y >= MAX_WIDTH)
    {
        scroll_down();
    }

    //if (('a' <= ch && ch <= 'z') || ('A' <= ch && ch <= 'Z'))
    if (32 <= ch && ch <= 126)
    {
        printf("char = %c\n", ch);
        gl_draw_char(cur_x, cur_y, ch, GL_GREEN);
        cur_x += font_get_width();
    }
    else if (ch == '\r')
    {
        //gl_draw_char(cur_x, cur_y, ch, GL_GREEN);
        //printf("yay for return\n");
        cur_x = 0;
    }
    else if (ch == '\n')
    {
        //printf("yay for newline\n");
        //gl_draw_char(cur_x, cur_y, ch, GL_GREEN);
        cur_x = 0;
        cur_y += font_get_height() + PADDING;
        current_line++;
    }
    else if (ch == '\b')
    {
        //gl_draw_char(cur_x, cur_y, ch, GL_GREEN);
        cur_x -= font_get_width();
        //cur_y += font_get_height();
    }
}
