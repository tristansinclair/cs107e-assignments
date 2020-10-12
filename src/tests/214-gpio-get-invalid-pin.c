// Attempt to read function of two invalid pins

#include "test-framework.h"
#include "gpio_ref.h"

#ifdef SOLUTION
    static unsigned int (*test_fn)(unsigned int) = ref_gpio_get_function;
#else
    static unsigned int (*test_fn)(unsigned int) = gpio_get_function;
#endif

void run_test(void) {
    unsigned int pins[] = {
        54,
        100
    };
    unsigned int func = GPIO_FUNC_OUTPUT;
    for (int i = 0; i < COUNT(pins); i++) {
        ref_gpio_set_function(pins[i], func);
        trace("pin=%02d, expected function=%02d, actual function=%02d\n", 
                pins[i], ref_gpio_get_function(pins[i]), test_fn(pins[i]));
    }
}
