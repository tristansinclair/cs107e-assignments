#include "assert.h"
#include "console.h"
#include "fb.h"
#include "gl.h"
#include "printf.h"
#include "strings.h"
#include "timer.h"
#include "uart.h"

#define _WIDTH 640
#define _HEIGHT 512

#define _NROWS 10
#define _NCOLS 20

static const color_t WHITE = 0xffffffff;
void test_fb(void)
{
    fb_init(700, 700, 4, FB_SINGLEBUFFER);
    printf("Framebuffer successfully configured.\n");
    printf("physical size = %d x %d\n", fb_get_width(), fb_get_height());
    printf("depth = %d bits\n", fb_get_depth());
    printf("pitch = %d bytes per row \n", fb_get_pitch());
    printf("buffer address = %p\n", fb_get_draw_buffer());

    unsigned char *cptr = fb_get_draw_buffer();
    printf("cptr address = %p\n", cptr);

    int nbytes = fb_get_pitch() * fb_get_height();
    memset(cptr, 0x99, nbytes); // fill entire framebuffer with light gray pixels

    //draw_square(0, 0, 200, 200, WHITE);
    timer_delay(3);
}

void test_gl_rectangles(void)
{
    gl_init(_WIDTH, _HEIGHT, GL_DOUBLEBUFFER);

    gl_clear(0xffffffff + (_WIDTH / 4));

    for (int a = (_WIDTH / 2); a > 0; a--)
    {
        gl_draw_rect(((_WIDTH / 2) - (a / 2)), ((_HEIGHT / 2) - (a / 2)), a, a, 0xffffffff + (a / 2));
    }
    timer_delay(2);

    /* OUT OF BOUNDS TESTS */

    // negative x, y, w and h
    gl_draw_rect(-20, 0, 100, 100, 0xffffffff);
    gl_draw_rect(0, -100, 100, 100, 0xffffffff);
    gl_draw_rect(0, 0, -100, 100, 0xffffffff);
    gl_draw_rect(0, 0, 100, -100, 0xffffffff);

    gl_swap_buffer();
    // over x and over y
    gl_draw_rect(0, 250, _WIDTH + 1000, 100, 0xffffffff);
    timer_delay(2);
    gl_draw_rect(0, 350, 250, _HEIGHT + 20000, 0xffffffff);
    timer_delay(2);
    gl_swap_buffer();

    gl_draw_char(60, 10, 'A', GL_AMBER);
    gl_draw_char(80, 10, 'B', GL_AMBER);
    gl_draw_char(100, 10, 'C', GL_AMBER);

    gl_draw_string(60, 40, "ABC", GL_AMBER);

    gl_swap_buffer();
}

void test_gl(void)
{
    // Double buffer mode, make sure you test single buffer too!
    gl_init(_WIDTH, _HEIGHT, GL_DOUBLEBUFFER);

    // Background is purple
    gl_clear(gl_color(0x55, 0, 0x55));

    // Draw green pixel at an arbitrary spot
    gl_draw_pixel(_WIDTH / 3, _HEIGHT / 3, GL_GREEN);
    assert(gl_read_pixel(_WIDTH / 3, _HEIGHT / 3) == GL_GREEN);

    // Blue rectangle in center of screen
    gl_draw_rect(_WIDTH / 2 - 50, _HEIGHT / 2 - 50, 100, 100, GL_BLUE);

    // Single amber character
    gl_draw_char(60, 10, 'A', GL_AMBER);

    // Show buffer with drawn contents
    gl_swap_buffer();
    timer_delay(3);
}

void hello_c(void)
{
    gl_init(1280, 1024, GL_DOUBLEBUFFER);

    while (1)
    {
        gl_clear(GL_BLACK);
        gl_draw_string(20, 20, "hello, world 0", GL_WHITE);
        gl_swap_buffer();

        gl_clear(GL_BLACK);
        gl_draw_string(20, 20, "hello, world 1", GL_WHITE);
        gl_swap_buffer();
    }
}

void test_gl_extra2(void)
{
    gl_init(1280, 1024, GL_DOUBLEBUFFER);

    int counter = 0;
    while (counter < 100)
    {
        gl_clear(GL_BLACK);
        gl_draw_string(20, 20, "hello, world 0", GL_WHITE);
        gl_swap_buffer();

        gl_clear(GL_BLACK);
        gl_draw_string(20, 20, "hello, world 1", GL_WHITE);
        gl_swap_buffer();
        counter++;
    }
}

void test_console(void)
{
    console_init(_NROWS, _NCOLS);

    // 1: "HELLO WORLD"
    console_printf("HELLO WORLD\r");
    timer_delay(1);

    // 1: "HAPPY WORLD"
    // 2: "CODING"
    console_printf("HAPPY\nCODING\n");
    timer_delay(1);

    // Clear
    console_printf("\f");

    // 1: "Goodbye"
    console_printf("Goodbye!\n");
    console_printf("ABCDEFGHIJKLMNOPQRSTUV\n");
    console_printf("!@#$%%^&*()_+-=[]{}:\";'<>?,./`~");
}

void test_console_etxra(void)
{
    console_init(_NROWS, _NCOLS);

    // 1: "HELLO WORLD"
    console_printf("HELLO WORLD\n");
    console_printf("HELLO WORLD 2\n");
    console_printf("HELLO WORLD 3 HELLO WORLD 3 HELLO WORLD 3\n");
    // console_printf("HELLO WORLD 4\n");
    // console_printf("HELLO WORLD 5\n");
    // console_printf("HELLO WORLD 6\n");
    // console_printf("HELLO WORLD 7\n");
    // console_printf("HELLO WORLD 8\n");
    // console_printf("HELLO WORLD 9\n");
    // timer_delay(1);
    // console_printf("HELLO WORLD 10\n");

    // timer_delay(3);
    // //console_printf("12345678901234567890123456\n");
    // //timer_delay(3);
    // console_printf("HELLO WORLD 11\n");
    // timer_delay(3);
    // console_printf("HELLO WORLD 12\n");
    // timer_delay(3);
    // console_printf("12345678901234567890123456\n");
    // console_printf("HELLO WORLD 13\n");
    // timer_delay(1);
    // console_printf("HELLO WORLD 14\n");
    // timer_delay(1);
    // console_printf("HELLO WORLD 15\n");
    // timer_delay(1);
}

/* TODO: Add tests to test your graphics library and console.
   For the graphics library, test both single & double
   buffering and confirm all drawing is clipeed to bounds
   of framebuffer
   For the console, make sure to test wrap-around and scrolling.
   Be sure to test each module separately as well as in combination
   with others.
*/

void main(void)
{
    uart_init();
    timer_init();
    printf("Executing main() in test_gl_console.c\n");

    //test_gl_rectangles();
    //test_fb();
    //test_gl_extra2();
    //test_gl();
    //test_console();
    test_console_etxra();

    //hello_c();

    printf("Completed main() in test_gl_console.c\n");
    uart_putchar(EOT);
}
