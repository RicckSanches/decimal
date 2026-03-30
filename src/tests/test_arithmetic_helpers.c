#include <check.h>
#include <stdio.h>
#include <stdlib.h>

#include "../s21_decimal.h"
#include "test_helpers.h"

// ===================== Decimal -> BigDecimal =====================
START_TEST(test_decimal_to_big_decimal_basic) {
  s21_decimal d = {{0x12345678, 0x9ABCDEF0, 0x0FEDCBA9, 0x80050000}};
  s21_big_decimal b = {{0}};
  decimal_to_big_decimal(&d, &b);

  s21_big_decimal expected = {
      {0x12345678, 0x9ABCDEF0, 0x0FEDCBA9, 0, 0, 0, 0, 0}};
  set_scale(expected.bits, BIG_DEC_BITS, 5);
  set_sign(expected.bits, BIG_DEC_BITS, 1);

  assert_number(b.bits, expected.bits, BIG_DEC_BITS, 5, 1);
}
END_TEST

START_TEST(test_decimal_to_big_decimal_zero) {
  s21_decimal d = {{0, 0, 0, 0}};
  s21_big_decimal b = {{1, 1, 1, 1, 1, 1, 1, 1}};
  decimal_to_big_decimal(&d, &b);

  s21_big_decimal expected = {{0}};
  assert_number(b.bits, expected.bits, BIG_DEC_BITS, 0, 0);
}
END_TEST

START_TEST(test_decimal_to_big_decimal_max) {
  s21_decimal d = {{0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0}};
  set_scale(d.bits, DEC_BITS, 28);
  set_sign(d.bits, DEC_BITS, 1);

  s21_big_decimal b = {{0}};
  decimal_to_big_decimal(&d, &b);

  uint32_t expected_bits[BIG_DEC_BITS / 32] = {0};
  expected_bits[0] = d.bits[0];
  expected_bits[1] = d.bits[1];
  expected_bits[2] = d.bits[2];

  assert_number(b.bits, expected_bits, BIG_DEC_BITS, 28, 1);
}
END_TEST

// ===================== BigDecimal -> Decimal =====================
START_TEST(test_big_to_decimal_basic) {
  s21_big_decimal big = {{12345, 0, 0, 0, 0, 0, 0, 0}};
  set_scale(big.bits, BIG_DEC_BITS, 2);
  set_sign(big.bits, BIG_DEC_BITS, 1);

  s21_decimal dec;
  s21_decimal expected = {{12345, 0, 0, 0}};
  set_scale(expected.bits, DEC_BITS, 2);
  set_sign(expected.bits, DEC_BITS, 1);

  big_decimal_to_decimal(&big, &dec);
  assert_number(dec.bits, expected.bits, DEC_BITS, 2, 1);
}
END_TEST

START_TEST(test_big_to_decimal_zero) {
  s21_big_decimal big = {{0}};
  s21_decimal dec;
  big_decimal_to_decimal(&big, &dec);

  s21_decimal expected = {{0}};
  assert_number(dec.bits, expected.bits, DEC_BITS, 0, 0);
}
END_TEST

START_TEST(test_big_to_decimal_max) {
  s21_big_decimal big = {{0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0, 0, 0, 0, 0}};
  set_scale(big.bits, BIG_DEC_BITS, 28);
  set_sign(big.bits, BIG_DEC_BITS, 1);

  s21_decimal dec;
  big_decimal_to_decimal(&big, &dec);

  s21_decimal expected = {{0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0}};
  set_scale(expected.bits, DEC_BITS, 28);
  set_sign(expected.bits, DEC_BITS, 1);

  assert_number(dec.bits, expected.bits, DEC_BITS, 28, 1);
}
END_TEST

// ===================== Normalize =====================
START_TEST(test_normalize_basic) {
  s21_big_decimal a = {{1, 0, 0, 0, 0, 0, 0, 0}};
  s21_big_decimal b = {{1, 0, 0, 0, 0, 0, 0, 0}};
  set_scale(a.bits, BIG_DEC_BITS, 1);
  set_scale(b.bits, BIG_DEC_BITS, 3);

  normalize_big_decimals(&a, &b);

  s21_big_decimal expected_a = {{100, 0, 0, 0, 0, 0, 0, 0}};
  set_scale(expected_a.bits, BIG_DEC_BITS, 3);

  assert_number(a.bits, expected_a.bits, BIG_DEC_BITS, 3, 0);
  ck_assert_int_eq(get_scale(b.bits, BIG_DEC_BITS), 3);
}
END_TEST

