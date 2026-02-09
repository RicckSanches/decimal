#include <check.h>
#include "../s21_decimal.h"

/* ============================================================================
 * mantissa_fits_96
 * ==========================================================================*/

START_TEST(test_mantissa_fits_96_true) {
  s21_big_decimal b = {{1, 2, 3, 0, 0, 0, 0, 0}};
  ck_assert_int_eq(mantissa_fits_96(&b), 1);
}
END_TEST

START_TEST(test_mantissa_fits_96_false) {
  s21_big_decimal b = {{1, 2, 3, 1, 0, 0, 0, 0}};
  ck_assert_int_eq(mantissa_fits_96(&b), 0);
}
END_TEST

/* ============================================================================
 * big_decimal_multiply_by_10
 * ==========================================================================*/

START_TEST(test_big_decimal_multiply_by_10_simple) {
  s21_big_decimal b = {{5, 0, 0, 0, 0, 0, 0, 0}};
  big_decimal_multiply_by_10(&b);

  ck_assert_uint_eq(b.bits[0], 50);
}
END_TEST

START_TEST(test_big_decimal_multiply_by_10_with_carry) {
  s21_big_decimal b = {{0xFFFFFFFF, 0, 0, 0, 0, 0, 0, 0}};
  big_decimal_multiply_by_10(&b);

  ck_assert_uint_eq(b.bits[0], 0xFFFFFFF6);
  ck_assert_uint_eq(b.bits[1], 9);
}
END_TEST

/* ============================================================================
 * big_decimal_divide_by_10
 * ==========================================================================*/

START_TEST(test_big_decimal_divide_by_10_simple) {
  s21_big_decimal b = {{100, 0, 0, 0, 0, 0, 0, 0}};
  int remainder = big_decimal_divide_by_10(&b);

  ck_assert_uint_eq(b.bits[0], 10);
  ck_assert_int_eq(remainder, 0);
}
END_TEST

START_TEST(test_big_decimal_divide_by_10_with_remainder) {
  s21_big_decimal b = {{101, 0, 0, 0, 0, 0, 0, 0}};
  int remainder = big_decimal_divide_by_10(&b);

  ck_assert_uint_eq(b.bits[0], 10);
  ck_assert_int_eq(remainder, 1);
}
END_TEST

/* ============================================================================
 * bank_round
 * ==========================================================================*/

START_TEST(test_bank_round_even_down) {
  s21_big_decimal b = {{10, 0, 0, 1, 0, 0, 0, 0}};
  bank_round(&b, 5);  // 10.5 → 10 (even)

  ck_assert_uint_eq(b.bits[0], 10);
}
END_TEST

START_TEST(test_bank_round_odd_up) {
  s21_big_decimal b = {{11, 0, 0, 1, 0, 0, 0, 0}};
  bank_round(&b, 5);  // 11.5 → 12

  ck_assert_uint_eq(b.bits[0], 12);
}
END_TEST

START_TEST(test_bank_round_large_remainder) {
  s21_big_decimal b = {{10, 0, 0, 1, 0, 0, 0, 0}};
  bank_round(&b, 7);

  ck_assert_uint_eq(b.bits[0], 11);
}
END_TEST

/* ============================================================================
 * decimal_to_big_decimal
 * ==========================================================================*/

START_TEST(test_decimal_to_big_decimal_basic) {
  s21_decimal d = {
      {0x12345678, 0x9ABCDEF0, 0x0FEDCBA9, 0x80050000}};
  s21_big_decimal b = {{0}};

  decimal_to_big_decimal(&d, &b);

  ck_assert_uint_eq(b.bits[0], d.bits[0]);
  ck_assert_uint_eq(b.bits[1], d.bits[1]);
  ck_assert_uint_eq(b.bits[2], d.bits[2]);

  for (int i = 3; i < 7; i++)
    ck_assert_uint_eq(b.bits[i], 0);

  ck_assert_int_eq(get_scale(b.bits, BIG_DEC_BITS), 5);
  ck_assert_int_eq(get_sign(b.bits, BIG_DEC_BITS), 1);
}
END_TEST

START_TEST(test_decimal_to_big_preserves_value) {
  s21_decimal d = {{123, 0, 0, 0}};
  set_scale(d.bits, DEC_BITS, 2);
  set_sign(d.bits, DEC_BITS, 1);

  s21_big_decimal b = {{0}};
  decimal_to_big_decimal(&d, &b);

  ck_assert_uint_eq(b.bits[0], 123);
  ck_assert_int_eq(get_scale(b.bits, BIG_DEC_BITS), 2);
  ck_assert_int_eq(get_sign(b.bits, BIG_DEC_BITS), 1);
}
END_TEST

