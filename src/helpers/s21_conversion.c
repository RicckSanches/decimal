#include "../s21_decimal.h"

int s21_from_int_to_decimal(int src, s21_decimal* dst) {
  int err = 0;
  if (!dst) {
    err = 1;
  } else {
    clear_decimal_bits(dst->bits, DEC_BITS, 1);
    uint32_t value;
    if (src < 0) {
      set_sign(dst->bits, DEC_BITS, 1);
      value = (uint32_t)(-(long long)src);
    } else {
      set_sign(dst->bits, DEC_BITS, 0);
      value = (uint32_t)src;
    }
    dst->bits[0] = value;
    set_scale(dst->bits, DEC_BITS, 0);
  }
  return err;
}

int s21_from_float_to_decimal(float src, s21_decimal* dst) {
  if (!dst) return 1;  // проверка на NULL

  clear_decimal_bits(dst->bits, DEC_BITS, 1);

  if (src == 0.0f) return 0;  // ноль сразу

  int sign = 0;
  if (src < 0.0f) {
    sign = 1;
    src = -src;
  }

  int scale = 0;

  // Умножаем на 10, пока есть дробная часть и scale < 28
  while (src != (float)(uint32_t)src && scale < 28) {
    src *= 10.0f;
    scale++;
  }

  // Проверка переполнения uint32_t
  if (src > 4294967295.0f) return 1;

  dst->bits[0] = (uint32_t)src;
  set_sign(dst->bits, DEC_BITS, sign);
  set_scale(dst->bits, DEC_BITS, scale);

  return 0;
}

int s21_from_decimal_to_int(s21_decimal src, int* dst) {
  int err = 0;  // по умолчанию успех

  if (!dst) {
    err = 1;  // ошибка: null pointer
  } else {
    s21_decimal temp;
    s21_truncate(src, &temp);

    int sign = get_sign(temp.bits, DEC_BITS);
    uint32_t val = temp.bits[0];

    if (val > (uint32_t)INT32_MAX) {
      err = 1;  // переполнение
    } else {
      *dst = sign ? -(int)val : (int)val;
    }
  }

  return err;
}

int s21_from_decimal_to_float(s21_decimal src, float* dst) {
  int err = 0;  // по умолчанию успех

  if (!dst) {
    err = 1;  // ошибка: null pointer
  } else {
    float result = 0.0f;
    for (int i = 2; i >= 0; i--) {  // 3 слова
      result *= 4294967296.0f;      // 2^32
      result += src.bits[i];
    }

    int scale = get_scale(src.bits, DEC_BITS);
    while (scale > 0) {
      result /= 10.0f;
      scale--;
    }

    if (get_sign(src.bits, DEC_BITS)) result = -result;
    *dst = result;
  }

  return err;
}