START_TEST(test_normalize_zero) {
  s21_big_decimal a = {{0}};
  s21_big_decimal b = {{123, 0, 0, 0, 0, 0, 0, 0}};
  set_scale(a.bits, BIG_DEC_BITS, 5);
  set_scale(b.bits, BIG_DEC_BITS, 2);

  normalize_big_decimals(&a, &b);

  s21_big_decimal expected_a = {{0}};
  s21_big_decimal expected_b = {{123, 0, 0, 0, 0, 0, 0, 0}};
  set_scale(expected_b.bits, BIG_DEC_BITS, 2);

  assert_number(a.bits, expected_a.bits, BIG_DEC_BITS, 2, 0);
  assert_number(b.bits, expected_b.bits, BIG_DEC_BITS, 2, 0);
}
END_TEST

START_TEST(test_normalize_large) {
  // Исходные big decimals
  s21_big_decimal a = {{1, 0, 0, 0, 0, 0, 0, 0}};
  s21_big_decimal b = {{1, 0, 0, 0, 0, 0, 0, 0}};

  // Устанавливаем масштабы
  set_scale(a.bits, BIG_DEC_BITS, 0);
  set_scale(b.bits, BIG_DEC_BITS, 10);

  // Нормализация
  normalize_big_decimals(&a, &b);

  // Проверка, что масштаб у обоих стал 10
  ck_assert_int_eq(get_scale(a.bits, BIG_DEC_BITS), 10);
  ck_assert_int_eq(get_scale(b.bits, BIG_DEC_BITS), 10);

  // Проверка, что значение a увеличилось до 10^10
  s21_big_decimal expected_a = {
      {0x540BE400, 0x2, 0, 0, 0, 0, 0, 0}};  // 10 000 000 000
  assert_number(a.bits, expected_a.bits, BIG_DEC_BITS, 10, 0);
}
END_TEST

// ===================== div10 =====================
START_TEST(test_div10_basic) {
  uint32_t bits[4] = {123, 0, 0, 0};
  int r = div10(bits, DEC_BITS);
  ck_assert_int_eq(r, 3);
  ck_assert_uint_eq(bits[0], 12);
}
END_TEST

START_TEST(test_div10_zero) {
  uint32_t bits[4] = {0, 0, 0, 0};
  int r = div10(bits, DEC_BITS);
  ck_assert_int_eq(r, 0);
  for (int i = 0; i < 4; i++) ck_assert_uint_eq(bits[i], 0);
}
END_TEST

START_TEST(test_div10_large) {
  uint32_t bits[4] = {1000000000, 0, 0, 0};
  int r = div10(bits, DEC_BITS);
  ck_assert_uint_eq(bits[0], 100000000);
  ck_assert_int_eq(r, 0);
}
END_TEST

// ===================== mul10 =====================
START_TEST(test_mul10_basic) {
  uint32_t bits[4] = {12, 0, 0, 0};
  int ovf = mul10(bits, DEC_BITS);
  ck_assert_uint_eq(bits[0], 120);
  ck_assert_int_eq(ovf, 0);
}
END_TEST

START_TEST(test_mul10_overflow) {
  uint32_t bits[4] = {0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0};
  int ovf = mul10(bits, DEC_BITS);
  ck_assert_int_eq(ovf, 1);
}
END_TEST

START_TEST(test_mul10_zero) {
  uint32_t bits[4] = {0, 0, 0, 0};
  int ovf = mul10(bits, DEC_BITS);
  ck_assert_uint_eq(bits[0], 0);
  ck_assert_int_eq(ovf, 0);
}
END_TEST

// ===================== add_one =====================
START_TEST(test_add_one_basic) {
  uint32_t bits[3] = {0xFFFFFFFF, 0, 0};
  add_one(bits, DEC_BITS);
  ck_assert_uint_eq(bits[0], 0);
  ck_assert_uint_eq(bits[1], 1);
}
END_TEST

START_TEST(test_add_one_zero) {
  uint32_t bits[3] = {0, 0, 0};
  add_one(bits, DEC_BITS);
  ck_assert_uint_eq(bits[0], 1);
  ck_assert_uint_eq(bits[1], 0);
}
END_TEST

