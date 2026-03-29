#include <check.h>
#include <stdio.h>

#include "test_helpers.h"  // bits_equal, assert_number, float_eq

// ===================== Addition Tests =====================
START_TEST(test_add_simple_positive) {
  s21_decimal a = {{5, 0, 0, 0}};
  s21_decimal b = {{3, 0, 0, 0}};
  s21_decimal res;
  s21_decimal expected = {{8, 0, 0, 0}};
  ck_assert_int_eq(s21_add(a, b, &res), S21_OK);
  assert_number(res.bits, expected.bits, DEC_BITS, 0, 0);
}
END_TEST

START_TEST(test_add_positive_negative) {
  s21_decimal a = {{10, 0, 0, 0}};
  s21_decimal b = {{4, 0, 0, 0}};
  set_sign(b.bits, DEC_BITS, 1);
  s21_decimal res;
  s21_decimal expected = {{6, 0, 0, 0}};
  ck_assert_int_eq(s21_add(a, b, &res), S21_OK);
  assert_number(res.bits, expected.bits, DEC_BITS, 0, 0);
}
END_TEST

START_TEST(test_add_with_scale_rounding) {
  s21_decimal a = {{5000, 0, 0, 0}};
  set_scale(a.bits, DEC_BITS, 2);
  s21_decimal b = {{25123, 0, 0, 0}};
  set_scale(b.bits, DEC_BITS, 3);
  s21_decimal res;
  s21_decimal expected = {{75123, 0, 0, 0}};
  set_scale(expected.bits, DEC_BITS, 3);
  ck_assert_int_eq(s21_add(a, b, &res), S21_OK);
  assert_number(res.bits, expected.bits, DEC_BITS, 3, 0);
}
END_TEST

START_TEST(test_add_overflow) {
  s21_decimal a = {{0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0}};
  s21_decimal b = {{1, 0, 0, 0}};
  s21_decimal res;
  ck_assert_int_eq(s21_add(a, b, &res), S21_TOO_BIG);
}
END_TEST

// ===================== Subtraction Tests =====================
START_TEST(test_sub_simple_positive) {
  s21_decimal a = {{10, 0, 0, 0}};
  s21_decimal b = {{3, 0, 0, 0}};
  s21_decimal res;
  s21_decimal expected = {{7, 0, 0, 0}};
  ck_assert_int_eq(s21_sub(a, b, &res), S21_OK);
  assert_number(res.bits, expected.bits, DEC_BITS, 0, 0);
}
END_TEST

START_TEST(test_sub_negative_result) {
  s21_decimal a = {{3, 0, 0, 0}};
  s21_decimal b = {{10, 0, 0, 0}};
  s21_decimal res;
  s21_decimal expected = {{7, 0, 0, 0}};
  set_sign(expected.bits, DEC_BITS, 1);
  set_scale(expected.bits, DEC_BITS, 0);
  ck_assert_int_eq(s21_sub(a, b, &res), S21_OK);
  assert_number(res.bits, expected.bits, DEC_BITS, 0, 1);
}
END_TEST

START_TEST(test_sub_with_scale_rounding) {
  s21_decimal a = {{5000, 0, 0, 0}};
  set_scale(a.bits, DEC_BITS, 2);
  s21_decimal b = {{25567, 0, 0, 0}};
  set_scale(b.bits, DEC_BITS, 3);
  s21_decimal res;
  s21_decimal expected = {{24433, 0, 0, 0}};
  set_scale(expected.bits, DEC_BITS, 3);
  ck_assert_int_eq(s21_sub(a, b, &res), S21_OK);
  assert_number(res.bits, expected.bits, DEC_BITS, 3, 0);
}
END_TEST

START_TEST(test_sub_overflow) {
  s21_decimal a = {{0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0}};
  set_sign(a.bits, DEC_BITS, 1);
  s21_decimal b = {{0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0}};
  set_sign(b.bits, DEC_BITS, 0);
  s21_decimal res;
  ck_assert_int_eq(s21_sub(a, b, &res), S21_TOO_SMALL);
}
END_TEST

// ===================== Multiplication Tests =====================
START_TEST(test_mul_simple_positive) {
  s21_decimal a = {{5, 0, 0, 0}};
  s21_decimal b = {{3, 0, 0, 0}};
  s21_decimal res;
  s21_decimal expected = {{15, 0, 0, 0}};
  ck_assert_int_eq(s21_mul(a, b, &res), S21_OK);
  assert_number(res.bits, expected.bits, DEC_BITS, 0, 0);
}
END_TEST

