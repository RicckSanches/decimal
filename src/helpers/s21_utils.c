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

    // Сохраняем оригинальное значение младшего слова
    uint32_t orig_lo = value.bits[0];

    // Отбрасываем дробную часть
    while (scale > 0) {
      div10(result->bits, DEC_BITS);
      scale--;
    }
    set_scale(result->bits, DEC_BITS, 0);

    // Если отрицательное и была дробная часть, добавляем +1
    if (sign && orig_lo != result->bits[0]) {
      uint64_t carry = 1;
      for (int i = 0; i < 3 && carry; i++) {
        uint64_t sum = (uint64_t)result->bits[i] + carry;
        result->bits[i] = (uint32_t)sum;
        carry = sum >> 32;
      }
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

    // Отбрасываем дробную часть, запоминая остаток
    while (scale > 0) {
      last_rem = div10(result->bits, DEC_BITS);
      scale--;
    }

    // Если остаток ≥5, округляем вверх
    if (last_rem >= 5) {
      uint64_t carry = 1;
      for (int i = 0; i < 3 && carry; i++) {
        uint64_t sum = (uint64_t)result->bits[i] + carry;
        result->bits[i] = (uint32_t)sum;
        carry = sum >> 32;
      }
    }

    set_scale(result->bits, DEC_BITS, 0);
    err = 0;
  }

  return err;
}