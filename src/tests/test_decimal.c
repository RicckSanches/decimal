#include <check.h>

#include "../s21_decimal.h"

// -------------------- Интеграционный тест: int/float → арифметика → округление
// → обратно --------------------
START_TEST(test_full_cycle_int_float) {
  s21_decimal a, b, res, rounded;
  int int_val;
  float float_val;

  // Конвертация из int
  s21_from_int_to_decimal(123, &a);  // a = 123

  // Конвертация из float
  s21_from_float_to_decimal(-45.67f, &b);  // b = -45.67

  // Арифметическая операция
  s21_add(a, b, &res);  // 123 + (-45.67) = 77.33
  ck_assert_int_eq(get_sign(res.bits, DEC_BITS), 0);
  ck_assert_int_eq(get_scale(res.bits, DEC_BITS), 2);

  // Банковское округление
  s21_round(res, &rounded);  // 77.33 → 77
  ck_assert_int_eq(get_scale(rounded.bits, DEC_BITS), 0);
  ck_assert_int_eq(rounded.bits[0], 77);

  // Обратная конвертация в int
  s21_from_decimal_to_int(rounded, &int_val);
  ck_assert_int_eq(int_val, 77);

  // Обратная конвертация в float
  s21_from_decimal_to_float(res, &float_val);
  float diff = float_val - 77.33f;
  if (diff < 0) diff = -diff;
  ck_assert(diff < 0.01f);  // допускаем погрешность 0.01
}
END_TEST

Suite* integrated_conversion_suite(void) {
  Suite* s = suite_create("Full Decimal Conversion and Arithmetic");
  TCase* tc_core = tcase_create("Core");

  tcase_add_test(tc_core, test_full_cycle_int_float);

  suite_add_tcase(s, tc_core);
  return s;
}