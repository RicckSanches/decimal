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

  ck_assert_uint_eq(res.bits[0], 12);
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

  ck_assert_uint_eq(res.bits[0], 12);  // округление вверх по модулю
  ck_assert_int_eq(get_sign(res.bits, DEC_BITS), 1);
}
END_TEST

/* ============================ EXTRA EDGE TESTS ===========================*/

/* ---------- TRUNCATE ---------- */

// scale > 1
START_TEST(test_truncate_large_scale) {
  s21_decimal d = {{123456, 0, 0, 0x00030000}};  // 123.456
  s21_decimal res;
  s21_truncate(d, &res);

  ck_assert_uint_eq(res.bits[0], 123);
  ck_assert_int_eq(get_scale(res.bits, DEC_BITS), 0);
}
END_TEST

// уже целое число
START_TEST(test_truncate_integer) {
  s21_decimal d = {{999, 0, 0, 0}};  // 999
  s21_decimal res;
  s21_truncate(d, &res);

  ck_assert_uint_eq(res.bits[0], 999);
}
END_TEST

/* ---------- NEGATE ---------- */

// двойной negate
START_TEST(test_negate_double) {
  s21_decimal d = {{777, 0, 0, 0x80000000}};
  s21_decimal tmp, res;

  s21_negate(d, &tmp);
  s21_negate(tmp, &res);

  ck_assert_uint_eq(res.bits[0], 777);
  ck_assert_int_eq(get_sign(res.bits, DEC_BITS), 1);
}
END_TEST

// сохранение scale
START_TEST(test_negate_preserve_scale) {
  s21_decimal d = {{12345, 0, 0, 0x00020000}};  // 123.45
  s21_decimal res;

  s21_negate(d, &res);

  ck_assert_int_eq(get_scale(res.bits, DEC_BITS), 2);
}
END_TEST

/* ---------- FLOOR ---------- */

// отрицательное маленькое число
START_TEST(test_floor_negative_small) {
  s21_decimal d = {{1, 0, 0, 0x80010000}};  // -0.1
  s21_decimal res;

  s21_floor(d, &res);

  ck_assert_uint_eq(res.bits[0], 1);
  ck_assert_int_eq(get_sign(res.bits, DEC_BITS), 1);  // -1
}
END_TEST

// положительное маленькое
START_TEST(test_floor_positive_small) {
  s21_decimal d = {{1, 0, 0, 0x00010000}};  // 0.1
  s21_decimal res;

  s21_floor(d, &res);

  ck_assert_uint_eq(res.bits[0], 0);
}
END_TEST

// уже целое (отрицательное)
START_TEST(test_floor_negative_integer) {
  s21_decimal d = {{123, 0, 0, 0x80000000}};  // -123
  s21_decimal res;

  s21_floor(d, &res);

  ck_assert_uint_eq(res.bits[0], 123);
  ck_assert_int_eq(get_sign(res.bits, DEC_BITS), 1);
}
END_TEST

/* ---------- ROUND (BANKER'S ROUNDING) ---------- */

// 2.5 → 2
START_TEST(test_round_bank_even_down) {
  s21_decimal d = {{25, 0, 0, 0x00010000}};  // 2.5
  s21_decimal res;

  s21_round(d, &res);

  ck_assert_uint_eq(res.bits[0], 2);
}
END_TEST

// 3.5 → 4
START_TEST(test_round_bank_even_up) {
  s21_decimal d = {{35, 0, 0, 0x00010000}};  // 3.5
  s21_decimal res;

  s21_round(d, &res);

  ck_assert_uint_eq(res.bits[0], 4);
}
END_TEST

// 14.5 → 14
START_TEST(test_round_even_large_down) {
  s21_decimal d = {{145, 0, 0, 0x00010000}};  // 14.5
  s21_decimal res;

  s21_round(d, &res);

  ck_assert_uint_eq(res.bits[0], 14);
}
END_TEST

// 15.5 → 16
START_TEST(test_round_even_large_up) {
  s21_decimal d = {{155, 0, 0, 0x00010000}};  // 15.5
  s21_decimal res;

  s21_round(d, &res);

  ck_assert_uint_eq(res.bits[0], 16);
}
END_TEST

// -2.5 → -2
START_TEST(test_round_negative_even_down) {
  s21_decimal d = {{25, 0, 0, 0x80010000}};  // -2.5
  s21_decimal res;

  s21_round(d, &res);

  ck_assert_uint_eq(res.bits[0], 2);
  ck_assert_int_eq(get_sign(res.bits, DEC_BITS), 1);
}
END_TEST

// -3.5 → -4
START_TEST(test_round_negative_even_up) {
  s21_decimal d = {{35, 0, 0, 0x80010000}};  // -3.5
  s21_decimal res;

  s21_round(d, &res);

  ck_assert_uint_eq(res.bits[0], 4);
  ck_assert_int_eq(get_sign(res.bits, DEC_BITS), 1);
}
END_TEST

// 0.5 → 0
START_TEST(test_round_zero_half) {
  s21_decimal d = {{5, 0, 0, 0x00010000}};  // 0.5
  s21_decimal res;

  s21_round(d, &res);

  ck_assert_uint_eq(res.bits[0], 0);
}
END_TEST

// 1.5 → 2
START_TEST(test_round_one_half) {
  s21_decimal d = {{15, 0, 0, 0x00010000}};  // 1.5
  s21_decimal res;

  s21_round(d, &res);

  ck_assert_uint_eq(res.bits[0], 2);
}
END_TEST

// большое scale
START_TEST(test_round_large_scale) {
  s21_decimal d = {{1234567, 0, 0, 0x00040000}};  // 123.4567
  s21_decimal res;

  s21_round(d, &res);

  ck_assert_uint_eq(res.bits[0], 123);
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

  /* ---------------- Extra ---------------- */
  tcase_add_test(tc, test_truncate_large_scale);
  tcase_add_test(tc, test_truncate_integer);

  tcase_add_test(tc, test_negate_double);
  tcase_add_test(tc, test_negate_preserve_scale);

  tcase_add_test(tc, test_floor_negative_small);
  tcase_add_test(tc, test_floor_positive_small);
  tcase_add_test(tc, test_floor_negative_integer);

  tcase_add_test(tc, test_round_bank_even_down);
  tcase_add_test(tc, test_round_bank_even_up);
  tcase_add_test(tc, test_round_even_large_down);
  tcase_add_test(tc, test_round_even_large_up);
  tcase_add_test(tc, test_round_negative_even_down);
  tcase_add_test(tc, test_round_negative_even_up);
  tcase_add_test(tc, test_round_zero_half);
  tcase_add_test(tc, test_round_one_half);
  tcase_add_test(tc, test_round_large_scale);

  suite_add_tcase(s, tc);
  return s;
}