/* ============================================================================
 * normalize_big_decimals
 * ==========================================================================*/

START_TEST(test_normalize_big_decimals_scales_equalized) {
  s21_big_decimal a = {{1, 0, 0, 0, 0, 0, 0, 0}};
  s21_big_decimal b = {{1, 0, 0, 0, 0, 0, 0, 0}};

  set_scale(a.bits, BIG_DEC_BITS, 1);
  set_scale(b.bits, BIG_DEC_BITS, 3);

  normalize_big_decimals(&a, &b);

  ck_assert_int_eq(get_scale(a.bits, BIG_DEC_BITS), 3);
  ck_assert_int_eq(get_scale(b.bits, BIG_DEC_BITS), 3);
  ck_assert_uint_eq(a.bits[0], 100);
}
END_TEST

START_TEST(test_normalize_big_decimals_b_smaller) {
    s21_big_decimal a = {{10, 0, 0, 0, 0, 0, 0, 0}};
    s21_big_decimal b = {{2, 0, 0, 0, 0, 0, 0, 0}};

    // a scale = 5, b scale = 2
    set_scale(a.bits, BIG_DEC_BITS, 5);
    set_scale(b.bits, BIG_DEC_BITS, 2);

    normalize_big_decimals(&a, &b);

    int scale_a = get_scale(a.bits, BIG_DEC_BITS);
    int scale_b = get_scale(b.bits, BIG_DEC_BITS);
    ck_assert_int_eq(scale_a, scale_b);
    
    // b должен увеличиться на 10^(5-2) = 1000
    ck_assert_uint_eq(b.bits[0], 2 * 1000);
}
END_TEST

/* ============================================================================
 * big_decimal_to_decimal
 * ==========================================================================*/

START_TEST(test_big_decimal_to_decimal_no_round) {
  s21_big_decimal b = {{0x12345678, 0x9ABCDEF0, 0x0FEDCBA9, 0, 0, 0, 0, 0}};
  set_scale(b.bits, BIG_DEC_BITS, 3);

  s21_decimal d = {{0}};
  big_decimal_to_decimal(&b, &d);

  ck_assert_uint_eq(d.bits[0], b.bits[0]);
  ck_assert_uint_eq(d.bits[1], b.bits[1]);
  ck_assert_uint_eq(d.bits[2], b.bits[2]);
  ck_assert_int_eq(get_scale(d.bits, DEC_BITS), 3);
}
END_TEST

START_TEST(test_big_decimal_to_decimal_with_rounding) {
  s21_big_decimal b = {{0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 1, 0, 0, 0, 0}};
  set_scale(b.bits, BIG_DEC_BITS, 0);

  s21_decimal d = {{0}};
  big_decimal_to_decimal(&b, &d);

  ck_assert_uint_gt(d.bits[0], 0);
}
END_TEST

/* ============================================================================
 * Suite
 * ==========================================================================*/

Suite* conversion_suite(void) {
  Suite* s = suite_create("Decimal <-> BigDecimal");
  TCase* tc = tcase_create("Core");

  tcase_add_test(tc, test_mantissa_fits_96_true);
  tcase_add_test(tc, test_mantissa_fits_96_false);

  tcase_add_test(tc, test_big_decimal_multiply_by_10_simple);
  tcase_add_test(tc, test_big_decimal_multiply_by_10_with_carry);

  tcase_add_test(tc, test_big_decimal_divide_by_10_simple);
  tcase_add_test(tc, test_big_decimal_divide_by_10_with_remainder);

  tcase_add_test(tc, test_bank_round_even_down);
  tcase_add_test(tc, test_bank_round_odd_up);
  tcase_add_test(tc, test_bank_round_large_remainder);

  tcase_add_test(tc, test_decimal_to_big_decimal_basic);
  tcase_add_test(tc, test_decimal_to_big_preserves_value);

  tcase_add_test(tc, test_normalize_big_decimals_scales_equalized);
  tcase_add_test(tc, test_normalize_big_decimals_b_smaller);

  tcase_add_test(tc, test_big_decimal_to_decimal_no_round);
  tcase_add_test(tc, test_big_decimal_to_decimal_with_rounding);

  suite_add_tcase(s, tc);
  return s;
}