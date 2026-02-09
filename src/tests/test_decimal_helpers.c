#include <check.h>
#include "../s21_decimal.h"


// ----------------------
// Тесты для get_bit
// ----------------------
START_TEST(test_get_bit_decimal_all_zero)
{
    s21_decimal d = {{0,0,0,0}};
    for (int i = 0; i < 96; i++) {
        ck_assert_int_eq(get_bit(d.bits, 96, i), 0);
    }
}
END_TEST

START_TEST(test_get_bit_decimal_set_bits)
{
    s21_decimal d = {{0,0,0,0}};
    d.bits[0] = 0b1010;      
    d.bits[1] = 0x80000000;  
    d.bits[2] = 0x1;         

    ck_assert_int_eq(get_bit(d.bits, 96, 1), 1);
    ck_assert_int_eq(get_bit(d.bits, 96, 3), 1);
    ck_assert_int_eq(get_bit(d.bits, 96, 63), 1);
    ck_assert_int_eq(get_bit(d.bits, 96, 64), 1);
}
END_TEST

START_TEST(test_get_bit_big_decimal_all_zero)
{
    s21_big_decimal d = {{0,0,0,0,0,0,0,0}};
    for (int i = 0; i < 256; i++) {
        ck_assert_int_eq(get_bit(d.bits, 256, i), 0);
    }
}
END_TEST

START_TEST(test_get_bit_big_decimal_set_bits)
{
    s21_big_decimal d = {{0,0,0,0,0,0,0,0}};
    d.bits[0] = 0b101;           
    d.bits[3] = 0x80000000;      
    d.bits[7] = 0x1;             

    ck_assert_int_eq(get_bit(d.bits, 256, 0), 1);
    ck_assert_int_eq(get_bit(d.bits, 256, 2), 1);
    ck_assert_int_eq(get_bit(d.bits, 256, 127), 1);
    ck_assert_int_eq(get_bit(d.bits, 256, 224), 1);
}
END_TEST

START_TEST(test_get_bit_out_of_range)
{
    s21_decimal d = {{0,0,0,0}};
    ck_assert_int_eq(get_bit(d.bits, 96, -1), 0);
    ck_assert_int_eq(get_bit(d.bits, 96, 96), 0);
    ck_assert_int_eq(get_bit(d.bits, 96, 150), 0);

    s21_big_decimal bd = {{0,0,0,0,0,0,0,0}};
    ck_assert_int_eq(get_bit(bd.bits, 256, -5), 0);
    ck_assert_int_eq(get_bit(bd.bits, 256, 300), 0);
}
END_TEST

// ----------------------
// Тесты для set_bit
// ----------------------
START_TEST(test_set_bit_decimal_set_and_clear)
{
    s21_decimal d = {{0,0,0,0}};

    set_bit(d.bits, 96, 0, 1);
    set_bit(d.bits, 96, 31, 1);
    set_bit(d.bits, 96, 64, 1);

    ck_assert_int_eq(get_bit(d.bits, 96, 0), 1);
    ck_assert_int_eq(get_bit(d.bits, 96, 31), 1);
    ck_assert_int_eq(get_bit(d.bits, 96, 64), 1);

    set_bit(d.bits, 96, 31, 0);
    ck_assert_int_eq(get_bit(d.bits, 96, 31), 0);
}
END_TEST

START_TEST(test_set_bit_decimal_out_of_range)
{
    s21_decimal d = {{0,0,0,0}};
    set_bit(d.bits, 96, -1, 1);
    set_bit(d.bits, 96, 96, 1);
    set_bit(d.bits, 96, 150, 1);

    for (int i = 0; i < 96; i++)
        ck_assert_int_eq(get_bit(d.bits, 96, i), 0);
}
END_TEST

START_TEST(test_set_bit_big_decimal_set_and_clear)
{
    s21_big_decimal d = {{0,0,0,0,0,0,0,0}};

    set_bit(d.bits, 256, 0, 1);
    set_bit(d.bits, 256, 127, 1);
    set_bit(d.bits, 256, 255, 1);

    ck_assert_int_eq(get_bit(d.bits, 256, 0), 1);
    ck_assert_int_eq(get_bit(d.bits, 256, 127), 1);
    ck_assert_int_eq(get_bit(d.bits, 256, 255), 1);

    set_bit(d.bits, 256, 127, 0);
    ck_assert_int_eq(get_bit(d.bits, 256, 127), 0);
}
END_TEST

