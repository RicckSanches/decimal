#include <check.h>

#include "../s21_decimal.h"

/* ============================ Truncate ================================ */
START_TEST(test_truncate_positive_fraction) {
  s21_decimal d = {{12345, 0, 0, 0x00010000}};  // scale = 1 → 1234.5
  s21_decimal res;
  s21_truncate(d, &res);

  ck_assert_uint_eq(res.bits[0], 1234);
  ck_assert_int_eq(get_scale(res.bits, DEC_BITS), 0);
  ck_assert_int_eq(get_sign(res.bits, DEC_BITS), 0);
}
END_TEST

START_TEST(test_truncate_negative_fraction) {
  s21_decimal d = {{9876, 0, 0, 0x80020000}};  // -98.76, scale=2
  s21_decimal res;
  s21_truncate(d, &res);

  ck_assert_uint_eq(res.bits[0], 98);
  ck_assert_int_eq(get_scale(res.bits, DEC_BITS), 0);
  ck_assert_int_eq(get_sign(res.bits, DEC_BITS), 1);
}
END_TEST

START_TEST(test_truncate_zero) {
  s21_decimal d = {{0, 0, 0, 0}};  // 0
  s21_decimal res;
  s21_truncate(d, &res);

  ck_assert_uint_eq(res.bits[0], 0);
  ck_assert_int_eq(get_sign(res.bits, DEC_BITS), 0);
  ck_assert_int_eq(get_scale(res.bits, DEC_BITS), 0);
}
END_TEST

/* ============================ Negate ================================ */
START_TEST(test_negate_positive) {
  s21_decimal d = {{42, 0, 0, 0}};
  s21_decimal res;
  s21_negate(d, &res);

  ck_assert_uint_eq(res.bits[0], 42);
  ck_assert_int_eq(get_sign(res.bits, DEC_BITS), 1);
}
END_TEST

START_TEST(test_negate_negative) {
  s21_decimal d = {{123, 0, 0, 0x80000000}};
  s21_decimal res;
  s21_negate(d, &res);

  ck_assert_uint_eq(res.bits[0], 123);
  ck_assert_int_eq(get_sign(res.bits, DEC_BITS), 0);
}
END_TEST

START_TEST(test_negate_zero) {
  s21_decimal d = {{0, 0, 0, 0}};  // 0
  s21_decimal res;
  s21_negate(d, &res);

  ck_assert_uint_eq(res.bits[0], 0);
  ck_assert_int_eq(get_sign(res.bits, DEC_BITS), 1);  // отрицательный ноль
}

END_TEST

/* ============================ Floor ================================ */
START_TEST(test_floor_positive_fraction) {
  s21_decimal d = {{12345, 0, 0, 0x00010000}};  // 1234.5
  s21_decimal res;
  s21_floor(d, &res);

  ck_assert_uint_eq(res.bits[0], 1234);
  ck_assert_int_eq(get_sign(res.bits, DEC_BITS), 0);
  ck_assert_int_eq(get_scale(res.bits, DEC_BITS), 0);
}
END_TEST

START_TEST(test_floor_negative_fraction) {
  s21_decimal d = {{12345, 0, 0, 0x80010000}};  // -1234.5
  s21_decimal res;
  s21_floor(d, &res);

  ck_assert_uint_eq(res.bits[0], 1235);  // округление вниз
  ck_assert_int_eq(get_sign(res.bits, DEC_BITS), 1);
  ck_assert_int_eq(get_scale(res.bits, DEC_BITS), 0);
}
END_TEST

START_TEST(test_floor_negative_max_value_fraction) {
  s21_decimal d = {{123456789, 0, 0, 0x80010000}};  // -12345678.9
  s21_decimal res;
  s21_floor(d, &res);

  ck_assert_uint_eq(res.bits[0], 12345679);  // округление вниз
  ck_assert_int_eq(get_sign(res.bits, DEC_BITS), 1);
}
END_TEST

/* ============================ Round ================================ */
START_TEST(test_round_half_up) {
  s21_decimal d = {{125, 0, 0, 0x00010000}};  // 12.5
  s21_decimal res;
  s21_round(d, &res);

  ck_assert_uint_eq(res.bits[0], 13);
  ck_assert_int_eq(get_sign(res.bits, DEC_BITS), 0);
  ck_assert_int_eq(get_scale(res.bits, DEC_BITS), 0);
}
END_TEST

START_TEST(test_round_half_down) {
  s21_decimal d = {{124, 0, 0, 0x00010000}};  // 12.4
  s21_decimal res;
  s21_round(d, &res);

  ck_assert_uint_eq(res.bits[0], 12);
  ck_assert_int_eq(get_sign(res.bits, DEC_BITS), 0);
  ck_assert_int_eq(get_scale(res.bits, DEC_BITS), 0);
}
END_TEST


START_TEST(test_round_negative_half) {
  s21_decimal d = {{125, 0, 0, 0x80010000}};  // -12.5
  s21_decimal res;
  s21_round(d, &res);

  ck_assert_uint_eq(res.bits[0], 13);  // округление вверх по модулю
  ck_assert_int_eq(get_sign(res.bits, DEC_BITS), 1);
}
END_TEST

/* ============================ Suite ================================ */
Suite* decimal_utils_suite(void) {
  Suite* s = suite_create("Decimal Math Functions");
  TCase* tc = tcase_create("Core");

  /* ---------------- Truncate ---------------- */
  tcase_add_test(tc, test_truncate_positive_fraction);
  tcase_add_test(tc, test_truncate_negative_fraction);
  tcase_add_test(tc, test_truncate_zero);

  /* ---------------- Negate ---------------- */
  tcase_add_test(tc, test_negate_positive);
  tcase_add_test(tc, test_negate_negative);
  tcase_add_test(tc, test_negate_zero);

  /* ---------------- Floor ---------------- */
  tcase_add_test(tc, test_floor_positive_fraction);
  tcase_add_test(tc, test_floor_negative_fraction);
  tcase_add_test(tc, test_floor_negative_max_value_fraction);

  /* ---------------- Round ---------------- */
  tcase_add_test(tc, test_round_half_up);
  tcase_add_test(tc, test_round_half_down);
  tcase_add_test(tc, test_round_negative_half);

  suite_add_tcase(s, tc);
  return s;
}