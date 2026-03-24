#include <check.h>

#include "../s21_decimal.h"

// -------------------- Addition Tests --------------------
START_TEST(test_add_simple_positive) {
  s21_decimal a = {{5, 0, 0, 0}};  // +5
  s21_decimal b = {{3, 0, 0, 0}};  // +3
  s21_decimal res;

  int status = s21_add(a, b, &res);
  ck_assert_int_eq(status, 0);
  ck_assert_int_eq(res.bits[0], 8);
  ck_assert_int_eq(get_sign(res.bits, DEC_BITS), 0);
}
END_TEST

START_TEST(test_add_positive_negative) {
  s21_decimal a = {{10, 0, 0, 0}};  // +10
  s21_decimal b = {{4, 0, 0, 0}};   // -4
  set_sign(b.bits, DEC_BITS, 1);
  s21_decimal res;

  int status = s21_add(a, b, &res);
  ck_assert_int_eq(status, 0);
  ck_assert_int_eq(res.bits[0], 6);
  ck_assert_int_eq(get_sign(res.bits, DEC_BITS), 0);
}
END_TEST

START_TEST(test_add_zero) {
  s21_decimal a = {{0, 0, 0, 0}};
  s21_decimal b = {{0, 0, 0, 0}};
  s21_decimal res;

  int status = s21_add(a, b, &res);
  ck_assert_int_eq(status, 0);
  ck_assert_int_eq(res.bits[0], 0);
}
END_TEST

START_TEST(test_add_with_scale_basic) {
  s21_decimal a = {{150, 0, 0, 0}};
  set_scale(a.bits, DEC_BITS, 1);  // 15.0
  s21_decimal b = {{75, 0, 0, 0}};
  set_scale(b.bits, DEC_BITS, 1);  // 7.5
  s21_decimal res;

  int status = s21_add(a, b, &res);
  ck_assert_int_eq(status, 0);
  ck_assert_int_eq(res.bits[0], 225);  // 22.5 → scale = 1
  ck_assert_int_eq(get_scale(res.bits, DEC_BITS), 1);
}
END_TEST

START_TEST(test_add_with_scale_and_rounding_bankers) {
  s21_decimal a = {{5000, 0, 0, 0}};  // 50.00
  set_scale(a.bits, DEC_BITS, 2);
  s21_decimal b = {{25123, 0, 0, 0}};  // 25.123
  set_scale(b.bits, DEC_BITS, 3);
  s21_decimal res;

  int status = s21_add(a, b, &res);
  ck_assert_int_eq(status, 0);
  ck_assert_int_eq(res.bits[0], 75123);
  ck_assert_int_eq(get_scale(res.bits, DEC_BITS), 3);
}
END_TEST

START_TEST(test_add_with_rounding_overflow) {
  s21_decimal a = {{0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0}};
  s21_decimal b = {{1, 0, 0, 0}};
  s21_decimal res;

  int status = s21_add(a, b, &res);
  ck_assert_int_eq(status, 1);  // положительное переполнение
}
END_TEST

// -------------------- Subtraction Tests --------------------
START_TEST(test_sub_simple) {
  s21_decimal a = {{10, 0, 0, 0}};
  s21_decimal b = {{3, 0, 0, 0}};
  s21_decimal res;

  int status = s21_sub(a, b, &res);
  ck_assert_int_eq(status, 0);
  ck_assert_int_eq(res.bits[0], 7);
  ck_assert_int_eq(get_sign(res.bits, DEC_BITS), 0);
}
END_TEST

START_TEST(test_sub_negative_result) {
  s21_decimal a = {{3, 0, 0, 0}};
  s21_decimal b = {{10, 0, 0, 0}};
  s21_decimal res;

  int status = s21_sub(a, b, &res);
  ck_assert_int_eq(status, 0);
  ck_assert_int_eq(res.bits[0], 7);
  ck_assert_int_eq(get_sign(res.bits, DEC_BITS), 1);
}
END_TEST

