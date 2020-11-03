#include "gl.h"
#include "strings.h"
#include "font.h"
#include "malloc.h"

void gl_init(unsigned int width, unsigned int height, gl_mode_t mode)
{
    fb_init(width, height, 4, mode); // use 32-bit depth always for graphics library
}

void gl_swap_buffer(void)
{
    fb_swap_buffer();
}

// physical width
unsigned int gl_get_width(void)
{
    return fb_get_width();
}

// physical height
unsigned int gl_get_height(void)
{
    return fb_get_height();
}

color_t gl_color(unsigned char r, unsigned char g, unsigned char b)
{
    // b g r a
    color_t color = 0;
    color |= (0xff << 24); // brightness
    color |= (r << 16);
    color |= (g << 8);
    color |= b;

    return color;
}

void gl_clear(color_t c)
{
    //gl_draw_rect(0, 0, gl_get_width(), gl_get_height(), c);

    unsigned int pixels_per_row = fb_get_pitch() / fb_get_depth();
    unsigned int(*fb)[pixels_per_row] = fb_get_draw_buffer();

    int max_height = gl_get_height();
    int max_width = gl_get_width();

    for (int y = 0; y < max_height; y++)
    {
        for (int x = 0; x < max_width; x++)
        {
            fb[y][x] = c;
        }
    }
}

void gl_draw_pixel(int x, int y, color_t c)
{
    if (x < 0 || y < 0 || x >= gl_get_width() || y >= gl_get_height()) // throw out out of bounds draws
        return;

    unsigned int pixels_per_row = fb_get_pitch() / fb_get_depth();
    unsigned int(*fb)[pixels_per_row] = fb_get_draw_buffer();

    fb[y][x] = c;
}

color_t gl_read_pixel(int x, int y)
{
    if (x < 0 || y < 0 || x >= gl_get_width() || y >= gl_get_height()) // throw out out of bounds reads
        return 0;

    unsigned int pixels_per_row = fb_get_pitch() / fb_get_depth();
    unsigned int(*fb)[pixels_per_row] = fb_get_draw_buffer();
    return fb[y][x];
}

void gl_draw_rect(int x, int y, int w, int h, color_t c)
{
    int max_height = gl_get_height();
    int max_width = gl_get_width();

    if (x < 0 || y < 0 || x >= max_width || y >= max_height || w < 0 || h < 0) // throw out out of bounds draws
        return;

    w = x + w <= max_width ? w : max_width - x;
    h = y + h <= max_height ? h : max_height - y;

    unsigned int pixels_per_row = fb_get_pitch() / fb_get_depth();
    unsigned int(*fb)[pixels_per_row] = fb_get_draw_buffer();

    for (int _y = y; _y < (y + h); _y++)
    {
        for (int _x = x; _x < (x + w); _x++)
        {
            fb[_y][_x] = c;
        }
    }
}

static void *char_buf;

void gl_draw_char(int x, int y, int ch, color_t c)
{
    int font_width = font_get_width();
    int font_height = font_get_height();

    int max_width = gl_get_width();
    int max_height = gl_get_height();

    if (x < 0 || y < 0 || x >= max_width || y >= max_height) // throw out out of bounds draws
        return;

    if (char_buf == NULL)
    {
        char_buf = malloc(font_get_size());
    }

    font_get_char(ch, (unsigned char *)char_buf, font_get_size());

    unsigned char(*char_buf_2d)[font_width] = char_buf;

    unsigned int pixels_per_row = fb_get_pitch() / fb_get_depth();
    unsigned int(*fb)[pixels_per_row] = fb_get_draw_buffer();

    for (int _y = y; _y < (y + font_height); _y++)
    {
        for (int _x = x; _x < (x + font_width); _x++)
        {
            if (char_buf_2d[_y - y][_x - x] == 0xff)
            {
                fb[_y][_x] = c;
            }
        }
    }
}

void gl_draw_string(int x, int y, const char *str, color_t c)
{
    int font_width = font_get_width();
    int _x = x;
    char *_str = (char *)str;
    while (*_str)
    {
        gl_draw_char(_x, y, *_str, c);
        _str++;           // move to next char
        _x += font_width; // move x for printing
        // add wrap?
    }
}

unsigned int gl_get_char_height(void)
{
    return font_get_height();
}

unsigned int gl_get_char_width(void)
{
    return font_get_width();
}

void gl_draw_line(int x1, int y1, int x2, int y2, color_t c)
{
    int dx = x2 - x1;
    int dy = y2 - y1;

    // int x = 0;
    // int y = 0;
    int y = 0;

    for (int x = x1; x < x2; x++)
    {
        y = y1 + dy * (x - x1) / dx;
        gl_draw_pixel(x, y, c);
    }
}