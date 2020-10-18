// Format strings that include the '%s' specifier.

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
            .format = "You finish my %s. Or is it %?",
            .strs = { 
                "sandwiches",
                "pickles"
            }
        },
        {
            .format = "%s%s",
            .strs = { 
                "car",
                "pet"
            }
        },
        {
            .format = "%s shi %s zao.", 
            .strs = {
                "Wo",
                "xi"
            }
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
                arg->strs[0],
                arg->strs[1]
        );
        trace("arg->format=\"%s\"\n", arg->format);
        trace("arg->strs[0]=\"%s\"\n", arg->strs[0]);
        trace("arg->strs[1]=\"%s\"\n", arg->strs[1]);
        trace("buf=\"%s\"\n", buf);
        trace_separator();
    }
}