START_TEST(test_add_one_overflow) {
  uint32_t bits[3] = {0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF};
  add_one(bits, DEC_BITS);
  ck_assert_uint_eq(bits[0], 0);
  ck_assert_uint_eq(bits[1], 0);
  ck_assert_uint_eq(bits[2], 0);
}
END_TEST

// ===================== should_round =====================
START_TEST(test_should_round_basic) {
  uint32_t bits[3] = {12, 0, 0};
  ck_assert_int_eq(should_round(3, 0, bits), 0);
  ck_assert_int_eq(should_round(7, 0, bits), 1);
  ck_assert_int_eq(should_round(5, 1, bits), 1);
}
END_TEST

START_TEST(test_should_round_zero) {
  uint32_t bits[3] = {0, 0, 0};
  ck_assert_int_eq(should_round(5, 0, bits), 0);
}
END_TEST

START_TEST(test_should_round_even) {
  uint32_t bits[3] = {2, 0, 0};
  ck_assert_int_eq(should_round(5, 0, bits), 0);
}
END_TEST

// ===================== fits_in_decimal =====================

START_TEST(test_fits_in_decimal_all_zero) {
  uint32_t bits[7] = {123, 456, 789, 0, 0, 0, 0};
  int res = fits_in_decimal(bits);
  ck_assert_int_eq(res, 1);  // старшие слова 3..6 нули → fits
}
END_TEST

START_TEST(test_fits_in_decimal_some_nonzero) {
  uint32_t bits[7] = {1, 2, 3, 0, 0, 1, 0};
  int res = fits_in_decimal(bits);
  ck_assert_int_eq(res, 0);  // старшее слово bits[5] != 0 → не fits
}
END_TEST

START_TEST(test_fits_in_decimal_all_nonzero) {
  uint32_t bits[7] = {0, 0, 0, 1, 1, 1, 1};
  int res = fits_in_decimal(bits);
  ck_assert_int_eq(res, 0);  // старшие слова не нули → не fits
}
END_TEST

START_TEST(test_fits_in_decimal_empty) {
  uint32_t bits[7] = {0, 0, 0, 0, 0, 0, 0};
  int res = fits_in_decimal(bits);
  ck_assert_int_eq(res, 1);  // все нули → fits
}
END_TEST

// ===================== compare_big_decimals =====================

START_TEST(test_compare_big_decimals_equal) {
  s21_big_decimal a = {{1, 2, 3, 4, 5, 6, 7, 0}};
  s21_big_decimal b = {{1, 2, 3, 4, 5, 6, 7, 0}};
  int res = compare_big_decimals(&a, &b);
  ck_assert_int_eq(res, 0);  // a == b
}
END_TEST

START_TEST(test_compare_big_decimals_a_greater) {
  s21_big_decimal a = {{1, 2, 3, 4, 5, 6, 7, 0}};
  s21_big_decimal b = {{1, 2, 3, 4, 5, 6, 6, 0}};
  int res = compare_big_decimals(&a, &b);
  ck_assert_int_eq(res, 1);  // a > b
}
END_TEST

START_TEST(test_compare_big_decimals_a_smaller) {
  s21_big_decimal a = {{1, 2, 3, 4, 5, 6, 7, 0}};
  s21_big_decimal b = {{1, 2, 3, 4, 5, 6, 8, 0}};
  int res = compare_big_decimals(&a, &b);
  ck_assert_int_eq(res, -1);  // a < b
}
END_TEST

START_TEST(test_compare_big_decimals_with_zeros) {
  s21_big_decimal a = {{0, 0, 0, 0, 0, 0, 0, 0}};
  s21_big_decimal b = {{0, 0, 0, 0, 0, 0, 0, 0}};
  int res = compare_big_decimals(&a, &b);
  ck_assert_int_eq(res, 0);  // a > b
}
END_TEST

// ===================== big_add =====================

START_TEST(test_big_add_simple) {
  s21_big_decimal a = {{1, 2, 3, 4, 0, 0, 0, 0}};
  s21_big_decimal b = {{5, 6, 7, 8, 0, 0, 0, 0}};
  big_add(&a, &b);
  s21_big_decimal expected = {{6, 8, 10, 12, 0, 0, 0, 0}};
  assert_number(a.bits, expected.bits, BIG_DEC_BITS, 0, 0);
}
END_TEST

