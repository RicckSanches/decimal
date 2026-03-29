#include <check.h>
#include <stdio.h>
#include "../s21_decimal.h"

// Вспомогательная функция для сравнения int с decimal
void assert_decimal_to_int(int expected, s21_decimal dec) {
    int actual;
    int res = s21_from_decimal_to_int(dec, &actual);
    ck_assert_int_eq(res, 0);
    ck_assert_int_eq(actual, expected);
}

// Вспомогательная функция для сравнения float с decimal
void assert_decimal_to_float(float expected, s21_decimal dec) {
    float actual;
    int res = s21_from_decimal_to_float(dec, &actual);
    ck_assert_int_eq(res, 0);
    // Сравниваем с небольшой точностью
    
    ck_assert_double_eq_tol(actual, expected, 1e-5);
}

// -------------------- Интеграционный тест --------------------
START_TEST(test_full_cycle_int_float_arithmetic) {
    s21_decimal dec_a, dec_b, result;
    int res;

    int a = -123;
    int b = 7;

    // ---------------- Преобразуем int → decimal ----------------
    res = s21_from_int_to_decimal(a, &dec_a);
    ck_assert_int_eq(res, 0);
    res = s21_from_int_to_decimal(b, &dec_b);
    ck_assert_int_eq(res, 0);

    // ---------------- Сложение ----------------
    s21_add(dec_a, dec_b, &result);
    assert_decimal_to_int(a + b, result);

    // ---------------- Вычитание ----------------
    s21_sub(dec_a, dec_b, &result);
    assert_decimal_to_int(a - b, result);

    // ---------------- Умножение ----------------
    s21_mul(dec_a, dec_b, &result);
    assert_decimal_to_int(a * b, result);

    // ---------------- Деление → float ----------------
    s21_div(dec_a, dec_b, &result);
    printf("res: %u, scale: %d, sign: %d\n", result.bits[0],
         get_scale(result.bits, DEC_BITS), get_sign(result.bits, DEC_BITS));
    float expected_div = (float)a / (float)b;
    assert_decimal_to_float(expected_div, result);
}
END_TEST

// -------------------- Suite --------------------
Suite* integrated_conversion_suite(void) {
    Suite* s = suite_create("Full Decimal Conversion and Arithmetic");
    TCase* tc_core = tcase_create("Core");

    tcase_add_test(tc_core, test_full_cycle_int_float_arithmetic);

    suite_add_tcase(s, tc_core);
    return s;
}