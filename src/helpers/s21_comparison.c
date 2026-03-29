#include "../s21_decimal.h"

static int compare_same_sign_decimals(s21_decimal* a, s21_decimal* b,
                                      int sign) {
  s21_big_decimal big_a, big_b;
  decimal_to_big_decimal(a, &big_a);
  decimal_to_big_decimal(b, &big_b);
  normalize_big_decimals(&big_a, &big_b);

  int result = 0;
  int high_index = get_high_word_index(BIG_DEC_BITS);
  for (int i = high_index - 1; i >= 0 && result == 0; i--) {
    if (big_a.bits[i] != big_b.bits[i]) {
      if (sign == 0)
        result = (big_a.bits[i] < big_b.bits[i]) ? -1 : 1;
      else
        result = (big_a.bits[i] > big_b.bits[i]) ? -1 : 1;
    }
  }

  return result;
}

static int compare_decimals(s21_decimal* a, s21_decimal* b) {
  int result = 0;

  int a_is_zero = is_zero(a->bits, DEC_BITS);
  int b_is_zero = is_zero(b->bits, DEC_BITS);

  int sign_a = get_sign(a->bits, DEC_BITS);
  int sign_b = get_sign(b->bits, DEC_BITS);

  if (!(a_is_zero && b_is_zero)) {
    if (sign_a != sign_b) {
      result = sign_a ? -1 : 1;
    } else {
      result = compare_same_sign_decimals(a, b, sign_a);
    }
  }

  return result;
}

int s21_is_less(s21_decimal a, s21_decimal b) {
  return compare_decimals(&a, &b) == -1;
}

int s21_is_less_or_equal(s21_decimal a, s21_decimal b) {
  int cmp = compare_decimals(&a, &b);
  return cmp == -1 || cmp == 0;
}

int s21_is_greater(s21_decimal a, s21_decimal b) {
  return compare_decimals(&a, &b) == 1;
}

int s21_is_greater_or_equal(s21_decimal a, s21_decimal b) {
  int cmp = compare_decimals(&a, &b);
  return cmp == 1 || cmp == 0;
}

int s21_is_equal(s21_decimal a, s21_decimal b) {
  return compare_decimals(&a, &b) == 0;
}

int s21_is_not_equal(s21_decimal a, s21_decimal b) {
  return compare_decimals(&a, &b) != 0;
}