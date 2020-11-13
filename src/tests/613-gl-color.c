// Construct colors from separate red, green, and blue components.

#include "test-framework.h"
#include "gl_ref.h"

void run_test(void) {
    for (unsigned char r = 0; r <= 0xff; r++) {
        for (unsigned char g = 0; g <= 0xff; g++) {
            for (unsigned char b = 0; b <= 0xff; b++) {
                trace("r=0x%02x, g=0x%02x, b=0x%02x\n", r, g, b);
#               ifdef SOLUTION
                color_t color = ref_gl_color(r, g, b);
#               else
                color_t color = gl_color(r, g, b);
#               endif
                trace("color=%08x\n", color);
                trace(VISUAL_BREAK);
            }
        }
    }
}

