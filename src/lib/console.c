#include "console.h"
#include <stdarg.h>
#include "gl.h"
#include "font.h"
#include "printf.h"
#include "strings.h"
#include "timer.h"
#include "malloc.h"
#include "assert.h"

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

static unsigned int line_count = 0;
static unsigned int tail = 0;
static unsigned int MAX_LINES = 0;

// for tracking line history
static char *display_history;
static char *cur_char_history;

#define PADDING 2

void console_init(unsigned int nrows, unsigned int ncols)
{
    FONT_WIDTH = font_get_width();
    FONT_HEIGHT = font_get_height();

    MAX_WIDTH = ncols * (FONT_WIDTH);
    MAX_HEIGHT = nrows * (FONT_HEIGHT + PADDING);

    cur_x = 0;
    cur_y = 0;

    MAX_LINES = nrows;
    display_history = malloc((MAX_LINES)*80);
    cur_char_history = display_history;

    gl_init(MAX_WIDTH, MAX_HEIGHT, GL_DOUBLEBUFFER);
}

void console_clear(void)
{
    gl_draw_rect(0, 0, MAX_WIDTH, MAX_HEIGHT, 0x0);
}

void print_history(void)
{
    for (int i = 0; i < 10; i++)
    {
        printf("index: %d   %s \n", i, display_history + (80 * i));
    }
    printf("TAIL index: %d\n", tail);
}

static void scroll_down(void)
{
    int new_x = 0;
    int new_y = 0;

    char *scroll_ptr = display_history;
    scroll_ptr += 80;
    scroll_ptr += tail * 80;

    console_clear();

    for (int i = 0; i < MAX_LINES; i++)
    {
        gl_draw_string(new_x, new_y, scroll_ptr, GL_GREEN);
        new_y += FONT_HEIGHT + 2;
        scroll_ptr += 80;
    }
}

void store_line(void)
{
    *cur_char_history = '\0'; // close current string
    //printf("cur_char_history: %s\n", display_history + (80 * tail));

    // move to the next index
    line_count++;
    tail = line_count % MAX_LINES;

    cur_char_history = display_history + (80 * tail);
    *cur_char_history = '\0'; // close the new buf to be empty

    print_history();
}

void next_line(void)
{
    store_line();

    if (line_count >= MAX_LINES)
    {
        scroll_down();
    }

    // move cursor
    cur_x = 0;
    cur_y += FONT_HEIGHT + PADDING;
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

    printf("console printf: %s\n", buf);

    return length;
}

static void process_char(char ch)
{
    if (cur_x >= MAX_WIDTH)
    {
        next_line();
    }
    if (cur_y >= MAX_WIDTH)
    {
        scroll_down();
    }

    //if (('a' <= ch && ch <= 'z') || ('A' <= ch && ch <= 'Z'))
    if (32 <= ch && ch <= 126)
    {
        gl_draw_char(cur_x, cur_y, ch, GL_GREEN);
        cur_x += FONT_WIDTH;

        // add char to the history
        *cur_char_history = ch;
        cur_char_history++;
    }
    else if (ch == '\r')
    {
        next_line();
    }
    else if (ch == '\n')
    {
        next_line();
    }
    else if (ch == '\b')
    {
        //gl_draw_char(cur_x, cur_y, ch, GL_GREEN);
        cur_x -= font_get_width();
        //cur_y += font_get_height();
    }
}
