#include "console.h"
#include <stdarg.h>
#include "gl.h"
#include "font.h"
#include "printf.h"
#include "strings.h"
#include "malloc.h"

static void process_char(char ch);

static unsigned int cur_x;
static unsigned int cur_y;

static unsigned int MAX_LINES; // max lines of the display
static unsigned int MAX_WIDTH; // pixels of display
static unsigned int MAX_HEIGHT;
static unsigned int FONT_WIDTH;
static unsigned int FONT_HEIGHT;

static unsigned int line_count = 0; // current total line count
static unsigned int tail = 0;       // last line wrote

// for tracking line history
static char *display_history; // holds the line history
static char *cur_char_history;

static unsigned int MAX_CHARS_PER_LINE; // max chars in a line
static unsigned int char_count = 0;     // current chars in a line

#define MAX_CHARS 84 // 80 for console + for for "pi> " to fix issues

#define BACKGROUND_COLOR 0x0  // black
#define TEXT_COLOR 0xFF00FF00 // GL_GREEN

void console_init(unsigned int nrows, unsigned int ncols)
{
    FONT_WIDTH = font_get_width();
    FONT_HEIGHT = font_get_height();

    MAX_WIDTH = ncols * (FONT_WIDTH);
    MAX_HEIGHT = nrows * (FONT_HEIGHT);

    cur_x = 0;
    cur_y = 0;

    MAX_CHARS_PER_LINE = ncols;
    MAX_LINES = nrows;
    display_history = malloc((MAX_LINES)*MAX_CHARS);
    cur_char_history = display_history;

    gl_init(MAX_WIDTH, MAX_HEIGHT, GL_DOUBLEBUFFER);
}

void console_clear(void)
{
    gl_draw_rect(0, 0, MAX_WIDTH, MAX_HEIGHT, BACKGROUND_COLOR); // fill screen with background
}

/*
 * print_history
 * used for debugging 
 * prints index and the current string stored in history
 */
// static void print_history(void)
// {
//     for (int i = 0; i < MAX_LINES; i++)
//     {
//         printf("index: %d   %s \n", i, display_history + (MAX_CHARS_PER_LINE * i));
//     }
//     printf("TAIL index: %d\n", tail);
// }

/*
 * console_printf
 * print to console via hdmi
 */
int console_printf(const char *format, ...)
{
    char buf[1024];
    va_list va;
    va_start(va, format);
    const int length = vsnprintf(buf, 1024, format, va);
    va_end(va);

    int string_length = strlen(buf);

    for (int i = 0; i < string_length; i++)
    {
        process_char(buf[i]);
    }

    return length;
}

/*
 * shift_up
 * shifts the display up by 1 char
 * copies extra copy to hidden buffer
 * to prep for quick swap
 */
static void shift_up(void)
{
    console_clear(); // clear the hidden buffer

    line_count++;
    tail = (tail + 1) % MAX_LINES;

    int new_y = 0;

    int temp_tail = tail + 1;

    char *shift_ptr = display_history + (temp_tail * MAX_CHARS);

    for (int i = 0; i < MAX_LINES - 1; i++)
    {
        gl_draw_string(0, new_y, shift_ptr, TEXT_COLOR);
        new_y += FONT_HEIGHT;
        temp_tail = (temp_tail + 1) % MAX_LINES;
        shift_ptr = display_history + (MAX_CHARS * temp_tail);
    }

    gl_swap_buffer(); // display shifted buffer

    // prep the next buffer
    console_clear();

    new_y = 0;
    temp_tail = tail + 1;
    shift_ptr = display_history + (temp_tail * MAX_CHARS);

    for (int i = 0; i < MAX_LINES - 1; i++)
    {
        gl_draw_string(0, new_y, shift_ptr, TEXT_COLOR);
        new_y += FONT_HEIGHT;
        temp_tail = (temp_tail + 1) % MAX_LINES;
        shift_ptr = display_history + (MAX_CHARS * temp_tail);
    }

    cur_char_history = display_history + (tail * MAX_CHARS);
    char_count = 0;

    cur_x = 0; // move x back to start
}

static void next_line(void)
{
    cur_y += FONT_HEIGHT;
    cur_x = 0;

    line_count++;
    char_count = 0;
    tail = (tail + 1) % MAX_LINES;
    cur_char_history = display_history + (tail * MAX_CHARS);
}

/*
 * standard_char
 * handles print of a standard ascii char
 */
static void standard_char(char ch)
{
    // add it to the history
    *cur_char_history = ch;
    cur_char_history++;
    *cur_char_history = '\0';

    // print it to the hidden buffer
    gl_draw_char(cur_x, cur_y, ch, TEXT_COLOR);
    gl_swap_buffer();                           // display
    gl_draw_char(cur_x, cur_y, ch, TEXT_COLOR); // copy to hidden buffer

    cur_x += FONT_WIDTH;
    char_count++;
}

/*
 * backspace
 * handles backspace key
 * currently does not support multi line backspace
 */
static void backspace(void)
{
    cur_char_history--;
    *cur_char_history = '\0';
    char_count--;

    cur_x -= FONT_WIDTH;
    gl_draw_rect(cur_x, cur_y, FONT_WIDTH, FONT_HEIGHT, 0x0);
    gl_swap_buffer();
    gl_draw_rect(cur_x, cur_y, FONT_WIDTH, FONT_HEIGHT, 0x0);
}

/*
 * formfeed
 * complete reset of the display and console
 */
static void formfeed(void)
{
    // reset variables
    cur_x = 0;
    cur_y = 0;
    tail = 0;
    line_count = 0;
    char_count = 0;

    // clear hidden, show, clear hidden
    console_clear();
    gl_swap_buffer();
    console_clear();
}

/*
 * process_char
 * processes a keypress from console_printf
 */
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
    else if (ch == '\f')
    {
        formfeed();
    }
    else if (ch == '\r')
    {
        cur_x = 0;
    }

    else
    {
        // handle wrap/shift
        if (char_count >= MAX_CHARS_PER_LINE)
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
        standard_char(ch);
    }
}