#include "assert.h"
#include "printf.h"
#include "printf_internal.h"
#include <stddef.h>
#include "strings.h"
#include "uart.h"

static void test_memset(void)
{
    // hex
    int numA = 0xefefefef;
    int numB = 2;
    memset(&numB, 0xef, sizeof(int));
    assert(numA == numB);

    // decimal
    int numC = 123;
    int numD = 0;
    memset(&numD, 123, 1);
    assert(numC == numD);

    // write zero bytes?
    int numE = 128;
    int numF = 0;
    memset(&numF, 100, 0);
    assert(numF == 0);

    // write 1 byte
    memset(&numF, numE, 1);
    assert(numF == numE);
}

static void test_memcpy(void)
{
    int numA = 0x12345678;
    int numB = 2;

    memcpy(&numB, &numA, sizeof(int));
    assert(numA == numB);

    // zero
    int numC = 0x0;
    int numD = 2;

    memcpy(&numD, &numC, sizeof(int));
    assert(numC == numD);

    // copy array of chars
    char test[5] = "test";
    assert(strcmp(test, "test") == 0);
    assert(strlen(test) == 4);

    char test2[9] = ""; // empty array
    memcpy(&test2, &test, strlen(test) + 1); // copy "test" + '\0' to test2 (5 bytes)
    assert(strlen(test2) == 4);
    assert(strcmp(test, test2) == 0);
}

static void test_strlen(void)
{
    assert(strlen("green") == 5);
    assert(strlen("") == 0); // Empty string
    assert(strlen(" ") == 1);
    assert(strlen("@!-&") == 4);
}

static void test_strcmp(void)
{
    // compares 1 vs. 2
    assert(strcmp("apple", "apple") == 0);
    assert(strcmp("apple", "applesauce") < 0);
    assert(strcmp("pears", "apples") > 0);
    assert(strcmp("p", "a") > 0);
    assert(strcmp("a", "b") < 0);

    // empty str edge cases
    assert(strcmp("", "") == 0);
    assert(strcmp("0", "") > 0);
    assert(strcmp("", "ABC") < 0);
}

static void test_strlcat(void)
{
    char buf[20];
    memset(buf, 0x77, sizeof(buf)); // init contents with known value

    // dst not NULL terminated tests
    assert(strlcat(buf, "TEST!", sizeof(buf)) == 25);
    assert(strlcat(buf, "123", sizeof(buf)) == 23);
    assert(strlcat(buf, "\0", sizeof(buf)) == 20);

    // start with empty string
    buf[0] = '\0';
    assert(strlen(buf) == 0);

    // add 2 letters
    strlcat(buf, "CS", sizeof(buf));
    assert(strlen(buf) == 2);
    assert(strcmp(buf, "CS") == 0); // check it added the appropriate letters

    // add 4 more characters
    strlcat(buf, "107e", sizeof(buf)); //  result should be "CS107e"
    assert(strlen(buf) == 6);
    assert(strcmp(buf, "CS107e") == 0); // check str contents

    // fill the array
    strlcat(buf, "CS107eCS107e!", sizeof(buf)); // 13 characters, should completely fill space
    assert(strlen(buf) == 19);
    assert(strcmp(buf, "CS107eCS107eCS107e!") == 0);

    // attempt to add more!
    assert(strlcat(buf, "TEST", sizeof(buf)) == 19); // final str length returned
    assert(strcmp(buf, "CS107eCS107eCS107e!") == 0); // str remains the same!
    assert(strlen(buf) == 19);                       // check to make sure no issues with NULL
}