START_TEST(test_big_add_with_carry) {
  s21_big_decimal a = {{0xFFFFFFFF, 0xFFFFFFFF, 0, 0, 0, 0, 0, 0}};
  s21_big_decimal b = {{1, 0, 0, 0, 0, 0, 0, 0}};
  big_add(&a, &b);
  s21_big_decimal expected = {
      {0, 0, 1, 0, 0, 0, 0, 0}};  // перенос из двух первых слов
  assert_number(a.bits, expected.bits, BIG_DEC_BITS, 0, 0);
}
END_TEST

START_TEST(test_big_add_zero) {
  s21_big_decimal a = {{0, 0, 0, 0, 0, 0, 0, 0}};
  s21_big_decimal b = {{0, 0, 0, 0, 0, 0, 0, 0}};
  big_add(&a, &b);
  s21_big_decimal expected = {{0, 0, 0, 0, 0, 0, 0, 0}};
  assert_number(a.bits, expected.bits, BIG_DEC_BITS, 0, 0);
}
END_TEST

START_TEST(test_big_add_large_numbers) {
  s21_big_decimal a = {
      {0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0, 0, 0, 0}};
  s21_big_decimal b = {{0x1, 0x0, 0x0, 0x0, 0, 0, 0, 0}};
  big_add(&a, &b);
  s21_big_decimal expected = {
      {0, 0, 0, 0, 1, 0, 0, 0}};  // перенос через 4 слова
  assert_number(a.bits, expected.bits, BIG_DEC_BITS, 0, 0);
}
END_TEST

// ===================== big_sub tests =====================
START_TEST(test_big_sub_simple) {
  s21_big_decimal a = {{10, 20, 30, 40, 0, 0, 0, 0}};
  s21_big_decimal b = {{1, 2, 3, 4, 0, 0, 0, 0}};
  big_sub(&a, &b);
  s21_big_decimal expected = {{9, 18, 27, 36, 0, 0, 0, 0}};
  assert_number(a.bits, expected.bits, BIG_DEC_BITS, 0, 0);
}
END_TEST

START_TEST(test_big_sub_with_borrow) {
  s21_big_decimal a = {{0, 0, 1, 0, 0, 0, 0, 0}};
  s21_big_decimal b = {{1, 0, 0, 0, 0, 0, 0, 0}};
  big_sub(&a, &b);
  s21_big_decimal expected = {{0xFFFFFFFF, 0xFFFFFFFF, 0, 0, 0, 0, 0, 0}};
  assert_number(a.bits, expected.bits, BIG_DEC_BITS, 0, 0);
}
END_TEST

START_TEST(test_big_sub_zero) {
  s21_big_decimal a = {{123, 456, 789, 0, 0, 0, 0, 0}};
  s21_big_decimal b = {{0, 0, 0, 0, 0, 0, 0, 0}};
  big_sub(&a, &b);
  s21_big_decimal expected = {{123, 456, 789, 0, 0, 0, 0, 0}};
  assert_number(a.bits, expected.bits, BIG_DEC_BITS, 0, 0);
}
END_TEST

START_TEST(test_big_sub_large_numbers) {
  s21_big_decimal a = {{0, 0, 0, 1, 0, 0, 0, 0}};
  s21_big_decimal b = {{1, 0, 0, 0, 0, 0, 0, 0}};
  big_sub(&a, &b);
  s21_big_decimal expected = {
      {0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0, 0, 0, 0, 0}};
  assert_number(a.bits, expected.bits, BIG_DEC_BITS, 0, 0);
}
END_TEST

// ===================== big_mul tests =====================
START_TEST(test_big_mul_simple) {
  s21_big_decimal a = {{2, 3, 0, 0, 0, 0, 0, 0}};
  s21_big_decimal b = {{4, 5, 0, 0, 0, 0, 0, 0}};
  s21_big_decimal res = {{0}};
  big_mul(&a, &b, &res);

  // Проверка вручную: 2*4=8, 2*5+3*4=10+12=22, 3*5=15
  s21_big_decimal expected = {{8, 22, 15, 0, 0, 0, 0, 0}};
  assert_number(res.bits, expected.bits, BIG_DEC_BITS, 0, 0);
}
END_TEST

