#include "backtrace.h"
#include "printf.h"

const char *name_of(uintptr_t fn_start_addr)
{
    uint32_t value = *(uint32_t *)(fn_start_addr - 4); // 4 bytes before the first instruction

    if ((value >> 24) == 0xff) // most significant byte
    {
        uint32_t length = (value & ~0xff000000);         // name has length
        char *name = (char *)(fn_start_addr)-length - 4; // name is at start address - length - 4
        return name;
    }
    else
    {
        return "???"; // no name for a function
    }
}

int backtrace(frame_t f[], int max_frames)
{
    int counter = 0; // num of stack frames

    uintptr_t *cur_fp;
    __asm__("mov %0, fp"
            : "=r"(cur_fp)); // cur_fp stored in register r11 and points to pc of function called from

    for (int i = 0; i < max_frames; i++)
    {
        uintptr_t *previous_fp = (uintptr_t *)(*(cur_fp - 3));

        if (previous_fp == NULL) // if previous_fp is at _cstart: return
        {
            return counter;
        }

        uintptr_t lr = *(cur_fp - 1); // -1 is -4 bytes b/c type is 4 bytes
        uintptr_t previous_pc = *previous_fp;

        frame_t frame_data =
            {
                .resume_addr = lr,                      // lr stores address to return to
                .resume_offset = lr - previous_pc + 12, // previous_pc + 12 = call of current function
                .name = name_of(previous_pc - 12)

            };
        f[i] = frame_data;

        cur_fp = previous_fp; // move cur_fp up to the previous_fp
        counter++;
    }

    return counter;
}

void print_frames(frame_t f[], int n)
{
    for (int i = 0; i < n; i++)
        printf("#%d 0x%x at %s+%d\n", i, f[i].resume_addr, f[i].name, f[i].resume_offset);
}

void print_backtrace(void)
{
    int max = 50;
    frame_t arr[max];

    int n = backtrace(arr, max);
    print_frames(arr + 1, n - 1); // print frames starting at this function's caller
}
