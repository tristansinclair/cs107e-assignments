// Format strings that include the '%p' specifier.

#include "test-framework.h"
// Other header files that need to be included.

#ifdef SOLUTION
    static int (*test_fn)(char*, size_t, const char*, ...) = ref_snprintf;
#else
    static int (*test_fn)(char*, size_t, const char*, ...) = snprintf;
#endif

void run_test(void) {
    const printf_args_t args[] = {
        {
            .format = "%p",
            .ptrs = { (void *)0x0 }
        },
        {
            .format = "Here's an important address: %p.",
            .ptrs = { (void *)0x8000 }
        },
        {
            .format = "The address %p is also important.", 
            .ptrs = { (void *)0x20200000 }
        }
    };
    unsigned int n = sizeof(args)/sizeof(*args);

    char buf[128];
    for (unsigned int i = 0; i < n; i++) {
        const printf_args_t *arg = args + i;
        test_fn(
                buf, 
                sizeof(buf),
                arg->format,
                arg->ptrs[0]
        );
        trace("arg->format=\"%s\"\n", arg->format);
        trace("arg->ptrs[0]=%p\n", arg->ptrs[0]);
        trace("buf=\"%s\"\n", buf);
        trace_separator();
    }
}