START_TEST(test_sub_with_scale_basic) {
  s21_decimal a = {{5000, 0, 0, 0}};  // 50.00
  set_scale(a.bits, DEC_BITS, 2);
  s21_decimal b = {{2500, 0, 0, 0}};  // 25.0
  set_scale(b.bits, DEC_BITS, 1);
  s21_decimal res;

  int status = s21_sub(a, b, &res);
  ck_assert_int_eq(status, 0);
  ck_assert_int_eq(res.bits[0], 20000);  // -200.0
  ck_assert_int_eq(get_scale(res.bits, DEC_BITS), 2);
  ck_assert_int_eq(get_sign(res.bits, DEC_BITS), 1);
}
END_TEST

START_TEST(test_sub_with_scale_and_rounding_bankers) {
  s21_decimal a = {{5000, 0, 0, 0}};  // 50.00
  set_scale(a.bits, DEC_BITS, 2);
  s21_decimal b = {{25567, 0, 0, 0}};  // 25.567
  set_scale(b.bits, DEC_BITS, 3);
  s21_decimal res;

  int status = s21_sub(a, b, &res);
  ck_assert_int_eq(status, 0);
  ck_assert_int_eq(res.bits[0], 24433);
  ck_assert_int_eq(get_scale(res.bits, DEC_BITS), 3);
}
END_TEST

START_TEST(test_sub_with_rounding_overflow) {
  s21_decimal a = {{0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0}};
  set_sign(a.bits, DEC_BITS, 1);  // отрицательное
  s21_decimal b = {{0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0}};
  set_sign(b.bits, DEC_BITS, 0);  // положительное
  s21_decimal res;

  int status = s21_sub(a, b, &res);
  ck_assert_int_eq(status, 2);  // отрицательное переполнение
}
END_TEST

START_TEST(test_sub_with_scale_and_rounding_overflow_mantis) {
  s21_decimal a;
  // a = 792281625142643375593543950335
  a.bits[0] = 0xFFFFFFFF;          // младшие 32 бита
  a.bits[1] = 0xFFFFFFFF;          // средние 32 бита
  a.bits[2] = 0xFFFFFFFF;          // старшие 32 бита
  a.bits[3] = 0;                   // scale + знак
  set_scale(a.bits, DEC_BITS, 0);  // scale = 0

  s21_decimal b = {{6, 0, 0, 0}};  // 0.6 в scale=1
  set_scale(b.bits, DEC_BITS, 1);
  s21_decimal res;
  int status = s21_sub(a, b, &res);
  ck_assert_int_eq(status, 0);
}
END_TEST

// -------------------- Multiplication Tests --------------------
START_TEST(test_mul_simple_positive) {
  s21_decimal a = {{5, 0, 0, 0}};  // +5
  s21_decimal b = {{3, 0, 0, 0}};  // +3
  s21_decimal res;

  int status = s21_mul(a, b, &res);
  ck_assert_int_eq(status, 0);
  ck_assert_int_eq(res.bits[0], 15);
  ck_assert_int_eq(get_sign(res.bits, DEC_BITS), 0);
}
END_TEST

START_TEST(test_mul_positive_negative) {
  s21_decimal a = {{10, 0, 0, 0}};  // +10
  s21_decimal b = {{4, 0, 0, 0}};   // -4
  set_sign(b.bits, DEC_BITS, 1);
  s21_decimal res;

  int status = s21_mul(a, b, &res);
  ck_assert_int_eq(status, 0);
  ck_assert_int_eq(res.bits[0], 40);
  ck_assert_int_eq(get_sign(res.bits, DEC_BITS), 1);
}
END_TEST