START_TEST(test_mul_positive_negative) {
  s21_decimal a = {{10, 0, 0, 0}};
  s21_decimal b = {{4, 0, 0, 0}};
  set_sign(b.bits, DEC_BITS, 1);
  s21_decimal res;
  s21_decimal expected = {{40, 0, 0, 0}};
  set_sign(expected.bits, DEC_BITS, 1);
  set_scale(expected.bits, DEC_BITS, 0);
  ck_assert_int_eq(s21_mul(a, b, &res), S21_OK);
  assert_number(res.bits, expected.bits, DEC_BITS, 0, 1);
}
END_TEST

START_TEST(test_mul_with_scale_rounding) {
  s21_decimal a = {{150, 0, 0, 0}};
  set_scale(a.bits, DEC_BITS, 1);
  s21_decimal b = {{20, 0, 0, 0}};
  set_scale(b.bits, DEC_BITS, 1);
  s21_decimal res;
  s21_decimal expected = {{3000, 0, 0, 0}};
  set_scale(expected.bits, DEC_BITS, 2);
  ck_assert_int_eq(s21_mul(a, b, &res), S21_OK);
  assert_number(res.bits, expected.bits, DEC_BITS, 2, 0);
}
END_TEST

START_TEST(test_mul_overflow) {
  s21_decimal a = {{0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0}};
  s21_decimal b = {{2, 0, 0, 0}};
  s21_decimal res;
  ck_assert_int_eq(s21_mul(a, b, &res), S21_TOO_BIG);
}
END_TEST

START_TEST(test_mul_large_scale) {
  s21_decimal a = {{1, 0, 0, 0}};
  set_scale(a.bits, DEC_BITS, 28);
  s21_decimal b = {{1, 0, 0, 0}};
  set_scale(b.bits, DEC_BITS, 28);
  s21_decimal res;
  s21_decimal expected = {{1, 0, 0, 0}};
  set_scale(expected.bits, DEC_BITS, 28);
  ck_assert_int_eq(s21_mul(a, b, &res), S21_OK);
  assert_number(res.bits, expected.bits, DEC_BITS, 28, 0);
}
END_TEST

START_TEST(test_mul_negative_and_large_scale) {
  s21_decimal a = {{5000, 0, 0, 0}};
  s21_decimal b = {{2000, 0, 0, 0}};
  set_scale(a.bits, DEC_BITS, 2);
  set_scale(b.bits, DEC_BITS, 2);
  set_sign(a.bits, DEC_BITS, 1);
  s21_decimal res;
  s21_decimal expected = {{10000000, 0, 0, 0}};
  set_scale(expected.bits, DEC_BITS, 4);
  set_sign(expected.bits, DEC_BITS, 1);
  ck_assert_int_eq(s21_mul(a, b, &res), S21_OK);
  assert_number(res.bits, expected.bits, DEC_BITS, 4, 1);
}
END_TEST

// ===================== Division Tests =====================
START_TEST(test_div_simple_positive) {
  s21_decimal a = {{10, 0, 0, 0}};
  s21_decimal b = {{2, 0, 0, 0}};
  s21_decimal res;
  s21_decimal expected = {{5, 0, 0, 0}};
  ck_assert_int_eq(s21_div(a, b, &res), S21_OK);
  assert_number(res.bits, expected.bits, DEC_BITS, 0, 0);
}
END_TEST

START_TEST(test_div_positive_negative) {
  s21_decimal a = {{20, 0, 0, 0}};
  s21_decimal b = {{4, 0, 0, 0}};
  set_sign(b.bits, DEC_BITS, 1);
  s21_decimal res;
  s21_decimal expected = {{5, 0, 0, 0}};
  set_sign(expected.bits, DEC_BITS, 1);
  set_scale(expected.bits, DEC_BITS, 0);
  ck_assert_int_eq(s21_div(a, b, &res), S21_OK);
  assert_number(res.bits, expected.bits, DEC_BITS, 0, 1);
}
END_TEST

