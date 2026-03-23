#include <check.h>
#include <stdlib.h>

#include "../s21_decimal.h"

/* ============================================================================
 */
/* ============================ ТЕСТЫ ФУНКЦИЙ ================================
 */
/* ============================================================================
 */

/* -------------------- Comparisons -------------------- */
START_TEST(test_comparisons_basic) {
  s21_decimal a = {{123, 0, 0, 0}};
  s21_decimal b = {{456, 0, 0, 0}};
  s21_decimal c = {{123, 0, 0, 0}};
  s21_decimal neg_a = {{123, 0, 0, 0}};
  set_sign(neg_a.bits, DEC_BITS, 1);

  // s21_is_less
  ck_assert_int_eq(s21_is_less(a, b), 1);
  ck_assert_int_eq(s21_is_less(b, a), 0);
  ck_assert_int_eq(s21_is_less(a, c), 0);

  // s21_is_less_or_equal
  ck_assert_int_eq(s21_is_less_or_equal(a, b), 1);
  ck_assert_int_eq(s21_is_less_or_equal(a, c), 1);
  ck_assert_int_eq(s21_is_less_or_equal(b, a), 0);

  // s21_is_greater
  ck_assert_int_eq(s21_is_greater(b, a), 1);
  ck_assert_int_eq(s21_is_greater(a, b), 0);
  ck_assert_int_eq(s21_is_greater(a, c), 0);

  // s21_is_greater_or_equal
  ck_assert_int_eq(s21_is_greater_or_equal(b, a), 1);
  ck_assert_int_eq(s21_is_greater_or_equal(a, c), 1);
  ck_assert_int_eq(s21_is_greater_or_equal(a, b), 0);

  // s21_is_equal
  ck_assert_int_eq(s21_is_equal(a, c), 1);
  ck_assert_int_eq(s21_is_equal(a, b), 0);

  // s21_is_not_equal
  ck_assert_int_eq(s21_is_not_equal(a, b), 1);
  ck_assert_int_eq(s21_is_not_equal(a, c), 0);

  // сравнение с отрицательным числом
  ck_assert_int_eq(s21_is_less(neg_a, a), 1);
  ck_assert_int_eq(s21_is_greater(a, neg_a), 1);
}
END_TEST

START_TEST(test_comparisons_with_zero) {
  s21_decimal zero = {{0, 0, 0, 0}};
  s21_decimal pos = {{123, 0, 0, 0}};
  s21_decimal neg = {{123, 0, 0, 0}};
  set_sign(neg.bits, DEC_BITS, 1);

  // ноль равен нулю
  ck_assert_int_eq(s21_is_equal(zero, zero), 1);
  ck_assert_int_eq(s21_is_not_equal(zero, zero), 0);

  // ноль меньше положительного
  ck_assert_int_eq(s21_is_less(zero, pos), 1);
  ck_assert_int_eq(s21_is_greater(pos, zero), 1);

  // ноль больше отрицательного
  ck_assert_int_eq(s21_is_greater(zero, neg), 1);
  ck_assert_int_eq(s21_is_less(neg, zero), 1);
}
END_TEST

START_TEST(test_comparisons_with_scale) {
  s21_decimal a = {{123, 0, 0, 0}};   // scale 2
  s21_decimal b = {{1230, 0, 0, 0}};  // scale 3
  set_scale(a.bits, DEC_BITS, 2);
  set_scale(b.bits, DEC_BITS, 3);

  // после нормализации они должны быть равны
  ck_assert_int_eq(s21_is_equal(a, b), 1);
  ck_assert_int_eq(s21_is_less(a, b), 0);
  ck_assert_int_eq(s21_is_greater(a, b), 0);

  // меняем знак b
  set_sign(b.bits, DEC_BITS, 1);
  ck_assert_int_eq(s21_is_less(b, a), 1);
  ck_assert_int_eq(s21_is_greater(a, b), 1);
}
END_TEST

START_TEST(test_comparisons_large_numbers) {
  s21_decimal a = {{0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0}};
  s21_decimal b = {{0xFFFFFFFE, 0xFFFFFFFF, 0xFFFFFFFF, 0}};

  // a > b
  ck_assert_int_eq(s21_is_greater(a, b), 1);
  ck_assert_int_eq(s21_is_less(b, a), 1);
  ck_assert_int_eq(s21_is_equal(a, b), 0);
}
END_TEST

