// Convert complicated strings to numbers.

#include "test-framework.h"
#include "strings_ref.h"

#ifdef SOLUTION
    static unsigned int (*test_fn)(const char*, const char**) = ref_strtonum;
#else
    static unsigned int (*test_fn)(const char*, const char**) = strtonum;
#endif

void run_test(void) {
    const char *strs[] = {
        "",
        "c94",
        "278 potatoes",
        "8812 yams",
        "517 13",
        "900 potatoes and 014 yams",
        "1888231 tubers",
        "starchy 19299010",
        "0xA",
        "10xa",
        "0x1c mugs",
        "0x2cups",
        "p0xfb01",
        "glasses 0x44a32",
        "0x742gfbe",
        "0x641B1aF",
        "0x990184AC"
    };
    unsigned int n = sizeof(strs)/sizeof(*strs);

    for (unsigned int i = 0; i < n; i++) {
        unsigned int res = test_fn(strs[i], NULL);
        trace("strs[%d]=\"%s\"\n", i, strs[i]);
        trace("res=%d\n", res);
        trace_separator();

        const char *endptr;
        res = test_fn(strs[i], &endptr);
        trace("strs[%d]=\"%s\"\n", i, strs[i]);
        trace("res=%d\n", res);
        trace("endptr=\"%s\"\n", endptr);
        trace_separator();
    }
}

