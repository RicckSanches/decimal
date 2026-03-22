#include <check.h>
#include "../s21_decimal.h"

/* -------------------- int -> decimal -------------------- */
START_TEST(test_from_int_to_decimal_basic_positive) {
    s21_decimal d = {{0}};
    int err = s21_from_int_to_decimal(12345, &d);

    ck_assert_int_eq(err, 0);
    ck_assert_uint_eq(d.bits[0], 12345);
    ck_assert_int_eq(get_sign(d.bits, DEC_BITS), 0);
    ck_assert_int_eq(get_scale(d.bits, DEC_BITS), 0);
}
END_TEST

START_TEST(test_from_int_to_decimal_basic_negative) {
    s21_decimal d = {{0}};
    int err = s21_from_int_to_decimal(-9876, &d);

    ck_assert_int_eq(err, 0);
    ck_assert_uint_eq(d.bits[0], 9876);
    ck_assert_int_eq(get_sign(d.bits, DEC_BITS), 1);
    ck_assert_int_eq(get_scale(d.bits, DEC_BITS), 0);
}
END_TEST

START_TEST(test_from_int_to_decimal_zero) {
    s21_decimal d = {{0}};
    int err = s21_from_int_to_decimal(0, &d);

    ck_assert_int_eq(err, 0);
    ck_assert_uint_eq(d.bits[0], 0);
    ck_assert_int_eq(get_sign(d.bits, DEC_BITS), 0);
    ck_assert_int_eq(get_scale(d.bits, DEC_BITS), 0);
}
END_TEST

START_TEST(test_from_int_to_decimal_max_int) {
    s21_decimal d = {{0}};
    int err = s21_from_int_to_decimal(INT_MAX, &d);

    ck_assert_int_eq(err, 0);
    ck_assert_uint_eq(d.bits[0], (unsigned int)INT_MAX);
    ck_assert_int_eq(get_sign(d.bits, DEC_BITS), 0);
    ck_assert_int_eq(get_scale(d.bits, DEC_BITS), 0);
}
END_TEST

START_TEST(test_from_int_to_decimal_min_int) {
    s21_decimal d = {{0}};
    int err = s21_from_int_to_decimal(INT_MIN, &d);

    ck_assert_int_eq(err, 0);
    ck_assert_uint_eq(d.bits[0], (unsigned int)(-(long long)INT_MIN));
    ck_assert_int_eq(get_sign(d.bits, DEC_BITS), 1);
    ck_assert_int_eq(get_scale(d.bits, DEC_BITS), 0);
}
END_TEST

START_TEST(test_from_int_to_decimal_null_pointer) {
    int err = s21_from_int_to_decimal(42, NULL);
    ck_assert_int_eq(err, 1);  // должен вернуть ошибку
}
END_TEST

/* -------------------- Suite -------------------- */
Suite* conversion_suite(void) {
    Suite* s = suite_create("int -> s21_decimal");
    TCase* tc = tcase_create("Core");

    tcase_add_test(tc, test_from_int_to_decimal_basic_positive);
    tcase_add_test(tc, test_from_int_to_decimal_basic_negative);
    tcase_add_test(tc, test_from_int_to_decimal_zero);
    tcase_add_test(tc, test_from_int_to_decimal_max_int);
    tcase_add_test(tc, test_from_int_to_decimal_min_int);
    tcase_add_test(tc, test_from_int_to_decimal_null_pointer);

    suite_add_tcase(s, tc);
    return s;
}