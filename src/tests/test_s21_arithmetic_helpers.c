#include <check.h>
#include <stdlib.h>

#include "../s21_decimal.h"

/* ============================================================================
 */
/* ============================ ТЕСТЫ ФУНКЦИЙ ================================
 */
/* ============================================================================
 */

/* -------------------- Decimal -> BigDecimal -------------------- */
START_TEST(test_decimal_to_big_decimal_basic_edge) {
  s21_decimal d = {{0x12345678, 0x9ABCDEF0, 0x0FEDCBA9, 0x80050000}};
  s21_big_decimal b = {{0}};
  decimal_to_big_decimal(&d, &b);
  ck_assert_uint_eq(b.bits[0], d.bits[0]);
  ck_assert_uint_eq(b.bits[1], d.bits[1]);
  ck_assert_uint_eq(b.bits[2], d.bits[2]);
  for (int i = 3; i < 7; i++) ck_assert_uint_eq(b.bits[i], 0);
  ck_assert_int_eq(get_scale(b.bits, BIG_DEC_BITS), 5);
  ck_assert_int_eq(get_sign(b.bits, BIG_DEC_BITS), 1);
}
END_TEST

/* -------------------- Normalize -------------------- */
START_TEST(test_normalize_big_decimals_scales_equalized_edge) {
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

START_TEST(test_normalize_big_decimals_scale_b_smaller) {
  s21_big_decimal a = {{1, 0, 0, 0, 0, 0, 0, 0}};
  s21_big_decimal b = {{1, 0, 0, 0, 0, 0, 0, 0}};
  set_scale(a.bits, BIG_DEC_BITS, 3);
  set_scale(b.bits, BIG_DEC_BITS, 1);

  normalize_big_decimals(&a, &b);

  ck_assert_int_eq(get_scale(a.bits, BIG_DEC_BITS), 3);
  ck_assert_int_eq(get_scale(b.bits, BIG_DEC_BITS), 3);
  ck_assert_uint_eq(b.bits[0], 100);  // b = 1 * 10^2
}
END_TEST

Suite* arithmetic_helpers_suite(void) {
  Suite* s = suite_create("Decimal <-> BigDecimal");
  TCase* tc = tcase_create("Core");

  /* Decimal -> BigDecimal */
  tcase_add_test(tc, test_decimal_to_big_decimal_basic_edge);

  /* Normalize */
  tcase_add_test(tc, test_normalize_big_decimals_scales_equalized_edge);
  tcase_add_test(tc, test_normalize_big_decimals_scale_b_smaller);

  suite_add_tcase(s, tc);
  return s;
}
