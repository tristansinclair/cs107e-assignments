// Write 8 randomly chosen pixels in the framebuffer.

#include "test-framework.h"
#include "gl_ref.h"
#include "rand_ref.h"

void run_test(void) {
#   ifdef SOLUTION
    ref_gl_init(GL_WIDTH, GL_HEIGHT, GL_SINGLEBUFFER); 
#   else
    gl_init(GL_WIDTH, GL_HEIGHT, GL_SINGLEBUFFER); 
#   endif
    for (unsigned int i = 0; i < 8; i++) {
        unsigned int x = ref_rand() % GL_WIDTH;
        unsigned int y = ref_rand() % GL_HEIGHT;
#       ifdef SOLUTION
        ref_gl_draw_pixel(x, y, GL_INDIGO);
#       else
        gl_draw_pixel(x, y, GL_INDIGO);
#       endif
        trace("grid[%d][%d]=%08x\n", y, x, ref_gl_read_pixel(x, y));
    }
}