START_TEST(test_div_with_scale) {
  s21_decimal a = {{75, 0, 0, 0}};
  set_scale(a.bits, DEC_BITS, 1);
  s21_decimal b = {{25, 0, 0, 0}};
  set_scale(b.bits, DEC_BITS, 1);
  s21_decimal res;
  s21_decimal expected = {{3, 0, 0, 0}};
  ck_assert_int_eq(s21_div(a, b, &res), S21_OK);
  printf("res: %u, scale: %d, sign: %d\n", res.bits[0],
         get_scale(res.bits, DEC_BITS), get_sign(res.bits, DEC_BITS));
  assert_number(res.bits, expected.bits, DEC_BITS, 0, 0);
}
END_TEST

START_TEST(test_div_fractional) {
  s21_decimal a = {{1, 0, 0, 0}};
  s21_decimal b = {{2, 0, 0, 0}};
  s21_decimal res;
  s21_decimal expected = {{5, 0, 0, 0}};
  set_scale(expected.bits, DEC_BITS, 1);
  ck_assert_int_eq(s21_div(a, b, &res), S21_OK);
  assert_number(res.bits, expected.bits, DEC_BITS, 1, 0);
}
END_TEST

START_TEST(test_div_by_zero) {
  s21_decimal a = {{10, 0, 0, 0}};
  s21_decimal b = {{0, 0, 0, 0}};
  s21_decimal res;
  ck_assert_int_eq(s21_div(a, b, &res), S21_DIV_BY_ZERO);
}
END_TEST

START_TEST(test_div_fractional_max_scale) {
  s21_decimal a = {{1, 0, 0, 0}};
  set_scale(a.bits, DEC_BITS, 28);
  s21_decimal b = {{2, 0, 0, 0}};
  s21_decimal res;
  s21_decimal expected = {{5, 0, 0, 0}};
  set_scale(expected.bits, DEC_BITS, 28);
  ck_assert_int_eq(s21_div(a, b, &res), S21_OK);
  assert_number(res.bits, expected.bits, DEC_BITS, 28, 0);
}
END_TEST

START_TEST(test_div_negative_and_fraction) {
  s21_decimal a = {{5000, 0, 0, 0}};
  s21_decimal b = {{2000, 0, 0, 0}};
  set_scale(a.bits, DEC_BITS, 2);
  set_scale(b.bits, DEC_BITS, 2);
  set_sign(a.bits, DEC_BITS, 1);
  s21_decimal res;
  s21_decimal expected = {{25, 0, 0, 0}};
  set_scale(expected.bits, DEC_BITS, 1);
  set_sign(expected.bits, DEC_BITS, 1);
  ck_assert_int_eq(s21_div(a, b, &res), S21_OK);
  assert_number(res.bits, expected.bits, DEC_BITS, 1, 1);
}
END_TEST

START_TEST(test_div_trailing_zeros) {
  s21_decimal a = {{100, 0, 0, 0}};
  s21_decimal b = {{4, 0, 0, 0}};
  s21_decimal res;

  s21_decimal expected = {{25, 0, 0, 0}};

  ck_assert_int_eq(s21_div(a, b, &res), S21_OK);
  assert_number(res.bits, expected.bits, DEC_BITS, 0, 0);
}
END_TEST

START_TEST(test_div_long_fraction) {
  s21_decimal a = {{1, 0, 0, 0}};
  s21_decimal b = {{3, 0, 0, 0}};
  s21_decimal res;

  ck_assert_int_eq(s21_div(a, b, &res), S21_OK);

  // 0.333333... (проверяем scale)
  ck_assert_int_le(get_scale(res.bits, DEC_BITS), 28);
}
END_TEST

START_TEST(test_div_bank_rounding) {
  s21_decimal a = {{5, 0, 0, 0}};
  s21_decimal b = {{2, 0, 0, 0}};
  s21_decimal res;

  s21_decimal expected = {{25, 0, 0, 0}};
  set_scale(expected.bits, DEC_BITS, 1);  // 2.5

  ck_assert_int_eq(s21_div(a, b, &res), S21_OK);
  assert_number(res.bits, expected.bits, DEC_BITS, 1, 0);
}
END_TEST

START_TEST(test_div_small_result) {
  s21_decimal a = {{1, 0, 0, 0}};
  s21_decimal b = {{10000000, 0, 0, 0}};
  s21_decimal res;

  ck_assert_int_eq(s21_div(a, b, &res), S21_OK);
  ck_assert_int_le(get_scale(res.bits, DEC_BITS), 28);
}
END_TEST