START_TEST(test_big_mul_with_zero) {
  s21_big_decimal a = {{123, 456, 789, 0, 0, 0, 0, 0}};
  s21_big_decimal b = {{0, 0, 0, 0, 0, 0, 0, 0}};
  s21_big_decimal res = {{0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0, 0, 0, 0, 0}};
  big_mul(&a, &b, &res);

  s21_big_decimal expected = {{0}};
  assert_number(res.bits, expected.bits, BIG_DEC_BITS, 0, 0);
}
END_TEST

START_TEST(test_big_mul_carry) {
  s21_big_decimal a = {{0xFFFFFFFF, 1, 0, 0, 0, 0, 0, 0}};
  s21_big_decimal b = {{2, 0, 0, 0, 0, 0, 0, 0}};
  s21_big_decimal res = {{0}};
  big_mul(&a, &b, &res);

  // Проверка: 0xFFFFFFFF*2=0x1FFFFFFFE -> младшее слово = 0xFFFFFFFE, carry=1
  s21_big_decimal expected = {{0xFFFFFFFE, 3, 0, 0, 0, 0, 0, 0}};
  assert_number(res.bits, expected.bits, BIG_DEC_BITS, 0, 0);
}
END_TEST

START_TEST(test_big_mul_large_numbers) {
  s21_big_decimal a = {{0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0, 0, 0, 0, 0}};
  s21_big_decimal b = {{2, 0, 0, 0, 0, 0, 0, 0}};
  s21_big_decimal res = {{0}};
  big_mul(&a, &b, &res);

  s21_big_decimal expected = {
      {0xFFFFFFFE, 0xFFFFFFFF, 0xFFFFFFFF, 1, 0, 0, 0, 0}};
  assert_number(res.bits, expected.bits, BIG_DEC_BITS, 0, 0);
}
END_TEST

// ===================== Big Div Digit =====================
START_TEST(test_big_div_digit_basic) {
  // Делимое = 123, делитель = 12
  s21_big_decimal divisor = {{12, 0, 0, 0, 0, 0, 0, 0}};
  s21_big_decimal remainder = {{123, 0, 0, 0, 0, 0, 0, 0}};
  uint32_t digit = 0;

  big_div_digit(&divisor, &remainder, &digit);

  // Проверяем, что digit максимально возможная цифра <= 9
  ck_assert_uint_eq(digit, 9);  // 12 * 9 = 108 <= 123

  // Проверяем остаток после вычитания divisor * digit
  s21_big_decimal expected_remainder = {
      {15, 0, 0, 0, 0, 0, 0, 0}};  // 123 - 108 = 15
  assert_number(remainder.bits, expected_remainder.bits, BIG_DEC_BITS, 0, 0);
}
END_TEST

START_TEST(test_big_div_digit_zero) {
  s21_big_decimal a = {{5, 0, 0, 0, 0, 0, 0, 0}};
  s21_big_decimal r = {{3, 0, 0, 0, 0, 0, 0, 0}};
  uint32_t digit = 0;
  big_div_digit(&a, &r, &digit);
  ck_assert_uint_eq(digit, 0);
}
END_TEST

// ===================== apply_bankers_rounding tests =====================
START_TEST(test_bankers_rounding_no_change) {
  s21_big_decimal val = {{12345, 0, 0, 0, 0, 0, 0, 0}};
  int scale = 0;
  int status = apply_bankers_rounding(&val, &scale);

  ck_assert_int_eq(status, S21_OK);
  ck_assert_uint_eq(val.bits[0], 12345);
  ck_assert_int_eq(scale, 0);
}
END_TEST

START_TEST(test_bankers_rounding_simple) {
  s21_big_decimal val = {
      {0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0, 0, 0, 0, 0}};  // реально большое
  int scale = 29;

  int status = apply_bankers_rounding(&val, &scale);

  ck_assert_int_eq(status, S21_OK);
  ck_assert_int_eq(scale, 28);
}
END_TEST

START_TEST(test_bankers_rounding_bankers_even) {
  s21_big_decimal val = {
      {0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0, 0, 0, 0, 0}};  // большое число
  int scale = 0;

  int status = apply_bankers_rounding(&val, &scale);

  ck_assert_int_eq(status, S21_OK);
  ck_assert(scale == 0);
}
END_TEST

