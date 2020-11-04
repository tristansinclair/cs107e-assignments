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

void refresh(void)
{
    int new_x = 0;
    int new_y = 0;

    // if (line_count >= MAX_LINES)
    // {
    //     new_y = FONT_HEIGHT * (MAX_LINES);
    // }

    char *refresh_ptr = display_history;

    //int reps = line_count < MAX_LINES ? line_count + 1 : MAX_LINES;
    //rintf("line count: %d", line_count);
    if (line_count >= MAX_LINES)
    {
        refresh_ptr = display_history + (((tail + 1) % MAX_LINES) * 80);
    }

    // if (line_count < MAX_LINES)
    // {
    for (int i = 0; i < MAX_LINES; i++)
    {
        //printf("%d refresh: %s\n", i, refresh_ptr);
        gl_draw_string(new_x, new_y, refresh_ptr, GL_GREEN);

        new_y += FONT_HEIGHT;

        refresh_ptr = display_history + (80 * ((i + 1) % MAX_LINES));

        if (line_count >= MAX_LINES)
        {
            refresh_ptr = display_history + (((tail + 1 + i + 1) % MAX_LINES) * 80);
        }
    }
    // }
    // else
    // {
    //     refresh_ptr = display_history + (((tail + 1 + MAX_LINES - 1 + 1) % MAX_LINES) * 80);
    //     //gl_draw_string(new_x, FONT_HEIGHT * (MAX_LINES - 1), refresh_ptr, GL_GREEN);
    //     gl_draw_char(new_x, new_y, *refresh_ptr, GL_GREEN);
    //     refresh_ptr++;
    //     //timer_delay(1);
    // }

    //printf("tail: %d\n", tail);
    //timer_delay(delay);
    gl_swap_buffer();
}

void print_history(void)
{
    for (int i = 0; i < 10; i++)
    {
        //printf("index: %d   %s \n", i, display_history + (80 * i));
    }
    //printf("TAIL index: %d\n", tail);
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
    line_count++;
    console_clear();

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
    //printf("tail: %d\n", tail);
    gl_swap_buffer();

    console_clear();

    // copy over again to 2nd fb for faster speeeeeeeed
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
}

static void next_line(void)
{
    line_count++;
    char_count = 0;
    tail = (tail + 1) % MAX_LINES;
    cur_char_history = display_history + (tail * 80);
}

static void process_char(char ch)
{
    if (ch == '\n')
    {
        if (line_count < MAX_LINES - 1)
        {
            next_line();
            refresh();
        }
        else
        {
            shift_up();
            //console_clear();
        }
    }
    else if (ch == '\b')
    {
        //*cur_char_history = ch;
        cur_char_history--;
        *cur_char_history = '\0';
        refresh();
    }

    else
    {
        if (char_count >= MAX_CHARS)
        {
            if (line_count < MAX_LINES - 1)
            {
                next_line();
                refresh();
            }
            else
            {
                shift_up();
                console_clear();
            }
        }
        // else
        // {
        //     gl_draw_char(cur_x, cur_y, ch, GL_GREEN);
        // }

        *cur_char_history = ch;
        cur_char_history++;
        *cur_char_history = '\0';

        char_count++;
        refresh();
    }
    //printf("line count: %d\n", line_count);
}