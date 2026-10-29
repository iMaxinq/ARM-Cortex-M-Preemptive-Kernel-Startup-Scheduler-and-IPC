#include "unity.h"
#include "kprintf.h"
#include <limits.h>
#include <stddef.h>

static char s_out_buf[256];
static size_t s_out_idx;

static void mock_putc(char c)
{
    if (s_out_idx < sizeof(s_out_buf) - 1U) {
        s_out_buf[s_out_idx++] = c;
        s_out_buf[s_out_idx] = '\0';
    }
}

void setUp(void)
{
    s_out_idx = 0;
    s_out_buf[0] = '\0';
    kprintf_init(mock_putc);
}

void tearDown(void)
{
}

void test_kprintf_plain_and_percent(void)
{
    kprintf("Hello 100%%!");
    TEST_ASSERT_EQUAL_STRING("Hello 100%!", s_out_buf);
}

void test_kprintf_char_and_strings(void)
{
    kprintf("%c: %s | %s", 'X', "test", (const char *)NULL);
    TEST_ASSERT_EQUAL_STRING("X: test | (null)", s_out_buf);
}

void test_kprintf_signed_integers(void)
{
    kprintf("%d %d %d %d", 0, 42, -42, INT_MIN);
    TEST_ASSERT_EQUAL_STRING("0 42 -42 -2147483648", s_out_buf);
}

void test_kprintf_unsigned_and_hex(void)
{
    kprintf("%u 0x%x 0x%x", 4294967295U, 0U, 0xDEADBEEFU);
    TEST_ASSERT_EQUAL_STRING("4294967295 0x0 0xdeadbeef", s_out_buf);
}

int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_kprintf_plain_and_percent);
    RUN_TEST(test_kprintf_char_and_strings);
    RUN_TEST(test_kprintf_signed_integers);
    RUN_TEST(test_kprintf_unsigned_and_hex);
    return UNITY_END();
}