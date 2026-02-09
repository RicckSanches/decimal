#include <check.h>
#include "../s21_decimal.h"

START_TEST(test_get_bit_basic) {
    s21_decimal d = {0};
    d.bits[0] = 5; // 0101

    ck_assert_int_eq(get_bit(d, 0), 1);
    ck_assert_int_eq(get_bit(d, 1), 0);
    ck_assert_int_eq(get_bit(d, 2), 1);
    ck_assert_int_eq(get_bit(d, 3), 0);
}
END_TEST

START_TEST(test_get_bit_middle_word) {
    s21_decimal d = {0};
    d.bits[1] = 1u << 7; // бит 39

    ck_assert_int_eq(get_bit(d, 39), 1);
    ck_assert_int_eq(get_bit(d, 38), 0);
}
END_TEST

START_TEST(test_get_bit_highest) {
    s21_decimal d = {0};
    d.bits[2] = 1u << 31; // бит 95

    ck_assert_int_eq(get_bit(d, 95), 1);
    ck_assert_int_eq(get_bit(d, 94), 0);
}
END_TEST

Suite *get_bit_suite(void) {
    Suite *s = suite_create("get_bit");
    TCase *tc = tcase_create("core");

    tcase_add_test(tc, test_get_bit_basic);
    tcase_add_test(tc, test_get_bit_middle_word);
    tcase_add_test(tc, test_get_bit_highest);

    suite_add_tcase(s, tc);
    return s;
}