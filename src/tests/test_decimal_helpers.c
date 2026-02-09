#include <check.h>
#include "../s21_decimal.h"

#define DEC_BITS 128
#define BIG_DEC_BITS 256

// ----------------------
// get_bit
// ----------------------
START_TEST(test_get_bit_decimal_all_zero) {
    s21_decimal d = {{0}};
    for (int i = 0; i < 96; i++) {
        ck_assert_int_eq(get_bit(d.bits, DEC_BITS, i), 0);
    }
}
END_TEST

START_TEST(test_get_bit_decimal_set_bits) {
    s21_decimal d = {{0}};
    d.bits[0] = 0b1010;
    d.bits[1] = 0x80000000;
    d.bits[2] = 0x1;

    ck_assert_int_eq(get_bit(d.bits, DEC_BITS, 1), 1);
    ck_assert_int_eq(get_bit(d.bits, DEC_BITS, 3), 1);
    ck_assert_int_eq(get_bit(d.bits, DEC_BITS, 63), 1);
    ck_assert_int_eq(get_bit(d.bits, DEC_BITS, 64), 1);
}
END_TEST

START_TEST(test_get_bit_big_decimal_all_zero) {
    s21_big_decimal d = {{0}};
    for (int i = 0; i < 224; i++) {
        ck_assert_int_eq(get_bit(d.bits, BIG_DEC_BITS, i), 0);
    }
}
END_TEST

START_TEST(test_get_bit_big_decimal_set_bits) {
    s21_big_decimal d = {{0}};
    d.bits[0] = 0b101;
    d.bits[3] = 0x80000000; // здесь это часть мантиссы
    d.bits[6] = 1u << 31;

    ck_assert_int_eq(get_bit(d.bits, BIG_DEC_BITS, 0), 1);
    ck_assert_int_eq(get_bit(d.bits, BIG_DEC_BITS, 2), 1);
    ck_assert_int_eq(get_bit(d.bits, BIG_DEC_BITS, 127), 1);
    ck_assert_int_eq(get_bit(d.bits, BIG_DEC_BITS, 223), 1);
}
END_TEST

START_TEST(test_get_bit_out_of_range) {
    s21_decimal d = {{0}};
    ck_assert_int_eq(get_bit(d.bits, DEC_BITS, -1), 0);
    ck_assert_int_eq(get_bit(d.bits, DEC_BITS, 224), 0);

    s21_big_decimal bd = {{0}};
    ck_assert_int_eq(get_bit(bd.bits, BIG_DEC_BITS, -10), 0);
    ck_assert_int_eq(get_bit(bd.bits, BIG_DEC_BITS, 300), 0);
}
END_TEST

// ----------------------
// set_bit
// ----------------------
START_TEST(test_set_bit_decimal) {
    s21_decimal d = {{0}};

    set_bit(d.bits, DEC_BITS, 0, 1);
    set_bit(d.bits, DEC_BITS, 31, 1);
    set_bit(d.bits, DEC_BITS, 64, 1);

    ck_assert_int_eq(get_bit(d.bits, DEC_BITS, 0), 1);
    ck_assert_int_eq(get_bit(d.bits, DEC_BITS, 31), 1);
    ck_assert_int_eq(get_bit(d.bits, DEC_BITS, 64), 1);

    set_bit(d.bits, DEC_BITS, 31, 0);
    ck_assert_int_eq(get_bit(d.bits, DEC_BITS, 31), 0);
}
END_TEST

START_TEST(test_set_bit_big_decimal) {
    s21_big_decimal d = {{0}};

    set_bit(d.bits, BIG_DEC_BITS, 0, 1);
    set_bit(d.bits, BIG_DEC_BITS, 127, 1);
    set_bit(d.bits, BIG_DEC_BITS, 223, 1);

    ck_assert_int_eq(get_bit(d.bits, BIG_DEC_BITS, 0), 1);
    ck_assert_int_eq(get_bit(d.bits, BIG_DEC_BITS, 127), 1);
    ck_assert_int_eq(get_bit(d.bits, BIG_DEC_BITS, 223), 1);
}
END_TEST

// ----------------------
// sign
// ----------------------
START_TEST(test_decimal_sign) {
    s21_decimal d = {{0}};
    set_sign(d.bits, DEC_BITS, 1);
    ck_assert_int_eq(get_sign(d.bits, DEC_BITS), 1);
    set_sign(d.bits, DEC_BITS, 0);
    ck_assert_int_eq(get_sign(d.bits, DEC_BITS), 0);
}
END_TEST

