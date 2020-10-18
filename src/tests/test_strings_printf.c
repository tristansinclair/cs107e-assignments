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

    char test2[9] = "";                      // empty array
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
    assert(strlcat(buf, "TEST", sizeof(buf)) == 23); // final str length returned
    assert(strcmp(buf, "CS107eCS107eCS107e!") == 0); // str remains the same!
    assert(strlen(buf) == 19);                       // check to make sure no issues with NULL

    // !!!!!!!!!!!!!!!!!!!!! issue
    char buf2[5];
    buf2[0] = '-';
    buf2[1] = '0';
    buf2[2] = '\0';

    //char *ptr = &buf2[1];
    strlcat(buf2 + 1, "9999", 4);
    assert(strcmp(buf2, "-099") == 0);
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

static void test_to_base(void)
{
    /* ----------- Decimal tests ----------- */
    char buf[8];
    memset(buf, 0x77, sizeof(buf));                 // init contents with known value
    int num = unsigned_to_base(buf, 8, 123, 10, 0); // ("123")
    assert(num == 3);
    assert(strlen(buf) == 3);
    assert(strcmp(buf, "123") == 0);
    memset(buf, 0x77, sizeof(buf));             // init contents with known value
    num = unsigned_to_base(buf, 8, 123, 10, 4); // ("0123")
    assert(num == 4);
    assert(strcmp(buf, "0123") == 0);
    // min value
    memset(buf, 0x77, sizeof(buf));             // init contents with known value
    num = unsigned_to_base(buf, 8, 123, 10, 9); // ("0000001")
    assert(num == 9);
    assert(strcmp(buf, "0000001") == 0);
    // bufsize = 0
    memset(buf, 0x77, sizeof(buf));             // init contents with known value
    num = unsigned_to_base(buf, 0, 123, 10, 9); // ("") b/c nothing is written here
    assert(num == 0);
    // all zeros
    memset(buf, 0x77, sizeof(buf));              // init contents with known value
    num = unsigned_to_base(buf, 8, 123, 10, 11); // ("0000000")
    assert(num == 11);
    assert(strcmp(buf, "0000000") == 0);
    // num bigger than buf size
    memset(buf, 0x77, sizeof(buf));                  // init contents with known value
    num = unsigned_to_base(buf, 8, 12345678, 10, 0); // ("1234567")
    assert(num == 8);
    assert(strcmp(buf, "1234567") == 0);

    /* ----------- Hex tests ----------- */
    memset(buf, 0x77, sizeof(buf));              // init contents with known value
    num = unsigned_to_base(buf, 8, 0x7e, 16, 0); // ("7e")
    assert(num == 2);
    assert(strcmp(buf, "7e") == 0);

    memset(buf, 0x77, sizeof(buf));                  // init contents with known value
    num = unsigned_to_base(buf, 8, 0x1a2e3b, 16, 0); // ("1a2e3b")
    assert(num == 6);
    assert(strcmp(buf, "1a2e3b") == 0);
    // min value
    memset(buf, 0x77, sizeof(buf));                // init contents with known value
    num = unsigned_to_base(buf, 8, 0x123a, 16, 7); // ("000123a")
    assert(num == 7);
    assert(strcmp(buf, "000123a") == 0);
    // all zeros
    memset(buf, 0x77, sizeof(buf));                 // init contents with known value
    num = unsigned_to_base(buf, 8, 0x123a, 16, 12); // ("0000000")
    assert(num == 12);
    assert(strcmp(buf, "0000000") == 0);
    // hex num bigger than buf size
    memset(buf, 0x77, sizeof(buf));                    // init contents with known value
    num = unsigned_to_base(buf, 8, 0x1234abcd, 16, 0); // ("1234abc")
    assert(num == 8);
    assert(strcmp(buf, "1234abc") == 0);
    // hex num bigger than buf size + min length
    memset(buf, 0x77, sizeof(buf));                     // init contents with known value
    num = unsigned_to_base(buf, 8, 0x1234abcd, 16, 10); // ("1234abc")
    assert(num == 10);
    assert(strcmp(buf, "001234a") == 0);

    /* ----------- Signed to Base Tesing ----------- */
    char buf2[5];
    size_t bufsize = sizeof(buf2);
    assert(bufsize == 5);

    memset(buf2, 0x77, bufsize);                     // init contents with known value
    int n = signed_to_base(buf2, 5, 0x123a, 16, 12); // (+ num)
    assert(n == 12);
    assert(strcmp(buf2, "0000") == 0);

    memset(buf2, 0x77, bufsize);                // init contents with known value
    n = signed_to_base(buf2, 5, 0x123a, 16, 0); // (+ num)
    assert(n == 4);
    assert(strcmp(buf2, "123a") == 0);

    memset(buf2, 0x77, bufsize);             // init contents with known value
    n = signed_to_base(buf2, 5, -12, 10, 4); // (+ num) larger than available space
    assert(n == 4);
    assert(strcmp(buf2, "-012") == 0); // ("-012")

    memset(buf2, 0x77, bufsize);             // init contents with known value
    n = signed_to_base(buf2, 5, -10, 10, 0); // (- num base 10)
    assert(n == 3);
    assert(strcmp(buf2, "-10") == 0);

    memset(buf2, 0x77, bufsize);               // init contents with known value
    n = signed_to_base(buf2, 5, -0x12, 16, 0); // (- num base 10)
    assert(n == 3);
    assert(strcmp(buf2, "-12") == 0);

    memset(buf2, 0x77, bufsize);               // init contents with known value
    n = signed_to_base(buf2, 5, -0x12, 16, 4); // (- num base 10)
    assert(n == 4);
    assert(strcmp(buf2, "-012") == 0);

    memset(buf2, 0x77, bufsize); // init contents with known value
    n = signed_to_base(buf2, 5, -9999, 10, 6);
    assert(n == 6);
    assert(strcmp(buf2, "-099") == 0);

    memset(buf2, 0x77, bufsize); // init contents with known value
    n = signed_to_base(buf2, 5, -9999999, 10, 6);
    assert(n == 8);
    assert(strcmp(buf2, "-999") == 0);

    memset(buf2, 0x77, bufsize); // init contents with known value
    n = signed_to_base(buf2, 5, -0x12abcde, 16, 6);
    assert(n == 8);
    assert(strcmp(buf2, "-12a") == 0);

    memset(buf2, 0x77, bufsize); // init contents with known value
    n = signed_to_base(buf2, 5, -0x1, 16, 0);
    assert(n == 2);
    assert(strcmp(buf2, "-1") == 0);

    memset(buf2, 'T', 1);
    memset(buf2 + 1, 'E', 1);
    memset(buf2 + 2, 'S', 1);
    memset(buf2 + 3, 'T', 1);
    memset(buf2 + 4, 0, 1);
    assert(strcmp(buf2, "TEST") == 0);
    assert(signed_to_base(buf2, 0, -0x123, 16, 10) == 1); // signed base w/ negative return 1 if bufsize = 0
    assert(strcmp(buf2, "TEST") == 0);
}

