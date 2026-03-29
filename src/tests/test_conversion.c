#include <check.h>
#include <math.h>
#include "../s21_decimal.h"
#include "test_helpers.h"

// ===================== INT -> DECIMAL =====================
START_TEST(test_from_int_positive) {
  s21_decimal dec;
  int res = s21_from_int_to_decimal(12345, &dec);
  ck_assert_int_eq(res, 0);
  uint32_t expected[4] = {12345, 0, 0, 0};
  assert_number(dec.bits, expected, DEC_BITS, 0, 0);
}
END_TEST

START_TEST(test_from_int_negative) {
  s21_decimal dec;
  int res = s21_from_int_to_decimal(-98765, &dec);
  ck_assert_int_eq(res, 0);
  uint32_t expected[4] = {98765, 0, 0, 0};
  assert_number(dec.bits, expected, DEC_BITS, 0, 1);
}
END_TEST

START_TEST(test_from_int_zero) {
  s21_decimal dec;
  int res = s21_from_int_to_decimal(0, &dec);
  ck_assert_int_eq(res, 0);
  uint32_t expected[4] = {0, 0, 0, 0};
  assert_number(dec.bits, expected, DEC_BITS, 0, 0);
}
END_TEST

START_TEST(test_from_int_max_min) {
  s21_decimal dec;
  int res;

  res = s21_from_int_to_decimal(INT32_MAX, &dec);
  ck_assert_int_eq(res, 0);
  uint32_t expected_max[4] = {INT32_MAX, 0, 0, 0};
  assert_number(dec.bits, expected_max, DEC_BITS, 0, 0);

  res = s21_from_int_to_decimal(INT32_MIN, &dec);
  ck_assert_int_eq(res, 0);
  uint32_t expected_min[4] = {2147483648U, 0, 0, 0};
  assert_number(dec.bits, expected_min, DEC_BITS, 0, 1);
}
END_TEST

START_TEST(test_from_int_null_pointer) {
  int res = s21_from_int_to_decimal(100, NULL);
  ck_assert_int_eq(res, 1);
}
END_TEST

// ===================== FLOAT -> DECIMAL =====================
START_TEST(test_from_float_basic) {
  s21_decimal dec;
  float src = 123.456f;
  int res = s21_from_float_to_decimal(src, &dec);
  ck_assert_int_eq(res, 0);

  float f;
  s21_from_decimal_to_float(dec, &f);
  ck_assert(float_eq(f, src, 1e-3f));
}
END_TEST

START_TEST(test_from_float_negative_zero_overflow) {
  s21_decimal dec;
  int res;

  // отрицательный ноль
  res = s21_from_float_to_decimal(-0.0f, &dec);
  ck_assert_int_eq(res, 0);
  uint32_t expected_neg_zero[4] = {0, 0, 0, 0};
  assert_number(dec.bits, expected_neg_zero, DEC_BITS, 0, 0);

  // большой float → overflow
  res = s21_from_float_to_decimal(5e9f, &dec);
  ck_assert_int_eq(res, 0);
}
END_TEST

START_TEST(test_from_float_small_fraction) {
  s21_decimal dec;
  float src = 0.000123f;
  int res = s21_from_float_to_decimal(src, &dec);
  ck_assert_int_eq(res, 0);

  float f;
  s21_from_decimal_to_float(dec, &f);
  ck_assert(float_eq(f, src, 1e-6f));
}
END_TEST

START_TEST(test_from_float_negative_fraction) {
  s21_decimal dec;
  float src = -0.4567f;
  int res = s21_from_float_to_decimal(src, &dec);
  ck_assert_int_eq(res, 0);

  float f;
  s21_from_decimal_to_float(dec, &f);
  ck_assert(float_eq(f, src, 1e-5f));

  ck_assert_int_eq(get_sign(dec.bits, DEC_BITS), 1);
}
END_TEST

START_TEST(test_from_float_large_scale) {
  s21_decimal dec;
  float src = 0.123456789f;
  int res = s21_from_float_to_decimal(src, &dec);
  ck_assert_int_eq(res, 0);

  float f;
  s21_from_decimal_to_float(dec, &f);
  ck_assert(float_eq(f, src, 1e-8f));
}
END_TEST

