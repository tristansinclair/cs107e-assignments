// Draw four characters in random locations in the framebuffer.

#include "test-framework.h"
#include "gl_ref.h"
#include "rand_ref.h"

void run_test(void) {
#   ifdef SOLUTION
    ref_gl_init(GL_WIDTH, GL_HEIGHT, GL_SINGLEBUFFER); 
#   else
    gl_init(GL_WIDTH, GL_HEIGHT, GL_SINGLEBUFFER); 
#   endif
    color_t color = GL_RED;
    const char *str = "hello world";
    for (const char ch = *str; ch; str++) {
        unsigned int x = ref_rand() % GL_WIDTH;
        unsigned int y = ref_rand() % GL_HEIGHT;
        unsigned width = ref_gl_get_char_width();
        unsigned height = ref_gl_get_char_height();
#       ifdef SOLUTION
        ref_gl_draw_char(x, y, ch, color);
#       else
        gl_draw_char(x, y, ch, color);
#       endif
        unsigned int lh = x, rh = x + width;
        unsigned int top = y, bottom = y + height;

#       ifdef SOLUTION
        for (unsigned int k = top; k < bottom; k++) {
            for (unsigned int j = lh; j < rh; j++) 
                trace("grid[%d][%d]=%d\n", k, j, ref_gl_read_pixel(j, k));
        }
#       else
        for (unsigned int k = top; k < bottom; k++) {
            for (unsigned int j = lh; j < rh; j++) 
                trace("grid[%d][%d]=%d\n", k, j, gl_read_pixel(j, k));
        }
#       endif

    }
}
