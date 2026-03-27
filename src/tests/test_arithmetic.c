#include <check.h>
#include <stdio.h>

#include "../s21_decimal.h"

// ===================== Helpers =====================
int decimals_equal(s21_decimal a, s21_decimal b) {
  for (int i = 0; i < 4; i++) {
    if (a.bits[i] != b.bits[i]) return 0;
  }
  return 1;
}

// Проверка всех битов, scale и знака
void assert_decimal(s21_decimal res, s21_decimal expected, int expected_scale,
                    int expected_sign) {
  ck_assert_msg(decimals_equal(res, expected), "Decimal bits mismatch");
  ck_assert_int_eq(get_scale(res.bits, DEC_BITS), expected_scale);
  ck_assert_int_eq(get_sign(res.bits, DEC_BITS), expected_sign);
}

// ===================== Addition Tests =====================
START_TEST(test_add_simple_positive) {
  s21_decimal a = {{5, 0, 0, 0}};
  s21_decimal b = {{3, 0, 0, 0}};
  s21_decimal res;
  s21_decimal expected = {{8, 0, 0, 0}};
  ck_assert_int_eq(s21_add(a, b, &res), S21_OK);
  assert_decimal(res, expected, 0, 0);
}
END_TEST

START_TEST(test_add_positive_negative) {
  s21_decimal a = {{10, 0, 0, 0}};
  s21_decimal b = {{4, 0, 0, 0}};
  set_sign(b.bits, DEC_BITS, 1);
  s21_decimal res;
  s21_decimal expected = {{6, 0, 0, 0}};
  ck_assert_int_eq(s21_add(a, b, &res), S21_OK);
  assert_decimal(res, expected, 0, 0);
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
  assert_decimal(res, expected, 3, 0);
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
  assert_decimal(res, expected, 0, 0);
}
END_TEST

START_TEST(test_sub_negative_result) {
  s21_decimal a = {{3, 0, 0, 0}};
  s21_decimal b = {{10, 0, 0, 0}};
  s21_decimal res;

  s21_decimal expected = {{7, 0, 0, 0}};  // |-7|
  set_sign(expected.bits, DEC_BITS, 1);   // знак минус
  set_scale(expected.bits, DEC_BITS, 0);  // scale 0

  ck_assert_int_eq(s21_sub(a, b, &res), S21_OK);
  assert_decimal(res, expected, 0, 1);
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
  assert_decimal(res, expected, 3, 0);
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
  assert_decimal(res, expected, 0, 0);
}
END_TEST

START_TEST(test_mul_positive_negative) {
  s21_decimal a = {{10, 0, 0, 0}};
  s21_decimal b = {{4, 0, 0, 0}};
  set_sign(b.bits, DEC_BITS, 1);  // b отрицательный
  s21_decimal res;

  s21_decimal expected = {{40, 0, 0, 0}};
  set_sign(expected.bits, DEC_BITS, 1);
  set_scale(expected.bits, DEC_BITS, 0);

  ck_assert_int_eq(s21_mul(a, b, &res), S21_OK);
  assert_decimal(res, expected, 0, 1);
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
  assert_decimal(res, expected, 2, 0);
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
  assert_decimal(res, expected, 28, 0);
}
END_TEST

START_TEST(test_mul_negative_and_large_scale) {
  s21_decimal a = {{5000, 0, 0, 0}};  // 50.00
  s21_decimal b = {{2000, 0, 0, 0}};  // 20.00
  set_scale(a.bits, DEC_BITS, 2);
  set_scale(b.bits, DEC_BITS, 2);
  set_sign(a.bits, DEC_BITS, 1);  // a отрицательный
  s21_decimal res;

  s21_decimal expected = {{10000000, 0, 0, 0}};  // 50*20=1000, но scale=4
  set_scale(expected.bits, DEC_BITS, 4);         // масштаб = 2+2
  set_sign(expected.bits, DEC_BITS, 1);          // знак минус

  ck_assert_int_eq(s21_mul(a, b, &res), S21_OK);
  assert_decimal(res, expected, 4, 1);
}
END_TEST

