// Format strings that include a mix of specifiers.

#include "test-framework.h"
// Other header files that need to be included.

#ifdef SOLUTION
    static int (*test_fn)(char*, size_t, const char*, ...) = ref_snprintf;
#else
    static int (*test_fn)(char*, size_t, const char*, ...) = snprintf;
#endif

void run_test(void) {
    char buf[128];

    test_fn(
            buf, 
            sizeof(buf),
            "%s %c",
            "Hello",
            '!'
    );
    trace("buf=\"%s\"\n", buf);
    trace_separator();

    test_fn(
            buf, 
            sizeof(buf),
            "%d %x",
            107,
            0x107
    );
    trace("buf=\"%s\"\n", buf);
    trace_separator();

    test_fn(
            buf, 
            sizeof(buf),
            "%d%%",
            100
    );
    trace("buf=\"%s\"\n", buf);
    trace_separator();

    test_fn(
            buf, 
            sizeof(buf),
            "I'd like %d %s please.",
            81941,
            "apples"
    );
    trace("buf=\"%s\"\n", buf);
    trace_separator();

    test_fn(
            buf, 
            sizeof(buf),
            "%p is an important %s.",
            (void *)0x8000,
            "address"
    );
    trace("buf=\"%s\"\n", buf);
    trace_separator();
}
