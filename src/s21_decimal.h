#ifndef S21_DECIMAL_H
#define S21_DECIMAL_H

#include <stdint.h>

typedef struct {
    uint32_t bits[4];
} s21_decimal;

typedef struct {
    uint32_t bits[6]; // 192 бита
} s21_big_decimal;

// Коды ошибок
#define S21_OK 0      // успех
#define S21_TOO_BIG 1 // число слишком велико или равно бесконечности
#define S21_TOO_SMALL 2 // число слишком мало или равно отрицательной бесконечности
#define S21_DIV_BY_ZERO 3 // деление на 0

// Арифметические операторы (4 функции)
int s21_add(s21_decimal value_1, s21_decimal value_2, s21_decimal *result);
int s21_sub(s21_decimal value_1, s21_decimal value_2, s21_decimal *result);
int s21_mul(s21_decimal value_1, s21_decimal value_2, s21_decimal *result);
int s21_div(s21_decimal value_1, s21_decimal value_2, s21_decimal *result);

// Операторы сравнения (6 функций)
int s21_is_less(s21_decimal a, s21_decimal b);
int s21_is_less_or_equal(s21_decimal a, s21_decimal b);
int s21_is_greater(s21_decimal a, s21_decimal b);
int s21_is_greater_or_equal(s21_decimal a, s21_decimal b);
int s21_is_equal(s21_decimal a, s21_decimal b);
int s21_is_not_equal(s21_decimal a, s21_decimal b);

// Преобразователи (4 функции)
int s21_from_int_to_decimal(int src, s21_decimal *dst);
int s21_from_float_to_decimal(float src, s21_decimal *dst);
int s21_from_decimal_to_int(s21_decimal src, int *dst);
int s21_from_decimal_to_float(s21_decimal src, float *dst);

// Другие функции (4 функции)
int s21_floor(s21_decimal value, s21_decimal *result);
int s21_round(s21_decimal value, s21_decimal *result);
int s21_truncate(s21_decimal value, s21_decimal *result);
int s21_negate(s21_decimal value, s21_decimal *result);

//helpers
int get_bit(s21_decimal d, int index);
void set_bit(s21_decimal *d, int index, int value);
int get_sign(s21_decimal d);
void set_sign(s21_decimal *d, int sign);
int get_scale(s21_decimal d);
void set_scale(s21_decimal *d, int scale);
void clear_decimal(s21_decimal *d);

#endif