START_TEST(test_set_bit_big_decimal_out_of_range)
{
    s21_big_decimal d = {{0,0,0,0,0,0,0,0}};

    set_bit(d.bits, 256, -5, 1);
    set_bit(d.bits, 256, 300, 1);

    for (int i = 0; i < 256; i++)
        ck_assert_int_eq(get_bit(d.bits, 256, i), 0);
}
END_TEST

// ----------------------
// Тесты для get_sign / set_sign
// ----------------------
START_TEST(test_s21_decimal_sign) {
    s21_decimal d = {{0}};
    set_sign(d.bits, 1);
    ck_assert_int_eq(get_sign(d.bits), 1);
    set_sign(d.bits, 0);
    ck_assert_int_eq(get_sign(d.bits), 0);
}
END_TEST

START_TEST(test_s21_big_decimal_sign) {
    s21_big_decimal bd = {{0}};
    set_sign(bd.bits, 1);
    ck_assert_int_eq(get_sign(bd.bits), 1);
    set_sign(bd.bits, 0);
    ck_assert_int_eq(get_sign(bd.bits), 0);
}
END_TEST

// ----------------------
// Тесты для get_scale / set_scale
// ----------------------
START_TEST(test_s21_decimal_scale) {
    s21_decimal d = {{0}};
    set_scale(d.bits, 5);
    ck_assert_int_eq(get_scale(d.bits), 5);
    set_scale(d.bits, 28);
    ck_assert_int_eq(get_scale(d.bits), 28);
}
END_TEST

START_TEST(test_s21_big_decimal_scale) {
    s21_big_decimal bd = {{0}};
    set_scale(bd.bits, 5);
    ck_assert_int_eq(get_scale(bd.bits), 5);
    set_scale(bd.bits, 28);
    ck_assert_int_eq(get_scale(bd.bits), 28);
}
END_TEST

// ----------------------
// Тесты для clear_decimal_bits
// ----------------------
START_TEST(test_s21_decimal_clear) {
    s21_decimal d = {{0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0x80050000}};
    clear_decimal_bits(d.bits, 96);

    ck_assert_int_eq(d.bits[0], 0);
    ck_assert_int_eq(d.bits[1], 0);
    ck_assert_int_eq(d.bits[2], 0);
    ck_assert_int_eq(get_sign(d.bits), 1);
    ck_assert_int_eq(get_scale(d.bits), 5);
}
END_TEST

START_TEST(test_s21_big_decimal_clear) {
    s21_big_decimal bd = {{0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0x80050000,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF,0xFFFFFFFF}};
    clear_decimal_bits(bd.bits, 256);

    for (int i = 0; i < 8; i++) {
        if (i == 3) continue; // сохраняем bits[3]
        ck_assert_int_eq(bd.bits[i], 0);
    }

    ck_assert_int_eq(get_sign(bd.bits), 1);
    ck_assert_int_eq(get_scale(bd.bits), 5);
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

    tcase_add_test(tc, test_set_bit_decimal_set_and_clear);
    tcase_add_test(tc, test_set_bit_decimal_out_of_range);
    tcase_add_test(tc, test_set_bit_big_decimal_set_and_clear);
    tcase_add_test(tc, test_set_bit_big_decimal_out_of_range);

    suite_add_tcase(s, tc);
    return s;
}

Suite *sign_suite(void) {
    Suite *s = suite_create("Sign");
    TCase *tc = tcase_create("Core");

    tcase_add_test(tc, test_s21_decimal_sign);
    tcase_add_test(tc, test_s21_big_decimal_sign);

    suite_add_tcase(s, tc);
    return s;
}

Suite *scale_suite(void) {
    Suite *s = suite_create("Scale");
    TCase *tc = tcase_create("Core");

    tcase_add_test(tc, test_s21_decimal_scale);
    tcase_add_test(tc, test_s21_big_decimal_scale);

    suite_add_tcase(s, tc);
    return s;
}

Suite *clear_bits_suite(void) {
    Suite *s = suite_create("ClearBits");
    TCase *tc = tcase_create("Core");

    tcase_add_test(tc, test_s21_decimal_clear);
    tcase_add_test(tc, test_s21_big_decimal_clear);

    suite_add_tcase(s, tc);
    return s;
}