START_TEST(test_big_decimal_sign) {
    s21_big_decimal d = {{0}};
    set_sign(d.bits, BIG_DEC_BITS, 1);
    ck_assert_int_eq(get_sign(d.bits, BIG_DEC_BITS), 1);
    set_sign(d.bits, BIG_DEC_BITS, 0);
    ck_assert_int_eq(get_sign(d.bits, BIG_DEC_BITS), 0);
}
END_TEST

// ----------------------
// scale
// ----------------------
START_TEST(test_decimal_scale) {
    s21_decimal d = {{0}};
    set_scale(d.bits, DEC_BITS, 5);
    ck_assert_int_eq(get_scale(d.bits, DEC_BITS), 5);
}
END_TEST

START_TEST(test_big_decimal_scale) {
    s21_big_decimal d = {{0}};
    set_scale(d.bits, BIG_DEC_BITS, 28);
    ck_assert_int_eq(get_scale(d.bits, BIG_DEC_BITS), 28);
}
END_TEST

// ----------------------
// clear_decimal_bits
// ----------------------
START_TEST(test_decimal_clear) {
    s21_decimal d = {{0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0x80050000}};
    clear_decimal_bits(d.bits, DEC_BITS);

    for (int i = 0; i < 3; i++)
        ck_assert_int_eq(d.bits[i], 0);

    ck_assert_int_eq(get_sign(d.bits, DEC_BITS), 1);
    ck_assert_int_eq(get_scale(d.bits, DEC_BITS), 5);
}
END_TEST

START_TEST(test_big_decimal_clear) {
    s21_big_decimal d = {{0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,
                          0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0x80050000}};
    clear_decimal_bits(d.bits, BIG_DEC_BITS);

    for (int i = 0; i < 7; i++)
        ck_assert_int_eq(d.bits[i], 0);

    ck_assert_int_eq(get_sign(d.bits, BIG_DEC_BITS), 1);
    ck_assert_int_eq(get_scale(d.bits, BIG_DEC_BITS), 5);
}
END_TEST

// ----------------------
// Suite
// ----------------------
Suite *get_bit_suite(void) {
    Suite *s = suite_create("get_bit");
    TCase *tc = tcase_create("Core");

    tcase_add_test(tc, test_get_bit_decimal_all_zero);
    tcase_add_test(tc, test_get_bit_decimal_set_bits);
    tcase_add_test(tc, test_get_bit_big_decimal_all_zero);
    tcase_add_test(tc, test_get_bit_big_decimal_set_bits);
    tcase_add_test(tc, test_get_bit_out_of_range);

    suite_add_tcase(s, tc);
    return s;
}

Suite *set_bit_suite(void) {
    Suite *s = suite_create("set_bit");
    TCase *tc = tcase_create("Core");

    tcase_add_test(tc, test_set_bit_decimal);
    tcase_add_test(tc, test_set_bit_big_decimal);

    suite_add_tcase(s, tc);
    return s;
}


Suite *get_sign_suite(void) {
    Suite *s = suite_create("get_sign");
    TCase *tc = tcase_create("Core");

    tcase_add_test(tc, test_decimal_sign);
    tcase_add_test(tc, test_big_decimal_sign);

    suite_add_tcase(s, tc);
    return s;
}

Suite *set_sign_suite(void) {
    Suite *s = suite_create("set_sign");
    TCase *tc = tcase_create("Core");

    tcase_add_test(tc, test_decimal_sign);
    tcase_add_test(tc, test_big_decimal_sign);

    suite_add_tcase(s, tc);
    return s;
}

Suite *get_scale_suite(void) {
    Suite *s = suite_create("get_scale");
    TCase *tc = tcase_create("Core");

    tcase_add_test(tc, test_decimal_scale);
    tcase_add_test(tc, test_big_decimal_scale);

    suite_add_tcase(s, tc);
    return s;
}

Suite *set_scale_suite(void) {
    Suite *s = suite_create("set_scale");
    TCase *tc = tcase_create("Core");

    tcase_add_test(tc, test_decimal_scale);
    tcase_add_test(tc, test_big_decimal_scale);

    suite_add_tcase(s, tc);
    return s;
}

Suite *clear_decimal_bits_suite(void) {
    Suite *s = suite_create("clear_decimal_bits");
    TCase *tc = tcase_create("Core");

    tcase_add_test(tc, test_decimal_clear);
    tcase_add_test(tc, test_big_decimal_clear);

    suite_add_tcase(s, tc);
    return s;
}