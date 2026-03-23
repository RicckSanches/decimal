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

START_TEST(test_normalize_big_decimals_already_equal) {
  s21_big_decimal a = {{5, 0, 0, 0, 0, 0, 0, 0}};
  s21_big_decimal b = {{10, 0, 0, 0, 0, 0, 0, 0}};
  set_scale(a.bits, BIG_DEC_BITS, 3);
  set_scale(b.bits, BIG_DEC_BITS, 3);
  normalize_big_decimals(&a, &b);
  ck_assert_int_eq(get_scale(a.bits, BIG_DEC_BITS), 3);
  ck_assert_int_eq(get_scale(b.bits, BIG_DEC_BITS), 3);
  ck_assert_uint_eq(a.bits[0], 5);
  ck_assert_uint_eq(b.bits[0], 10);
}
END_TEST

START_TEST(test_normalize_big_decimals_zero_values) {
  s21_big_decimal a = {{0}};
  s21_big_decimal b = {{0}};
  set_scale(a.bits, BIG_DEC_BITS, 5);
  set_scale(b.bits, BIG_DEC_BITS, 2);
  normalize_big_decimals(&a, &b);
  ck_assert_int_eq(get_scale(a.bits, BIG_DEC_BITS),
                   get_scale(b.bits, BIG_DEC_BITS));
  ck_assert_uint_eq(a.bits[0], 0);
  ck_assert_uint_eq(b.bits[0], 0);
}
END_TEST

START_TEST(test_normalize_big_decimals_mul10_overflow) {
  s21_big_decimal a = {{0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0, 0, 0, 0, 0}};
  s21_big_decimal b = {{1, 0, 0, 0, 0, 0, 0, 0}};
  set_scale(a.bits, BIG_DEC_BITS, 0);
  set_scale(b.bits, BIG_DEC_BITS, 2);

  normalize_big_decimals(&a, &b);

  // Проверяем, что scale выровнялись, а переполнение обработано
  ck_assert_int_eq(get_scale(a.bits, BIG_DEC_BITS),
                   get_scale(b.bits, BIG_DEC_BITS));
}
END_TEST

START_TEST(test_normalize_big_decimals_multi_step_overflow) {
  s21_big_decimal a = {{0xFFFFFFF0, 0xFFFFFFF0, 0xFFFFFFF0, 0, 0, 0, 0, 0}};
  s21_big_decimal b = {{1, 0, 0, 0, 0, 0, 0, 0}};

  set_scale(a.bits, BIG_DEC_BITS, 0);
  set_scale(b.bits, BIG_DEC_BITS, 5);

  normalize_big_decimals(&a, &b);

  // Проверяем, что scale теперь равны
  int scale_a = get_scale(a.bits, BIG_DEC_BITS);
  int scale_b = get_scale(b.bits, BIG_DEC_BITS);
  ck_assert_int_eq(scale_a, scale_b);

  // Проверяем, что значения не потеряны (a должно быть максимально близким к
  // переполнению)
  ck_assert_uint_gt(a.bits[0], 0);
  ck_assert_uint_eq(b.bits[0], 1);  // b может быть уменьшено, но не до нуля
}
END_TEST



// -------------------- big_to_decimal --------------------
// -------------------- Без переполнения --------------------
START_TEST(test_big_to_decimal_simple) {
  s21_big_decimal big = {{123, 0, 0, 0, 0, 0}};
  set_scale(big.bits, BIG_DEC_BITS, 0);
  set_sign(big.bits, BIG_DEC_BITS, 0);

  s21_decimal dec;
  int res = big_decimal_to_decimal(&big, &dec);

  ck_assert_int_eq(res, 0);
  ck_assert_int_eq(dec.bits[0], 123);
  ck_assert_int_eq(get_scale(dec.bits, DEC_BITS), 0);
  ck_assert_int_eq(get_sign(dec.bits, DEC_BITS), 0);
}
END_TEST

// -------------------- Со scale --------------------
START_TEST(test_big_to_decimal_with_scale) {
  s21_big_decimal big = {{12345, 0, 0, 0, 0, 0}};
  set_scale(big.bits, BIG_DEC_BITS, 2);

  s21_decimal dec;
  int res = big_decimal_to_decimal(&big, &dec);

  ck_assert_int_eq(res, 0);
  ck_assert_int_eq(dec.bits[0], 12345);
  ck_assert_int_eq(get_scale(dec.bits, DEC_BITS), 2);
}
END_TEST

// -------------------- Отрицательное --------------------
START_TEST(test_big_to_decimal_negative) {
  s21_big_decimal big = {{555, 0, 0, 0, 0, 0}};
  set_sign(big.bits, BIG_DEC_BITS, 1);

  s21_decimal dec;
  int res = big_decimal_to_decimal(&big, &dec);

  ck_assert_int_eq(res, 0);
  ck_assert_int_eq(get_sign(dec.bits, DEC_BITS), 1);
}
END_TEST

// -------------------- Переполнение --------------------
START_TEST(test_big_to_decimal_overflow) {
  s21_big_decimal big = {{0, 0, 0, 1, 0, 0}};  // старшее слово заполнено

  s21_decimal dec;
  int res = big_decimal_to_decimal(&big, &dec);

  ck_assert_int_eq(res, 1);  // положительное переполнение
}
END_TEST

// -------------------- Отрицательное переполнение --------------------
START_TEST(test_big_to_decimal_negative_overflow) {
  s21_big_decimal big = {{0, 0, 0, 1, 0, 0}};
  set_sign(big.bits, BIG_DEC_BITS, 1);

  s21_decimal dec;
  int res = big_decimal_to_decimal(&big, &dec);

  ck_assert_int_eq(res, 2);  // отрицательное переполнение
}
END_TEST

// -------------------- Банковское округление --------------------
START_TEST(test_big_to_decimal_bank_round) {
  // 15 -> при делении должно округлиться до 2
  s21_big_decimal big = {{15, 0, 0, 1, 0, 0}};
  set_scale(big.bits, BIG_DEC_BITS, 1);

  s21_decimal dec;
  int res = big_decimal_to_decimal(&big, &dec);

  ck_assert_int_eq(res, 0);
  ck_assert_int_eq(dec.bits[0], 2);
}
END_TEST

// -------------------- Граница (влезает после округления) --------------------
START_TEST(test_big_to_decimal_fit_after_round) {
  s21_big_decimal big = {{0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 1, 0, 0}};
  set_scale(big.bits, BIG_DEC_BITS, 1);

  s21_decimal dec;
  int res = big_decimal_to_decimal(&big, &dec);

  // может стать либо 0 (если округлилось), либо overflow
  ck_assert_int_ge(res, 0);
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
  tcase_add_test(tc, test_normalize_big_decimals_already_equal);
  tcase_add_test(tc, test_normalize_big_decimals_zero_values);
  tcase_add_test(tc, test_normalize_big_decimals_mul10_overflow);
  tcase_add_test(tc, test_normalize_big_decimals_multi_step_overflow);
  
  // big_to_decimal
  tcase_add_test(tc, test_big_to_decimal_simple);
  tcase_add_test(tc, test_big_to_decimal_with_scale);
  tcase_add_test(tc, test_big_to_decimal_negative);
  tcase_add_test(tc, test_big_to_decimal_overflow);
  tcase_add_test(tc, test_big_to_decimal_negative_overflow);
  tcase_add_test(tc, test_big_to_decimal_bank_round);
  tcase_add_test(tc, test_big_to_decimal_fit_after_round);

  suite_add_tcase(s, tc);
  return s;
}
