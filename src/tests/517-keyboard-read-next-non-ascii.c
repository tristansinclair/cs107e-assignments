// Read the scancodes in
// $CS107E_TESTS_REPO/keyboard-emulator/scancode-sequences/scancode-sequence-517.h
// sent by the keyboard emulator using keyboard_read_event.

#include "test-framework.h"

void run_test(void) {
#   define NUM_CHARS 64
    read_keyboard_emulator_chars(NUM_CHARS);
}
