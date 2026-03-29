
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
  int status = 0;

  if (!dst) {
    status = 1;
  } else {
    clear_decimal_bits(dst->bits, DEC_BITS, 1);

    float abs_value = (src < 0) ? -src : src;

    if ((abs_value > 0.0f && abs_value < 1e-28f) ||
        abs_value > 79228162514264337593543950335.0f || src != src ||
        (src * 0 != 0)) {
      status = 1;
    } else {
      int sign = (src < 0) ? 1 : 0;
      float value = abs_value;

      int scale = 0;
      while (value != 0.0f && value < 10000000.0f && scale < 28) {
        value *= 10.0f;
        scale++;
      }

      int int_part = (int)value;
      float frac = value - int_part;
      if (frac > 0.5f || (frac == 0.5f && (int_part & 1))) {
        int_part++;
      }

      dst->bits[0] = int_part;
      dst->bits[1] = dst->bits[2] = 0;

      set_scale(dst->bits, DEC_BITS, scale);
      set_sign(dst->bits, DEC_BITS, sign);
    }
  }

  return status;
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