static void test_snprintf(void)
{
    char buf[100];
    size_t bufsize = sizeof(buf);

    memset(buf, 0x77, sizeof(buf)); // init contents with known value

    // Start off simple...
    assert(snprintf(buf, bufsize, "Hello, world!") == 13);
    assert(strcmp(buf, "Hello, world!") == 0);

    // Decimal
    assert(snprintf(buf, bufsize, "%d", 45) == 2);
    assert(strcmp(buf, "45") == 0);

    assert(snprintf(buf, bufsize, "%03d", 45) == 3);
    assert(strcmp(buf, "045") == 0);

    assert(snprintf(buf, bufsize, "%010d", 123456) == 10);
    assert(strcmp(buf, "0000123456") == 0);

    // Hexadecimal
    assert(snprintf(buf, bufsize, "%04x", 0xef) == 4);
    assert(strcmp(buf, "00ef") == 0);

    // Pointer
    assert(snprintf(buf, bufsize, "%p", (void *)0x20200004) == 10);
    assert(strcmp(buf, "0x20200004") == 0);

    // Character
    assert(snprintf(buf, bufsize, "%c", 'A') == 1);
    assert(strcmp(buf, "A") == 0);

    // String
    assert(snprintf(buf, bufsize, "%s", "binky") == 5);
    assert(strcmp(buf, "binky") == 0);

    // Format string with intermixed codes
    assert(snprintf(buf, bufsize, "CS%d%c!", 107, 'e') == 7);
    assert(strcmp(buf, "CS107e!") == 0);

    // Test return value
    assert(snprintf(buf, bufsize, "Hello") == 5);
    assert(snprintf(buf, 2, "Hello") == 5);

    // big test of all the tags
    char *testptr = (char *)0x12345678;
    assert(snprintf(buf, bufsize, "c: %c, s: %s, d: %d, d(-): %d, d(min): %03d, x: 0x%x, x(min): 0x%02x, p: %p, %%: %%", 'a', "abc", 2, -12, 1, 0xf, 0x3, testptr) == 85);
    assert(strcmp(buf, "c: a, s: abc, d: 2, d(-): -12, d(min): 001, x: 0xf, x(min): 0x03, p: 0x12345678, %: %") == 0);

    // More rigorous edge case testing
    char buf2[10];
    size_t bufsize2 = sizeof(buf2);

    memset(buf2, 0x77, sizeof(buf2)); // init contents with known value

    // buf limit testing
    assert(snprintf(buf2, bufsize2, "Hello, world!") == 13);
    assert(strcmp(buf2, "Hello, wo") == 0);

    assert(snprintf(buf2, bufsize2, "Test: %s", "Hello!") == 12);
    assert(strcmp(buf2, "Test: Hel") == 0);

    assert(snprintf(buf2, bufsize2, "Test: %d", 12345678) == 14);
    assert(strcmp(buf2, "Test: 123") == 0);

    assert(snprintf(buf2, bufsize2, "Test: %017d", 12345678) == 17 + 6);
    assert(strcmp(buf2, "Test: 000") == 0);

    snprintf(buf2, bufsize2, "Test: ab%c%c%c%c", 'c', 'd', 'e', 'f');
    assert(strcmp(buf2, "Test: abc") == 0);

    // testing printf
    printf("\n");
    printf("Tests for our printf function: \n");
    // "Test: abcdef"
    printf("Test: ab%c%c%c%c \n", 'c', 'd', 'e', 'f');
    // "c: a, s: abc, d: 2, d(-): -12, d(min): 001, x: 0xf, x(min): 0x03, p: 0x12345678, %: %"
    printf("Long test w/ all types: ");
    printf("c: %c, s: %s, d: %d, d(-): %d, d(min): %03d, x: 0x%x, x(min): 0x%02x, p: %p, %%: %%\n ", 'a', "abc", 2, -12, 1, 0xf, 0x3, testptr);
    printf("\n\n");
}

void main(void)
{
    uart_init();
    uart_putstring("Start execute main() in tests/test_strings_printf.c\n");

    test_memset();
    test_memcpy();
    test_strlen();
    test_strcmp();
    test_strlcat();
    test_strtonum();
    test_to_base();
    test_snprintf();

    uart_putstring("Successfully finished executing main() in tests/test_strings_printf.c\n");
    uart_putchar(EOT);
}
