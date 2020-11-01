#include "gl.h"
#include "strings.h"

void gl_init(unsigned int width, unsigned int height, gl_mode_t mode)
{
    fb_init(width, height, 4, mode); // use 32-bit depth always for graphics library
}

void gl_swap_buffer(void)
{
    fb_swap_buffer();
}

unsigned int gl_get_width(void)
{
    return fb_get_width();
}

unsigned int gl_get_height(void)
{
    return fb_get_height();
}

color_t gl_color(unsigned char r, unsigned char g, unsigned char b)
{
    color_t color = 0;
    color |= (0xff << 24); // brightness
    color |= (r << 16);
    color |= (g << 8);
    color |= b;

    return color;
}

void gl_clear(color_t c)
{
    gl_draw_rect(0, 0, gl_get_width(), gl_get_height(), c);
}

void gl_draw_pixel(int x, int y, color_t c)
{
    int pitch = fb_get_pitch() / 4;
    unsigned int(*arr)[pitch] = fb_get_draw_buffer();
    arr[y][x] = c;
}

color_t gl_read_pixel(int x, int y)
{
    unsigned int per_row = fb_get_pitch() / fb_get_depth();
    unsigned int(*im)[per_row] = fb_get_draw_buffer();
    return im[y][x];
}

void gl_draw_rect(int x, int y, int w, int h, color_t c)
{
    // int max_height = gl_get_height();
    // int max_width = gl_get_width();
    {

        for (int _y = y; _y < h; _y++)
        {
            for (int _x = x; _x < w; _x++)
            {
                gl_draw_pixel(_x, _y, c);
            }
        }
    }
}

void gl_draw_char(int x, int y, int ch, color_t c)
{
    // int font_height = font_get_height();
    // int font_width = font_get_width();

    // int fb_width = gl_get_width();
    // int fb_height = gl_get_height();

    return;
}

void gl_draw_string(int x, int y, const char *str, color_t c)
{
    // TODO: implement this function
}

unsigned int gl_get_char_height(void)
{
    // TODO: implement this function
    return 0;
}

unsigned int gl_get_char_width(void)
{
    // TODO: implement this function
    return 0;
}

void gl_draw_line(int x1, int y1, int x2, int y2, color_t c)
{
    int dx = x2 - x1;
    int dy = y2 - y1;

    int x = 0;
    int y = 0;

    for (int x = x1; x < x2; x++)
    {
        y = y1 + dy * (x - x1) / dx;
        gl_draw_pixel(x, y, c);
    }
}