START_TEST(test_div_different_scales) {
  s21_decimal a = {{12345, 0, 0, 0}};
  set_scale(a.bits, DEC_BITS, 2);  // 123.45

  s21_decimal b = {{5, 0, 0, 0}};  // 5

  s21_decimal res;

  s21_decimal expected = {{2469, 0, 0, 0}};
  set_scale(expected.bits, DEC_BITS, 2);  // 24.69

  ck_assert_int_eq(s21_div(a, b, &res), S21_OK);
  assert_number(res.bits, expected.bits, DEC_BITS, 2, 0);
}
END_TEST

START_TEST(test_div_self) {
  s21_decimal a = {{123456, 0, 0, 0}};
  s21_decimal res;

  ck_assert_int_eq(s21_div(a, a, &res), S21_OK);

  s21_decimal expected = {{1, 0, 0, 0}};
  assert_number(res.bits, expected.bits, DEC_BITS, 0, 0);
}
END_TEST

START_TEST(test_div_scale_normalization) {
  s21_decimal a = {{1000, 0, 0, 0}};
  set_scale(a.bits, DEC_BITS, 3);  // 1.000

  s21_decimal b = {{2, 0, 0, 0}};

  s21_decimal res;

  s21_decimal expected = {{5, 0, 0, 0}};
  set_scale(expected.bits, DEC_BITS, 1);  // 0.5

  ck_assert_int_eq(s21_div(a, b, &res), S21_OK);
  assert_number(res.bits, expected.bits, DEC_BITS, 1, 0);
}
END_TEST

START_TEST(test_div_negative_negative) {
  s21_decimal a = {{10, 0, 0, 0}};
  s21_decimal b = {{2, 0, 0, 0}};
  set_sign(a.bits, DEC_BITS, 1);
  set_sign(b.bits, DEC_BITS, 1);

  s21_decimal res;

  s21_decimal expected = {{5, 0, 0, 0}};

  ck_assert_int_eq(s21_div(a, b, &res), S21_OK);
  assert_number(res.bits, expected.bits, DEC_BITS, 0, 0);
}
END_TEST

START_TEST(test_div_overflow) {
  s21_decimal a = {{0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0}};
  s21_decimal b = {{1, 0, 0, 0}};
  s21_decimal res;

  ck_assert_int_eq(s21_div(a, b, &res), S21_OK);
}
END_TEST

// ===================== Suite =====================
Suite* arithmetic_suite(void) {
  Suite* s = suite_create("Decimal Arithmetic");
  TCase* tc_core = tcase_create("Core");

  // Addition
  tcase_add_test(tc_core, test_add_simple_positive);
  tcase_add_test(tc_core, test_add_positive_negative);
  tcase_add_test(tc_core, test_add_with_scale_rounding);
  tcase_add_test(tc_core, test_add_overflow);

  // Subtraction
  tcase_add_test(tc_core, test_sub_simple_positive);
  tcase_add_test(tc_core, test_sub_negative_result);
  tcase_add_test(tc_core, test_sub_with_scale_rounding);
  tcase_add_test(tc_core, test_sub_overflow);

  // Multiplication
  tcase_add_test(tc_core, test_mul_simple_positive);
  tcase_add_test(tc_core, test_mul_positive_negative);
  tcase_add_test(tc_core, test_mul_with_scale_rounding);
  tcase_add_test(tc_core, test_mul_overflow);
  tcase_add_test(tc_core, test_mul_large_scale);
  tcase_add_test(tc_core, test_mul_negative_and_large_scale);

  // Division
  tcase_add_test(tc_core, test_div_simple_positive);
  tcase_add_test(tc_core, test_div_positive_negative);
  tcase_add_test(tc_core, test_div_with_scale);
  tcase_add_test(tc_core, test_div_fractional);
  tcase_add_test(tc_core, test_div_by_zero);
  tcase_add_test(tc_core, test_div_fractional_max_scale);
  tcase_add_test(tc_core, test_div_negative_and_fraction);
  tcase_add_test(tc_core, test_div_trailing_zeros);
  tcase_add_test(tc_core, test_div_long_fraction);
  tcase_add_test(tc_core, test_div_bank_rounding);
  tcase_add_test(tc_core, test_div_small_result);
  tcase_add_test(tc_core, test_div_different_scales);
  tcase_add_test(tc_core, test_div_self);
  tcase_add_test(tc_core, test_div_scale_normalization);
  tcase_add_test(tc_core, test_div_negative_negative);
  tcase_add_test(tc_core, test_div_overflow);

  suite_add_tcase(s, tc_core);
  return s;
}