// Draw enough text on the screen to cause the console to scroll

#include "test-framework.h"
#include "console_ref.h"
#include "gl_ref.h"

#define FINAL_STR "Final"
#define CONSOLE_WIDTH (5)
#define CONSOLE_HEIGHT (2)

void run_test(void) {
#   ifdef SOLUTION
    ref_console_init(CONSOLE_HEIGHT, CONSOLE_WIDTH);
    color_t background_color = ref_gl_read_pixel(0, 0); // normalize background color
    for (size_t i = 0; i < CONSOLE_HEIGHT; ++i)
        ref_console_printf("%05d", i);
    ref_console_printf(FINAL_STR);

    /* Read the pixel values in the first char row */
    for (size_t i = 0; i < ref_gl_get_width(); ++i) {
        for (size_t j = 0; j < ref_gl_get_height(); ++j) {
            trace("pixel (%d, %d) has color: %d\n", 
                          i,
                          j,
                          ref_gl_read_pixel(i, j) != background_color
            );
        }
    }

#   else
    console_init(CONSOLE_HEIGHT, CONSOLE_WIDTH);
    color_t background_color = gl_read_pixel(0, 0); // normalize background color
    for (size_t i = 0; i < CONSOLE_HEIGHT; ++i)
        console_printf("%05d", i);
    console_printf(FINAL_STR);

    /* Read the pixel values in the first char row */
    for (size_t i = 0; i < gl_get_width(); ++i) {
        for (size_t j = 0; j < gl_get_height(); ++j) {
            trace("pixel (%d, %d) has color: %d\n", 
                          i,
                          j,
                          gl_read_pixel(i, j) != background_color
            );
        }
    }

#   endif

    // /* Read the pixel values in the first char row */
    // for (size_t i = 0; i < ref_gl_get_width(); ++i) {
    //     for (size_t j = 0; j < ref_gl_get_height(); ++j) {
    //         trace("pixel (%d, %d) has color: %d\n", 
    //                       i,
    //                       j,
    //                       ref_gl_read_pixel(i, j) != background_color
    //         );
    //     }
    // }
}
