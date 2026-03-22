#include <check.h>
#include "../s21_decimal.h"

/* ============================================================================
 * get_bit / set_bit / sign / scale / clear_decimal_bits
 * ==========================================================================*/

/* ---------------- get_bit ---------------- */
START_TEST(test_get_bit_decimal_all_zero) {
    s21_decimal d = {{0}};
    for (int i = 0; i < 96; i++)
        ck_assert_int_eq(get_bit(d.bits, DEC_BITS, i), 0);
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
    for (int i = 0; i < 224; i++)
        ck_assert_int_eq(get_bit(d.bits, BIG_DEC_BITS, i), 0);
}
END_TEST

START_TEST(test_get_bit_big_decimal_set_bits) {
    s21_big_decimal d = {{0}};
    d.bits[0] = 0b101;
    d.bits[3] = 0x80000000;
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

/* ---------------- set_bit ---------------- */
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

/* ---------------- sign ---------------- */
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

/* ---------------- scale ---------------- */
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

/* -------------------- Decimal clear tests -------------------- */
START_TEST(test_decimal_clear_keep_metadata) {
    s21_decimal d = {{0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0x80050000}};

    // Очищаем только мантиссу, оставляем metadata
    clear_decimal_bits(d.bits, DEC_BITS, 0);

    // Мантисса обнулена
    for (int i = 0; i < 3; i++)
        ck_assert_int_eq(d.bits[i], 0);

    // Metadata остались
    ck_assert_int_eq(get_sign(d.bits, DEC_BITS), 1);
    ck_assert_int_eq(get_scale(d.bits, DEC_BITS), 5);
}
END_TEST

START_TEST(test_decimal_clear_full) {
    s21_decimal d = {{0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0x80050000}};

    // Полное обнуление, включая metadata
    clear_decimal_bits(d.bits, DEC_BITS, 1);

    for (int i = 0; i < 4; i++)
        ck_assert_int_eq(d.bits[i], 0);
}
END_TEST

START_TEST(test_big_decimal_clear_keep_metadata) {
    s21_big_decimal d = {
        {0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
         0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0x80050000}
    };

    // Очищаем мантиссу (первые 7 слов), оставляем metadata
    clear_decimal_bits(d.bits, BIG_DEC_BITS, 0);

    for (int i = 0; i < 7; i++)
        ck_assert_int_eq(d.bits[i], 0);

    // Metadata остались
    ck_assert_int_eq(get_sign(d.bits, BIG_DEC_BITS), 1);
    ck_assert_int_eq(get_scale(d.bits, BIG_DEC_BITS), 5);
}
END_TEST

START_TEST(test_big_decimal_clear_full) {
    s21_big_decimal d = {
        {0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
         0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0x80050000}
    };

    // Полное обнуление
    clear_decimal_bits(d.bits, BIG_DEC_BITS, 1);

    for (int i = 0; i < 8; i++)
        ck_assert_int_eq(d.bits[i], 0);
}
END_TEST

/* -------------------- Новые тесты -------------------- */
START_TEST(test_decimal_clear_zero) {
    s21_decimal d = {{0,0,0,0}};
    clear_decimal_bits(d.bits, DEC_BITS, 0);

    for (int i = 0; i < 3; i++)
        ck_assert_int_eq(d.bits[i], 0);

    ck_assert_int_eq(get_sign(d.bits, DEC_BITS), 0);
    ck_assert_int_eq(get_scale(d.bits, DEC_BITS), 0);
}
END_TEST

START_TEST(test_big_decimal_clear_zero) {
    s21_big_decimal d = {{0,0,0,0,0,0,0,0}};
    clear_decimal_bits(d.bits, BIG_DEC_BITS, 0);

    for (int i = 0; i < 7; i++)
        ck_assert_int_eq(d.bits[i], 0);

    ck_assert_int_eq(get_sign(d.bits, BIG_DEC_BITS), 0);
    ck_assert_int_eq(get_scale(d.bits, BIG_DEC_BITS), 0);
}
END_TEST

/* ============================================================================
 * Suite
 * ==========================================================================*/

Suite *decimal_helpers_suite(void) {
    Suite *s = suite_create("Decimal Helpers");
    TCase *tc_core = tcase_create("Core");

    /* get_bit */
    tcase_add_test(tc_core, test_get_bit_decimal_all_zero);
    tcase_add_test(tc_core, test_get_bit_decimal_set_bits);
    tcase_add_test(tc_core, test_get_bit_big_decimal_all_zero);
    tcase_add_test(tc_core, test_get_bit_big_decimal_set_bits);
    tcase_add_test(tc_core, test_get_bit_out_of_range);

    /* set_bit */
    tcase_add_test(tc_core, test_set_bit_decimal);
    tcase_add_test(tc_core, test_set_bit_big_decimal);

    /* sign */
    tcase_add_test(tc_core, test_decimal_sign);
    tcase_add_test(tc_core, test_big_decimal_sign);

    /* scale */
    tcase_add_test(tc_core, test_decimal_scale);
    tcase_add_test(tc_core, test_big_decimal_scale);

    /* clear_decimal_bits */
    tcase_add_test(tc_core, test_decimal_clear_keep_metadata);
    tcase_add_test(tc_core, test_decimal_clear_full);
    tcase_add_test(tc_core, test_big_decimal_clear_keep_metadata);
    tcase_add_test(tc_core, test_big_decimal_clear_full);
    tcase_add_test(tc_core, test_decimal_clear_zero);
    tcase_add_test(tc_core, test_big_decimal_clear_zero);

    suite_add_tcase(s, tc_core);
    return s;
}