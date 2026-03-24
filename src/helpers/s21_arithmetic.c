#include "../s21_decimal.h"
#include <stdint.h>

// -------------------- Прототипы big-операций --------------------
void big_mul(const s21_big_decimal* a, const s21_big_decimal* b, s21_big_decimal* res);
void big_div(const s21_big_decimal* a, const s21_big_decimal* b, s21_big_decimal* res);
int is_zero_big(const s21_big_decimal* a);

// -------------------- Вспомогательные функции --------------------
static inline int fits_in_decimal(uint32_t* bits) {
  int result = 1;
  for (int i = 3; i < 7 && result; i++) {
    if (bits[i] != 0) result = 0;
  }
  return result;
}

static inline int compare_big_decimals(s21_big_decimal* a, s21_big_decimal* b) {
  int result = 0;
  int high = get_high_word_index(BIG_DEC_BITS);
  for (int i = high - 1; i >= 0 && result == 0; i--) {
    if (a->bits[i] != b->bits[i]) {
      result = (a->bits[i] > b->bits[i]) ? 1 : -1;
    }
  }
  return result;
}

static inline int apply_bankers_rounding(s21_big_decimal* value, int* scale) {
  int status = 0;
  int last_rem = 0;
  int has_tail = 0;
  while (!fits_in_decimal(value->bits) && *scale > 0) {
    int rem = div10(value->bits, BIG_DEC_BITS);
    if (*scale == 1) last_rem = rem;
    else if (rem != 0) has_tail = 1;
    (*scale)--;
  }

  if (!fits_in_decimal(value->bits)) {
    status = get_sign(value->bits, BIG_DEC_BITS) ? 2 : 1;
    return status;
  }

  if (last_rem != 0 || has_tail != 0) {
    if (should_round(last_rem, has_tail, value->bits)) {
      add_one(value->bits, DEC_BITS);
      if (!fits_in_decimal(value->bits)) {
        status = get_sign(value->bits, BIG_DEC_BITS) ? 2 : 1;
      }
    }
  }

  set_scale(value->bits, BIG_DEC_BITS, *scale);
  return status;
}

// -------------------- Арифметические операции --------------------
int s21_add(s21_decimal value_1, s21_decimal value_2, s21_decimal* result) {
  int status = 0;
  s21_big_decimal a, b, res_big;
  decimal_to_big_decimal(&value_1, &a);
  decimal_to_big_decimal(&value_2, &b);

  normalize_big_decimals(&a, &b);

  int sign_a = get_sign(a.bits, BIG_DEC_BITS);
  int sign_b = get_sign(b.bits, BIG_DEC_BITS);

  clear_decimal_bits(res_big.bits, BIG_DEC_BITS, 1);

  if (sign_a == sign_b) {
    uint64_t carry = 0;
    for (int i = 0; i < BIG_DEC_BITS / 32; i++) {
      uint64_t sum = (uint64_t)a.bits[i] + b.bits[i] + carry;
      res_big.bits[i] = (uint32_t)sum;
      carry = sum >> 32;
    }
    set_sign(res_big.bits, BIG_DEC_BITS, sign_a);
  } else {
    int cmp = compare_big_decimals(&a, &b);
    s21_big_decimal* bigger = cmp >= 0 ? &a : &b;
    s21_big_decimal* smaller = cmp >= 0 ? &b : &a;

    int result_sign = (cmp == 0) ? 0 : ((cmp > 0) ? sign_a : 1 - sign_a);

    int borrow = 0;
    for (int i = 0; i < BIG_DEC_BITS / 32; i++) {
      int64_t diff = (int64_t)bigger->bits[i] - smaller->bits[i] - borrow;
      if (diff < 0) {
        diff += ((int64_t)1 << 32);
        borrow = 1;
      } else {
        borrow = 0;
      }
      res_big.bits[i] = (uint32_t)diff;
    }
    set_sign(res_big.bits, BIG_DEC_BITS, result_sign);
  }

  int scale = get_scale(a.bits, BIG_DEC_BITS);
  status = apply_bankers_rounding(&res_big, &scale);
  big_decimal_to_decimal(&res_big, result);

  return status;
}