START_TEST(test_compare_negatives) {
    s21_decimal a = {{5, 0, 0, 0}};
    s21_decimal b = {{3, 0, 0, 0}};
    set_sign(a.bits, DEC_BITS, 1);
    set_sign(b.bits, DEC_BITS, 1);

    ck_assert_int_eq(s21_is_less(a, b), 1);    // -5 < -3
    ck_assert_int_eq(s21_is_greater(a, b), 0);
    ck_assert_int_eq(s21_is_less(b, a), 0);    // -3 < -5 -> false
    ck_assert_int_eq(s21_is_greater(b, a), 1); // -3 > -5 -> true
}
END_TEST

/* -------------------- Comparisons: additional edge cases -------------------- */
START_TEST(test_comparisons_zero_sign) {
  s21_decimal pos_zero = {{0, 0, 0, 0}};
  s21_decimal neg_zero = {{0, 0, 0, 0}};
  set_sign(neg_zero.bits, DEC_BITS, 1);

  // +0 == -0
  ck_assert_int_eq(s21_is_equal(pos_zero, neg_zero), 1);
  ck_assert_int_eq(s21_is_not_equal(pos_zero, neg_zero), 0);

  // < и > должны быть false
  ck_assert_int_eq(s21_is_less(pos_zero, neg_zero), 0);
  ck_assert_int_eq(s21_is_greater(pos_zero, neg_zero), 0);
}
END_TEST

START_TEST(test_comparisons_mixed_signs_scale) {
  s21_decimal a = {{123, 0, 0, 0}};   // +1.23 scale=2
  s21_decimal b = {{1230, 0, 0, 0}};  // -12.30 scale=2
  set_sign(b.bits, DEC_BITS, 1);      // b отрицательное
  set_scale(a.bits, DEC_BITS, 2);
  set_scale(b.bits, DEC_BITS, 2);

  // +a > -b
  ck_assert_int_eq(s21_is_greater(a, b), 1);
  ck_assert_int_eq(s21_is_less(b, a), 1);

  // == должно быть false
  ck_assert_int_eq(s21_is_equal(a, b), 0);
}

START_TEST(test_comparisons_large_negative_numbers) {
  s21_decimal a = {{0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0}};  // большое число
  s21_decimal b = {{0xFFFFFFFE, 0xFFFFFFFF, 0xFFFFFFFF, 0}};  // чуть меньше
  set_sign(a.bits, DEC_BITS, 1);  // a отрицательное
  set_sign(b.bits, DEC_BITS, 1);  // b отрицательное

  // -a < -b → a меньше, т.к. |a| > |b|
  ck_assert_int_eq(s21_is_less(a, b), 1);
  ck_assert_int_eq(s21_is_greater(a, b), 0);
  ck_assert_int_eq(s21_is_equal(a, b), 0);
}

START_TEST(test_comparisons_equal_different_scale) {
  s21_decimal a = {{123, 0, 0, 0}};   // 1.23 scale=2
  s21_decimal b = {{1230, 0, 0, 0}};  // 1.230 scale=3
  set_scale(a.bits, DEC_BITS, 2);
  set_scale(b.bits, DEC_BITS, 3);

  // после нормализации они должны быть равны
  ck_assert_int_eq(s21_is_equal(a, b), 1);
  ck_assert_int_eq(s21_is_less(a, b), 0);
  ck_assert_int_eq(s21_is_greater(a, b), 0);
}
END_TEST

Suite* comparisons_suite(void) {
  Suite* s = suite_create("Decimal Comparisons");
  TCase* tc = tcase_create("Core");

  tcase_add_test(tc, test_comparisons_basic);
  tcase_add_test(tc, test_comparisons_with_zero);
  tcase_add_test(tc, test_comparisons_with_scale);
  tcase_add_test(tc, test_comparisons_large_numbers);
  tcase_add_test(tc, test_compare_negatives);
  
  tcase_add_test(tc, test_comparisons_zero_sign);
  tcase_add_test(tc, test_comparisons_mixed_signs_scale);
  tcase_add_test(tc, test_comparisons_large_negative_numbers);
  tcase_add_test(tc, test_comparisons_equal_different_scale);

  suite_add_tcase(s, tc);
  return s;
}