// ===================== DECIMAL -> INT =====================
START_TEST(test_decimal_to_int_basic) {
  s21_decimal dec;
  int i, res;

  s21_from_int_to_decimal(100, &dec);
  res = s21_from_decimal_to_int(dec, &i);
  ck_assert_int_eq(res, 0);
  ck_assert_int_eq(i, 100);

  s21_from_int_to_decimal(-100, &dec);
  res = s21_from_decimal_to_int(dec, &i);
  ck_assert_int_eq(res, 0);
  ck_assert_int_eq(i, -100);

  s21_from_float_to_decimal(123.987f, &dec);
  res = s21_from_decimal_to_int(dec, &i);
  ck_assert_int_eq(res, 0);
  ck_assert_int_eq(i, 123);
}
END_TEST

START_TEST(test_decimal_to_int_with_scale) {
  s21_decimal dec;
  int i, res;
  s21_from_float_to_decimal(456.789f, &dec);  // scale = 3
  res = s21_from_decimal_to_int(dec, &i);
  ck_assert_int_eq(res, 0);
  ck_assert_int_eq(i, 456);
}
END_TEST

START_TEST(test_decimal_to_int_null_overflow) {
  s21_decimal dec;
  int i, res;

  s21_from_int_to_decimal(123, &dec);
  res = s21_from_decimal_to_int(dec, NULL);
  ck_assert_int_eq(res, 1);

  s21_from_int_to_decimal(INT32_MAX, &dec);
  dec.bits[0] += 1;  // искусственно создаем переполнение
  res = s21_from_decimal_to_int(dec, &i);
  ck_assert_int_eq(res, 1);
}
END_TEST

// ===================== DECIMAL -> FLOAT =====================
START_TEST(test_decimal_to_float_basic) {
  s21_decimal dec;
  float f;
  int res;

  s21_from_float_to_decimal(3.14159f, &dec);
  res = s21_from_decimal_to_float(dec, &f);
  ck_assert_int_eq(res, 0);
  ck_assert(float_eq(f, 3.14159f, 1e-5f));

  s21_from_float_to_decimal(-2.71828f, &dec);
  res = s21_from_decimal_to_float(dec, &f);
  ck_assert_int_eq(res, 0);
  ck_assert(float_eq(f, -2.71828f, 1e-5f));

  s21_from_float_to_decimal(0.0f, &dec);
  res = s21_from_decimal_to_float(dec, &f);
  ck_assert_int_eq(res, 0);
  ck_assert(fabs(f) < 1e-7f);
}
END_TEST

START_TEST(test_decimal_to_float_large_scale) {
  s21_decimal dec;
  float f;
  s21_from_float_to_decimal(0.00000012345f, &dec);
  int res = s21_from_decimal_to_float(dec, &f);
  ck_assert_int_eq(res, 0);
  ck_assert(float_eq(f, 0.00000012345f, 1e-10f));
}
END_TEST

START_TEST(test_decimal_to_float_null) {
  s21_decimal dec;
  s21_from_float_to_decimal(1.23f, &dec);
  int res = s21_from_decimal_to_float(dec, NULL);
  ck_assert_int_eq(res, 1);
}
END_TEST

// ===================== SUITE =====================
Suite* conversion_suite(void) {
  Suite* s = suite_create("Decimal Conversion");
  TCase* tc = tcase_create("Core");

  // int -> decimal
  tcase_add_test(tc, test_from_int_positive);
  tcase_add_test(tc, test_from_int_negative);
  tcase_add_test(tc, test_from_int_zero);
  tcase_add_test(tc, test_from_int_max_min);
  tcase_add_test(tc, test_from_int_null_pointer);

  // float -> decimal
  tcase_add_test(tc, test_from_float_basic);
  tcase_add_test(tc, test_from_float_negative_zero_overflow);
  tcase_add_test(tc, test_from_float_small_fraction);
  tcase_add_test(tc, test_from_float_negative_fraction);
  tcase_add_test(tc, test_from_float_large_scale);

  // decimal -> int
  tcase_add_test(tc, test_decimal_to_int_basic);
  tcase_add_test(tc, test_decimal_to_int_with_scale);
  tcase_add_test(tc, test_decimal_to_int_null_overflow);

  // decimal -> float
  tcase_add_test(tc, test_decimal_to_float_basic);
  tcase_add_test(tc, test_decimal_to_float_large_scale);
  tcase_add_test(tc, test_decimal_to_float_null);

  suite_add_tcase(s, tc);
  return s;
}