// ===================== Division Tests =====================
START_TEST(test_div_simple_positive) {
  s21_decimal a = {{10, 0, 0, 0}};
  s21_decimal b = {{2, 0, 0, 0}};
  s21_decimal res;
  s21_decimal expected = {{5, 0, 0, 0}};
  ck_assert_int_eq(s21_div(a, b, &res), S21_OK);
  assert_decimal(res, expected, 0, 0);
}
END_TEST

START_TEST(test_div_positive_negative) {
  s21_decimal a = {{20, 0, 0, 0}};
  s21_decimal b = {{4, 0, 0, 0}};
  set_sign(b.bits, DEC_BITS, 1);  // b отрицательный
  s21_decimal res;

  s21_decimal expected = {{5, 0, 0, 0}};
  set_sign(expected.bits, DEC_BITS, 1);  // результат отрицательный
  set_scale(expected.bits, DEC_BITS, 0);

  ck_assert_int_eq(s21_div(a, b, &res), S21_OK);
  assert_decimal(res, expected, 0, 1);
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
  assert_decimal(res, expected, 0, 0);
}
END_TEST

START_TEST(test_div_fractional) {
  s21_decimal a = {{1, 0, 0, 0}};
  s21_decimal b = {{2, 0, 0, 0}};
  s21_decimal res;
  s21_decimal expected = {{5, 0, 0, 0}};
  set_scale(expected.bits, DEC_BITS, 1);
  ck_assert_int_eq(s21_div(a, b, &res), S21_OK);
  assert_decimal(res, expected, 1, 0);
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
  printf("res: %u, scale: %d, sign: %d\n", res.bits[0],
         get_scale(res.bits, DEC_BITS), get_sign(res.bits, DEC_BITS));
  assert_decimal(res, expected, 28, 0);
}
END_TEST

START_TEST(test_div_negative_and_fraction) {
  s21_decimal a = {{5000, 0, 0, 0}};  // 50.00
  s21_decimal b = {{2000, 0, 0, 0}};  // 20.00
  set_scale(a.bits, DEC_BITS, 2);
  set_scale(b.bits, DEC_BITS, 2);
  set_sign(a.bits, DEC_BITS, 1);  // a отрицательный
  s21_decimal res;

  s21_decimal expected = {{250, 0, 0, 0}};  // 50/20 = 2.5, scale=2 → 2.50 → 250
  set_scale(expected.bits, DEC_BITS, 2);
  set_sign(expected.bits, DEC_BITS, 1);

  ck_assert_int_eq(s21_div(a, b, &res), S21_OK);
  assert_decimal(res, expected, 2, 1);
}
END_TEST

// ===================== Suite =====================
Suite* arithmetic_suite(void) {
  Suite* s = suite_create("Decimal Arithmetic");
  TCase* tc_core = tcase_create("Core");

  // ===================== Addition =====================
  tcase_add_test(tc_core, test_add_simple_positive);
  tcase_add_test(tc_core, test_add_positive_negative);
  tcase_add_test(tc_core, test_add_with_scale_rounding);
  tcase_add_test(tc_core, test_add_overflow);

  // ===================== Subtraction =====================
  tcase_add_test(tc_core, test_sub_simple_positive);
  tcase_add_test(tc_core, test_sub_negative_result);
  tcase_add_test(tc_core, test_sub_with_scale_rounding);
  tcase_add_test(tc_core, test_sub_overflow);

  // ===================== Multiplication =====================
  tcase_add_test(tc_core, test_mul_simple_positive);
  tcase_add_test(tc_core, test_mul_positive_negative);
  tcase_add_test(tc_core, test_mul_with_scale_rounding);
  tcase_add_test(tc_core, test_mul_overflow);
  tcase_add_test(tc_core, test_mul_large_scale);
  tcase_add_test(tc_core, test_mul_negative_and_large_scale);

  // ===================== Division =====================
  tcase_add_test(tc_core, test_div_simple_positive);
  tcase_add_test(tc_core, test_div_positive_negative);
  tcase_add_test(tc_core, test_div_with_scale);
  tcase_add_test(tc_core, test_div_fractional);
  tcase_add_test(tc_core, test_div_by_zero);
  tcase_add_test(tc_core, test_div_fractional_max_scale);
  tcase_add_test(tc_core, test_div_negative_and_fraction);

  suite_add_tcase(s, tc_core);
  return s;
}