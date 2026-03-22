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

int s21_from_float_to_decimal(float src, s21_decimal* dst) { return 0; }

int s21_from_decimal_to_int(s21_decimal src, int* dst) { return 0; }

int s21_from_decimal_to_float(s21_decimal src, float* dst) { return 0; }