START_TEST(test_bankers_rounding_overflow) {
  s21_big_decimal val = {
      {0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0x0, 0x0, 0x0, 0x0}};
  int scale = 0;
  int status = apply_bankers_rounding(&val, &scale);

  // не помещается -> S21_TOO_BIG
  ck_assert_int_eq(status, S21_TOO_BIG);
}
END_TEST

START_TEST(test_bankers_rounding_scale_limit) {
  s21_big_decimal val = {{123456, 0, 0, 0, 0, 0, 0, 0}};
  int scale = 35;
  int status = apply_bankers_rounding(&val, &scale);

  ck_assert_int_eq(status, S21_OK);
  ck_assert_int_eq(scale, 0);
}
END_TEST

// ===================== Suite =====================
Suite* arithmetic_helpers_suite(void) {
  Suite* s = suite_create("Decimal <-> BigDecimal Helpers");
  TCase* tc = tcase_create("Core");

  // Decimal <-> BigDecimal
  tcase_add_test(tc, test_decimal_to_big_decimal_basic);
  tcase_add_test(tc, test_decimal_to_big_decimal_zero);
  tcase_add_test(tc, test_decimal_to_big_decimal_max);

  tcase_add_test(tc, test_big_to_decimal_basic);
  tcase_add_test(tc, test_big_to_decimal_zero);
  tcase_add_test(tc, test_big_to_decimal_max);

  // Normalize
  tcase_add_test(tc, test_normalize_basic);
  tcase_add_test(tc, test_normalize_zero);
  tcase_add_test(tc, test_normalize_large);

  // div10
  tcase_add_test(tc, test_div10_basic);
  tcase_add_test(tc, test_div10_zero);
  tcase_add_test(tc, test_div10_large);

  // mul10
  tcase_add_test(tc, test_mul10_basic);
  tcase_add_test(tc, test_mul10_overflow);
  tcase_add_test(tc, test_mul10_zero);

  // add_one
  tcase_add_test(tc, test_add_one_basic);
  tcase_add_test(tc, test_add_one_zero);
  tcase_add_test(tc, test_add_one_overflow);

  // should_round
  tcase_add_test(tc, test_should_round_basic);
  tcase_add_test(tc, test_should_round_zero);
  tcase_add_test(tc, test_should_round_even);

  // fits_in_decimal
  tcase_add_test(tc, test_fits_in_decimal_all_zero);
  tcase_add_test(tc, test_fits_in_decimal_some_nonzero);
  tcase_add_test(tc, test_fits_in_decimal_all_nonzero);
  tcase_add_test(tc, test_fits_in_decimal_empty);

  // compare_big_decimals
  tcase_add_test(tc, test_compare_big_decimals_equal);
  tcase_add_test(tc, test_compare_big_decimals_a_greater);
  tcase_add_test(tc, test_compare_big_decimals_a_smaller);
  tcase_add_test(tc, test_compare_big_decimals_with_zeros);

  // big_add
  tcase_add_test(tc, test_big_add_simple);
  tcase_add_test(tc, test_big_add_with_carry);
  tcase_add_test(tc, test_big_add_zero);
  tcase_add_test(tc, test_big_add_large_numbers);

  // big_sub
  tcase_add_test(tc, test_big_sub_simple);
  tcase_add_test(tc, test_big_sub_with_borrow);
  tcase_add_test(tc, test_big_sub_zero);
  tcase_add_test(tc, test_big_sub_large_numbers);

  // big_mul
  tcase_add_test(tc, test_big_mul_simple);
  tcase_add_test(tc, test_big_mul_with_zero);
  tcase_add_test(tc, test_big_mul_carry);
  tcase_add_test(tc, test_big_mul_large_numbers);

  // Big Div Digit
  tcase_add_test(tc, test_big_div_digit_basic);
  tcase_add_test(tc, test_big_div_digit_zero);

  // apply_bankers_rounding tests
  tcase_add_test(tc, test_bankers_rounding_no_change);
  tcase_add_test(tc, test_bankers_rounding_simple);
  tcase_add_test(tc, test_bankers_rounding_bankers_even);
  tcase_add_test(tc, test_bankers_rounding_overflow);
  tcase_add_test(tc, test_bankers_rounding_scale_limit);

  suite_add_tcase(s, tc);
  return s;
}