static void test_strtonum(void)
{
    int val = strtonum("013", NULL);
    assert(val == 13);
    const char *input = "107rocks", *rest = NULL;
    val = strtonum(input, &rest);
    assert(val == 107);
    assert(rest == &input[3]);

    // Basic conversion tests
    int val2 = strtonum("0x1", NULL);
    assert(val2 == 1);
    int val3 = strtonum("1234567890", NULL);
    assert(val3 == 1234567890);
    int val4 = strtonum("0x1abcdef7", NULL);
    assert(val4 == 0x1abcdef7);
    int val5 = strtonum("0xabcdef", NULL);
    assert(val5 == 0xabcdef);

    // With invalid chars 
    val = strtonum("12345W", NULL); // decimal w/ invalid
    assert(val == 12345);
    val2 = strtonum("0x12345W", NULL); // hex w/ invalid
    assert(val2 == 0x12345);
    val3 = strtonum("1234abc", NULL); // hex w/o "0x"
    assert(val3 == 1234);
    val4 = strtonum("a1234abc", NULL); // intial invalid
    assert(val4 == 0);
    val5 = strtonum("0X1abc", NULL); // improper hex
    assert(val5 == 0);
    val5 = strtonum("0x01abcdef", NULL); // 0 beginning of hex
    assert(val5 == 0x1abcdef);
    val5 = strtonum("0x01234", NULL); // 0 beginning w/o x for "Ox"
    assert(val5 == 0x1234);

    // endptr tests
    const char *input2 = "0x123abcD", *rest2 = NULL; // in hex str
    strtonum(input2, &rest2);
    assert(rest2 == &input2[8]);
    const char *input3 = "12345", *rest3 = NULL; // normal int w/ '\0'
    strtonum(input3, &rest3);
    assert(rest3 == &input3[5]);
    const char *input4 = "", *rest4 = NULL; // emptry str '\0'
    strtonum(input4, &rest4);
    assert(rest4 == &input4[0]);
    const char *input5 = "0X", *rest5 = NULL; // 0X
    strtonum(input5, &rest5);
    assert(rest5 == &input5[1]);
}

// static void test_to_base(void)
// {
//     char buf[5];
//     size_t bufsize = sizeof(buf);

//     memset(buf, 0x77, bufsize); // init contents with known value

//     int n = signed_to_base(buf, bufsize, -9999, 10, 6);
//     assert(strcmp(buf, "-099") == 0)
//     assert(n == 6);
// }

// static void test_snprintf(void)
// {
//     char buf[100];
//     size_t bufsize = sizeof(buf);

//     memset(buf, 0x77, sizeof(buf)); // init contents with known value

//     // Start off simple...
//     snprintf(buf, bufsize, "Hello, world!");
//     assert(strcmp(buf, "Hello, world!") == 0);

//     // Decimal
//     snprintf(buf, bufsize, "%d", 45);
//     assert(strcmp(buf, "45") == 0);

//     // Hexadecimal
//     snprintf(buf, bufsize, "%04x", 0xef);
//     assert(strcmp(buf, "00ef") == 0);

//     // Pointer
//     snprintf(buf, bufsize, "%p", (void *) 0x20200004);
//     assert(strcmp(buf, "0x20200004") == 0);

//     // Character
//     snprintf(buf, bufsize, "%c", 'A');
//     assert(strcmp(buf, "A") == 0);

//     // String
//     snprintf(buf, bufsize, "%s", "binky");
//     assert(strcmp(buf, "binky") == 0);

//     // Format string with intermixed codes
//     snprintf(buf, bufsize, "CS%d%c!", 107, 'e');
//     assert(strcmp(buf, "CS107e!") == 0);

//     // Test return value
//     assert(snprintf(buf, bufsize, "Hello") == 5);
//     assert(snprintf(buf, 2, "Hello") == 5);
// }

void main(void)
{
    // TODO: Add more and better tests!

    uart_init();
    uart_putstring("Start execute main() in tests/test_strings_printf.c\n");

    test_memset();
    test_memcpy();
    test_strlen();
    test_strcmp();
    test_strlcat();
    test_strtonum();
    // test_to_base();
    // test_snprintf();

    uart_putstring("Successfully finished executing main() in tests/test_strings_printf.c\n");
    uart_putchar(EOT);
}
