#ifndef TEST_HELPERS_H
#define TEST_HELPERS_H

#include <check.h>

#include "../s21_decimal.h"

// Сравнение мантиссы произвольного размера
static inline int bits_equal(const uint32_t* a, const uint32_t* b,
                             int size_in_bits) {
  int equal = 1;  // считаем, что равны
  int words = get_mantissa_word_count(size_in_bits);
  for (int i = 0; i < words && equal; i++) {
    if (a[i] != b[i]) {
      equal = 0;
    }
  }
  return equal;
}

// Проверка decimal или big_decimal: bits, scale и sign
static inline void assert_number(const uint32_t* res_bits,
                                 const uint32_t* expected_bits,
                                 int size_in_bits, int expected_scale,
                                 int expected_sign) {
  ck_assert_msg(bits_equal(res_bits, expected_bits, size_in_bits),
                "Bits mismatch for %d-bit number", size_in_bits);
  ck_assert_int_eq(get_scale(res_bits, size_in_bits), expected_scale);
  ck_assert_int_eq(get_sign(res_bits, size_in_bits), expected_sign);
}

// Проверка float с точностью eps
static inline int float_eq(float a, float b, float eps) {
  float diff = a > b ? a - b : b - a;
  return diff < eps;
}

#endif  // TEST_HELPERS_H