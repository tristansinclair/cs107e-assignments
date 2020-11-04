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
static char *display_history; // holds the display
static char *cur_char_history;

static unsigned int MAX_CHARS;
static unsigned int char_count = 0;

//static unsigned int refresh_needed = 1;

//static int delay = 0;

void console_init(unsigned int nrows, unsigned int ncols)
{
    FONT_WIDTH = font_get_width();
    FONT_HEIGHT = font_get_height();

    MAX_WIDTH = ncols * (FONT_WIDTH);
    MAX_HEIGHT = nrows * (FONT_HEIGHT);

    cur_x = 0;
    cur_y = 0;

    MAX_CHARS = ncols;
    MAX_LINES = nrows;
    display_history = malloc((MAX_LINES)*80);
    cur_char_history = display_history;

    gl_init(MAX_WIDTH, MAX_HEIGHT, GL_DOUBLEBUFFER);
}

void console_clear(void)
{
    gl_draw_rect(0, 0, MAX_WIDTH, MAX_HEIGHT, 0x0);
}

void refresh_buffer(void)
{
    gl_swap_buffer();
    console_clear();

    int new_x = 0;
    int new_y = 0;

    char *refresh_ptr = display_history;

    if (line_count >= MAX_LINES)
    {
        refresh_ptr = display_history + (((tail + 1) % MAX_LINES) * 80);
    }

    for (int i = 0; i < MAX_LINES; i++)
    {
        gl_draw_string(new_x, new_y, refresh_ptr, GL_GREEN);

        new_y += FONT_HEIGHT;

        refresh_ptr = display_history + (80 * ((i + 1) % MAX_LINES));

        if (line_count >= MAX_LINES)
        {
            refresh_ptr = display_history + (((tail + 1 + i + 1) % MAX_LINES) * 80);
        }
    }
}

void print_history(void)
{
    for (int i = 0; i < 10; i++)
    {
        printf("index: %d   %s \n", i, display_history + (80 * i));
    }
    printf("TAIL index: %d\n", tail);
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

    //printf("console printf: %s\n", buf);
    //print_history();

    return length;
}

static void shift_up(void)
{
    console_clear(); // clear the hidden buffer

    cur_x = 0;
    cur_y = 0;

    line_count++;
    tail = (tail + 1) % MAX_LINES;

    int new_x = 0;
    int new_y = 0;

    int temp_tail = tail + 1;

    char *shift_ptr = display_history + (temp_tail * 80);

    for (int i = 0; i < MAX_LINES - 1; i++)
    {
        gl_draw_string(new_x, new_y, shift_ptr, GL_GREEN);
        new_y += FONT_HEIGHT;
        temp_tail = (temp_tail + 1) % MAX_LINES;
        shift_ptr = display_history + (80 * temp_tail);
    }

    cur_char_history = display_history + (tail * 80);
    char_count = 0;

    gl_swap_buffer();

    new_x = 0;
    new_y = 0;

    temp_tail = tail + 1;

    shift_ptr = display_history + (temp_tail * 80);

    for (int i = 0; i < MAX_LINES - 1; i++)
    {
        gl_draw_string(new_x, new_y, shift_ptr, GL_GREEN);
        new_y += FONT_HEIGHT;
        temp_tail = (temp_tail + 1) % MAX_LINES;
        shift_ptr = display_history + (80 * temp_tail);
    }

    cur_char_history = display_history + (tail * 80);
    char_count = 0;


    print_history();
    printf("SHIFT UP\n");
}

static void next_line(void)
{
    cur_y += FONT_HEIGHT;
    cur_x = 0;

    line_count++;
    char_count = 0;
    tail = (tail + 1) % MAX_LINES;
    cur_char_history = display_history + (tail * 80);

    print_history();
    printf("NEXT LINE\n");
}

static void standard_char(char ch)
{
    // add it to the history
    *cur_char_history = ch;
    cur_char_history++;
    *cur_char_history = '\0';

    // print it to the hidden buffer
    gl_draw_char(cur_x, cur_y, ch, GL_GREEN);
    gl_swap_buffer();
    gl_draw_char(cur_x, cur_y, ch, GL_GREEN);

    // increment
    cur_x += FONT_WIDTH;
    char_count++;
}

static void backspace(void)
{
    cur_char_history--;
    *cur_char_history = '\0';

    cur_x -= FONT_WIDTH;
    gl_draw_rect(cur_x, cur_y, FONT_WIDTH, FONT_HEIGHT, 0x0);
    gl_swap_buffer();
    gl_draw_rect(cur_x, cur_y, FONT_WIDTH, FONT_HEIGHT, 0x0);z
}

static void process_char(char ch)
{
    if (ch == '\n')
    {
        if (line_count < MAX_LINES - 1)
        {
            next_line();
        }
        else
        {
            shift_up();
        }
    }
    else if (ch == '\b')
    {
        backspace();
    }
    else
    {
        // handle wrap/shift
        if (char_count >= MAX_CHARS)
        {
            if (line_count < MAX_LINES - 1)
            {
                next_line();
            }
            else
            {
                shift_up();
            }
        }
        standard_char(ch); //  draw char

    }
}