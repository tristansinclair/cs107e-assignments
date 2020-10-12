// Attempt to set pin 30 to an invalid function.

#include "test-framework.h"
#include "gpio_ref.h"

#ifdef SOLUTION
    static void (*test_fn)(unsigned int, unsigned int) = ref_gpio_set_function;
#else
    static void (*test_fn)(unsigned int, unsigned int) = gpio_set_function;
#endif

void run_test(void) {
    unsigned int pin = GPIO_PIN30;
    unsigned int funcs[] = {
        8,
        13
    };
    for (int i = 0; i < sizeof(funcs)/sizeof(*funcs); i++) {
        test_fn(pin, funcs[i]);
        trace("pin=%02d, expected function=%02d, actual function=%02d\n", 
                pin, funcs[i], ref_gpio_get_function(pin));
    }
}