int s21_sub(s21_decimal value_1, s21_decimal value_2, s21_decimal* result) {
  s21_decimal neg_b;
  s21_negate(value_2, &neg_b);
  return s21_add(value_1, neg_b, result);
}

// -------------------- Умножение --------------------
int s21_mul(s21_decimal value_1, s21_decimal value_2, s21_decimal* result) {
  s21_big_decimal a, b, res_big;
  decimal_to_big_decimal(&value_1, &a);
  decimal_to_big_decimal(&value_2, &b);

  clear_decimal_bits(res_big.bits, BIG_DEC_BITS, 1);

  int sign_a = get_sign(a.bits, BIG_DEC_BITS);
  int sign_b = get_sign(b.bits, BIG_DEC_BITS);
  int result_sign = sign_a ^ sign_b;

  int scale = get_scale(a.bits, BIG_DEC_BITS) + get_scale(b.bits, BIG_DEC_BITS);

  big_mul(&a, &b, &res_big);

  set_sign(res_big.bits, BIG_DEC_BITS, result_sign);
  apply_bankers_rounding(&res_big, &scale);
  big_decimal_to_decimal(&res_big, result);

  return 0;
}

// -------------------- Деление --------------------
int s21_div(s21_decimal value_1, s21_decimal value_2, s21_decimal* result) {
  s21_big_decimal a = {0}, b = {0}, res_big = {0};
  decimal_to_big_decimal(&value_1, &a);
  decimal_to_big_decimal(&value_2, &b);

  if (is_zero_big(&b)) return 3;

  clear_decimal_bits(res_big.bits, BIG_DEC_BITS, 1);

  int sign_a = get_sign(a.bits, BIG_DEC_BITS);
  int sign_b = get_sign(b.bits, BIG_DEC_BITS);
  int result_sign = sign_a ^ sign_b;

  int scale_a = get_scale(a.bits, BIG_DEC_BITS);
  int scale_b = get_scale(b.bits, BIG_DEC_BITS);
  int res_scale = scale_a - scale_b;
  if (res_scale < 0) res_scale = 0;

  big_div(&a, &b, &res_big);

  set_sign(res_big.bits, BIG_DEC_BITS, result_sign);
  apply_bankers_rounding(&res_big, &res_scale);
  big_decimal_to_decimal(&res_big, result);

  return 0;
}

// -------------------- Вспомогательные big-функции --------------------
int is_zero_big(const s21_big_decimal* a) {
  for (int i = 0; i < 6; i++)
    if (a->bits[i] != 0) return 0;
  return 1;
}

void big_mul(const s21_big_decimal* a, const s21_big_decimal* b, s21_big_decimal* res) {
  clear_decimal_bits(res->bits, BIG_DEC_BITS, 1);
  for (int i = 0; i < 6; i++) {
    uint64_t carry = 0;
    for (int j = 0; j + i < 6; j++) {
      uint64_t cur = (uint64_t)a->bits[i] * (uint64_t)b->bits[j] + res->bits[i + j] + carry;
      res->bits[i + j] = (uint32_t)cur;
      carry = cur >> 32;
    }
  }
}

void big_div(const s21_big_decimal* a, const s21_big_decimal* b, s21_big_decimal* res) {
  clear_decimal_bits(res->bits, BIG_DEC_BITS, 1);
  s21_big_decimal dividend = *a;
  if (is_zero_big(b)) return;

  s21_big_decimal one = {0};
  one.bits[0] = 1;

  while (compare_big_decimals(&dividend, (s21_big_decimal*)b) >= 0) {
    s21_big_decimal tmp = *b;
    int borrow = 0;
    for (int i = 0; i < 6; i++) {
      int64_t diff = (int64_t)dividend.bits[i] - tmp.bits[i] - borrow;
      if (diff < 0) {
        diff += ((int64_t)1 << 32);
        borrow = 1;
      } else borrow = 0;
      dividend.bits[i] = (uint32_t)diff;
    }

    uint64_t carry = 0;
    for (int i = 0; i < 6; i++) {
      uint64_t sum = (uint64_t)res->bits[i] + one.bits[i] + carry;
      res->bits[i] = (uint32_t)sum;
      carry = sum >> 32;
    }
  }
}