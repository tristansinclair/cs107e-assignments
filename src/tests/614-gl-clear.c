// Clear the framebuffer in single buffer mode.

#include "test-framework.h"
#include "gl_ref.h"

void run_test(void) {
#   ifdef SOLUTION
    ref_gl_init(GL_WIDTH, GL_HEIGHT, GL_SINGLEBUFFER); 
    ref_gl_clear(GL_MAGENTA);
#   else
    gl_init(GL_WIDTH, GL_HEIGHT, GL_SINGLEBUFFER); 
    gl_clear(GL_MAGENTA);
#   endif
    for (unsigned int y = 0; y < GL_HEIGHT; y++) {
        for (unsigned int x = 0; x < GL_WIDTH; x++) 
            trace("grid[%d][%d]=%08x\n", y, x, ref_gl_read_pixel(x, y));
    }
}
