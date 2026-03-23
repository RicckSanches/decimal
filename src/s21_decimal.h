#ifndef S21_DECIMAL_H
#define S21_DECIMAL_H

#include <limits.h>
#include <stdint.h>

#define DEC_BITS 128
#define BIG_DEC_BITS 256

typedef struct {
  uint32_t bits[4];
} s21_decimal;

typedef struct {
  uint32_t bits[8];  //  256 бита
} s21_big_decimal;

// Коды ошибок
#define S21_OK 0       // успех
#define S21_TOO_BIG 1  // число слишком велико или равно бесконечности
#define S21_TOO_SMALL \
  2  // число слишком мало или равно отрицательной бесконечности
#define S21_DIV_BY_ZERO 3  // деление на 0

static inline int get_high_word_index(int size_in_bits) {
  return size_in_bits / 32 - 1;
}

// Арифметические операторы (4 функции)
int s21_add(s21_decimal value_1, s21_decimal value_2, s21_decimal* result);
int s21_sub(s21_decimal value_1, s21_decimal value_2, s21_decimal* result);
int s21_mul(s21_decimal value_1, s21_decimal value_2, s21_decimal* result);
int s21_div(s21_decimal value_1, s21_decimal value_2, s21_decimal* result);

// Операторы сравнения (6 функций)
int s21_is_less(s21_decimal a, s21_decimal b);
int s21_is_less_or_equal(s21_decimal a, s21_decimal b);
int s21_is_greater(s21_decimal a, s21_decimal b);
int s21_is_greater_or_equal(s21_decimal a, s21_decimal b);
int s21_is_equal(s21_decimal a, s21_decimal b);
int s21_is_not_equal(s21_decimal a, s21_decimal b);

// Преобразователи (4 функции)
int s21_from_int_to_decimal(int src, s21_decimal* dst);
int s21_from_float_to_decimal(float src, s21_decimal* dst);
int s21_from_decimal_to_int(s21_decimal src, int* dst);
int s21_from_decimal_to_float(s21_decimal src, float* dst);

// Другие функции (4 функции)
int s21_floor(s21_decimal value, s21_decimal* result);
int s21_round(s21_decimal value, s21_decimal* result);
int s21_truncate(s21_decimal value, s21_decimal* result);
int s21_negate(s21_decimal value, s21_decimal* result);

// decimal_helpers
int get_bit(const uint32_t* bits, int size_in_bits, int index);
void set_bit(uint32_t* bits, int size_in_bits, int index, int value);
int get_sign(const uint32_t* bits, int size_in_bits);
void set_sign(uint32_t* bits, int size_in_bits, int sign);
int get_scale(const uint32_t* bits, int size_in_bits);
void set_scale(uint32_t* bits, int size_in_bits, int scale);
void clear_decimal_bits(uint32_t* bits, int size_in_bits, int clear_metadata);
int div10(uint32_t* bits, int size_in_bits);
int mul10(uint32_t* bits, int size_in_bits);
int should_round(int last_rem, int has_tail, const uint32_t* bits);
void add_one(uint32_t* bits, int size_in_bits);

// arithmetic_helpers
void decimal_to_big_decimal(s21_decimal* dec, s21_big_decimal* big);
int is_zero(const uint32_t* bits, int size_in_bits);
void normalize_big_decimals(s21_big_decimal* a, s21_big_decimal* b);
int big_decimal_to_decimal(s21_big_decimal* big, s21_decimal* dec);

#endif