START_TEST(test_mul_with_scale) {
  s21_decimal a = {{150, 0, 0, 0}};  // 15.0 scale=1
  set_scale(a.bits, DEC_BITS, 1);
  s21_decimal b = {{20, 0, 0, 0}};  // 2.0 scale=1
  set_scale(b.bits, DEC_BITS, 1);
  s21_decimal res;

  int status = s21_mul(a, b, &res);
  ck_assert_int_eq(status, 0);
  ck_assert_int_eq(res.bits[0], 3000);  // 30.00
  ck_assert_int_eq(get_scale(res.bits, DEC_BITS), 2);
}
END_TEST

// -------------------- Division Tests --------------------
START_TEST(test_div_simple_positive) {
  s21_decimal a = {{10, 0, 0, 0}};  // +10
  s21_decimal b = {{2, 0, 0, 0}};   // +2
  s21_decimal res;

  int status = s21_div(a, b, &res);
  ck_assert_int_eq(status, 0);
  ck_assert_int_eq(res.bits[0], 5);
  ck_assert_int_eq(get_sign(res.bits, DEC_BITS), 0);
}
END_TEST

START_TEST(test_div_positive_negative) {
  s21_decimal a = {{20, 0, 0, 0}};  // +20
  s21_decimal b = {{4, 0, 0, 0}};   // -4
  set_sign(b.bits, DEC_BITS, 1);
  s21_decimal res;

  int status = s21_div(a, b, &res);
  ck_assert_int_eq(status, 0);
  ck_assert_int_eq(res.bits[0], 5);
  ck_assert_int_eq(get_sign(res.bits, DEC_BITS), 1);
}
END_TEST

START_TEST(test_div_with_scale) {
  s21_decimal a = {{75, 0, 0, 0}};  // 7.5 scale=1
  set_scale(a.bits, DEC_BITS, 1);
  s21_decimal b = {{25, 0, 0, 0}};  // 2.5 scale=1
  set_scale(b.bits, DEC_BITS, 1);
  s21_decimal res;

  int status = s21_div(a, b, &res);
  ck_assert_int_eq(status, 0);
  ck_assert_int_eq(res.bits[0], 3);  // 7.5 / 2.5 = 3.0
  ck_assert_int_eq(get_scale(res.bits, DEC_BITS), 0);
}

START_TEST(test_div_by_zero) {
  s21_decimal a = {{10, 0, 0, 0}};
  s21_decimal b = {{0, 0, 0, 0}};
  s21_decimal res;

  int status = s21_div(a, b, &res);
  ck_assert_int_eq(status, 3);  // деление на 0
}
END_TEST

// -------------------- Suite --------------------
Suite* arithmetic_suite(void) {
  Suite* s = suite_create("Decimal Arithmetic");
  TCase* tc_core = tcase_create("Core");

  // Addition
  tcase_add_test(tc_core, test_add_simple_positive);
  tcase_add_test(tc_core, test_add_positive_negative);
  tcase_add_test(tc_core, test_add_zero);
  tcase_add_test(tc_core, test_add_with_scale_basic);
  tcase_add_test(tc_core, test_add_with_scale_and_rounding_bankers);
  tcase_add_test(tc_core, test_add_with_rounding_overflow);

  // Subtraction
  tcase_add_test(tc_core, test_sub_simple);
  tcase_add_test(tc_core, test_sub_negative_result);
  tcase_add_test(tc_core, test_sub_with_scale_basic);
  tcase_add_test(tc_core, test_sub_with_scale_and_rounding_bankers);
  tcase_add_test(tc_core, test_sub_with_rounding_overflow);
  tcase_add_test(tc_core, test_sub_with_scale_and_rounding_overflow_mantis);

  // Multiplication
  tcase_add_test(tc_core, test_mul_simple_positive);
  tcase_add_test(tc_core, test_mul_positive_negative);
  tcase_add_test(tc_core, test_mul_with_scale);

  // Division
  tcase_add_test(tc_core, test_div_simple_positive);
  tcase_add_test(tc_core, test_div_positive_negative);
  tcase_add_test(tc_core, test_div_with_scale);
  tcase_add_test(tc_core, test_div_by_zero);

  suite_add_tcase(s, tc_core);
  return s;
}