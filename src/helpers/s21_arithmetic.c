#include <stdint.h>

#include "../s21_decimal.h"

// -------------------- Арифметические операции --------------------
int s21_add(s21_decimal value_1, s21_decimal value_2, s21_decimal* result) {
  if (!result) return S21_TOO_BIG;

  s21_big_decimal a = {0}, b = {0}, res = {0};
  decimal_to_big_decimal(&value_1, &a);
  decimal_to_big_decimal(&value_2, &b);

  normalize_big_decimals(&a, &b);

  int sign_a = get_sign(a.bits, BIG_DEC_BITS);
  int sign_b = get_sign(b.bits, BIG_DEC_BITS);

  if (sign_a == sign_b) {
    big_add(&a, &b);
    res = a;
    set_sign(res.bits, BIG_DEC_BITS, sign_a);
  } else {
    if (compare_big_decimals(&a, &b) >= 0) {
      big_sub(&a, &b);
      res = a;
      set_sign(res.bits, BIG_DEC_BITS, sign_a);
    } else {
      big_sub(&b, &a);
      res = b;
      set_sign(res.bits, BIG_DEC_BITS, sign_b);
    }
  }

  int scale = get_scale(a.bits, BIG_DEC_BITS);
  int status = apply_bankers_rounding(&res, &scale);
  big_decimal_to_decimal(&res, result);

  return status;
}

int s21_sub(s21_decimal value_1, s21_decimal value_2, s21_decimal* result) {
  s21_decimal neg_b;
  s21_negate(value_2, &neg_b);
  return s21_add(value_1, neg_b, result);
}

int s21_mul(s21_decimal value_1, s21_decimal value_2, s21_decimal* result) {
  if (!result) return S21_TOO_BIG;

  s21_big_decimal a = {0}, b = {0}, res_big = {0};
  decimal_to_big_decimal(&value_1, &a);
  decimal_to_big_decimal(&value_2, &b);

  int result_sign =
      get_sign(a.bits, BIG_DEC_BITS) ^ get_sign(b.bits, BIG_DEC_BITS);
  int scale = get_scale(a.bits, BIG_DEC_BITS) + get_scale(b.bits, BIG_DEC_BITS);

  big_mul(&a, &b, &res_big);
  set_sign(res_big.bits, BIG_DEC_BITS, result_sign);

  int status = apply_bankers_rounding(&res_big, &scale);
  big_decimal_to_decimal(&res_big, result);

  return status;
}

int s21_div(s21_decimal value_1, s21_decimal value_2, s21_decimal* result) {
  if (is_zero(value_2.bits, DEC_BITS)) return S21_DIV_BY_ZERO;

  // Переводим в big_decimal
  s21_big_decimal a_big = {0}, b_big = {0}, res_big = {0};
  decimal_to_big_decimal(&value_1, &a_big);
  decimal_to_big_decimal(&value_2, &b_big);

  // Вычисляем знак результата
  int sign_res =
      get_sign(a_big.bits, BIG_DEC_BITS) ^ get_sign(b_big.bits, BIG_DEC_BITS);

  // Нормализуем scale чисел перед делением
  normalize_big_decimals(&a_big, &b_big);

  s21_big_decimal remainder = a_big;
  clear_decimal_bits(res_big.bits, BIG_DEC_BITS, 1);

  // Начальный scale результата
  int scale_a = get_scale(a_big.bits, BIG_DEC_BITS);
  int scale_b = get_scale(b_big.bits, BIG_DEC_BITS);
  int result_scale = scale_a - scale_b;
  if (result_scale < 0) result_scale = 0;

  const int MAX_SCALE = 28;

  // Основной цикл: формируем результат цифра за цифрой
  for (int i = 0; i <= MAX_SCALE; i++) {
    if (is_zero(remainder.bits, BIG_DEC_BITS)) break;

    // Делим остаток на делитель, получаем следующую цифру
    uint32_t digit = 0;
    big_div_digit(&b_big, &remainder, &digit);

    mul10(res_big.bits, BIG_DEC_BITS); 

    // Прибавляем новую цифру
    add_one(res_big.bits, BIG_DEC_BITS);  // для digit > 0
    res_big.bits[0] += digit;

    // Если остаток не ноль и scale < MAX_SCALE, готовим следующий шаг
    if (!is_zero(remainder.bits, BIG_DEC_BITS) && result_scale < MAX_SCALE) {
      mul10(remainder.bits, BIG_DEC_BITS);
      result_scale++;
    }
  }

  // Применяем банковское округление для приведения к обычному decimal
  int status = apply_bankers_rounding(&res_big, &result_scale);

  // Конвертируем обратно в decimal
  big_decimal_to_decimal(&res_big, result);
  set_sign(result->bits, DEC_BITS, sign_res);
  set_scale(result->bits, DEC_BITS, result_scale);

  return status;
}