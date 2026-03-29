#include "../s21_decimal.h"

// ---------------------- Truncate ----------------------
int s21_truncate(s21_decimal value, s21_decimal* result) {
  int err = 1;  // по умолчанию ошибка

  if (result) {
    *result = value;
    int scale = get_scale(value.bits, DEC_BITS);
    while (scale > 0) {
      div10(result->bits, DEC_BITS);
      scale--;
    }
    set_scale(result->bits, DEC_BITS, 0);
    err = 0;
  }

  return err;
}

// ---------------------- Negate ----------------------
int s21_negate(s21_decimal value, s21_decimal* result) {
  int err = 1;

  if (result) {
    *result = value;
    int sign = get_sign(value.bits, DEC_BITS);
    set_sign(result->bits, DEC_BITS, !sign);
    err = 0;
  }

  return err;
}

// ---------------------- Floor ----------------------
int s21_floor(s21_decimal value, s21_decimal* result) {
  int err = 1;

  if (result) {
    *result = value;
    int scale = get_scale(value.bits, DEC_BITS);
    int sign = get_sign(value.bits, DEC_BITS);

    int has_fraction = 0;
    s21_decimal temp = value;

    // Проверяем, есть ли дробная часть
    int temp_scale = scale;
    while (temp_scale > 0) {
      int rem = div10(temp.bits, DEC_BITS);
      if (rem != 0) has_fraction = 1;
      temp_scale--;
    }

    // Отбрасываем дробную часть в result
    while (scale > 0) {
      div10(result->bits, DEC_BITS);
      scale--;
    }
    set_scale(result->bits, DEC_BITS, 0);

    // Для отрицательных чисел с дробной частью делаем floor (используем
    // универсальный add_one)
    if (sign && has_fraction) {
      add_one(result->bits, DEC_BITS);  // 3 слова для s21_decimal
    }

    err = 0;
  }

  return err;
}

// ---------------------- Round ----------------------
int s21_round(s21_decimal value, s21_decimal* result) {
  int err = 1;

  if (result) {
    *result = value;

    int scale = get_scale(value.bits, DEC_BITS);
    int last_rem = 0;
    int has_tail = 0;

    while (scale > 0) {
      int rem = div10(result->bits, DEC_BITS);

      if (scale == 1)
        last_rem = rem;
      else if (rem != 0)
        has_tail = 1;

      scale--;
    }

    if (should_round(last_rem, has_tail, result->bits)) {
      add_one(result->bits, DEC_BITS);
    }

    set_scale(result->bits, DEC_BITS, 0);
    err = 0;
  }

  return err;
}