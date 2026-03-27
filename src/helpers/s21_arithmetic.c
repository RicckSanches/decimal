#include <stdint.h>

#include "../s21_decimal.h"

// -------------------- Вспомогательные функции --------------------
static inline int fits_in_decimal(uint32_t* bits) {
  for (int i = 3; i < 7; i++) {
    if (bits[i] != 0) return 0;
  }
  return 1;
}

static inline int compare_big_decimals(const s21_big_decimal* a,
                                       const s21_big_decimal* b) {
  int high = get_high_word_index(BIG_DEC_BITS);
  for (int i = high - 1; i >= 0; i--) {
    if (a->bits[i] != b->bits[i]) return (a->bits[i] > b->bits[i]) ? 1 : -1;
  }
  return 0;
}

static inline void big_add(s21_big_decimal* a, const s21_big_decimal* b) {
  uint64_t carry = 0;
  int words = get_mantissa_word_count(BIG_DEC_BITS);
  for (int i = 0; i < words; i++) {
    uint64_t sum = (uint64_t)a->bits[i] + b->bits[i] + carry;
    a->bits[i] = (uint32_t)sum;
    carry = sum >> 32;
  }
}

static inline void big_sub(s21_big_decimal* a, const s21_big_decimal* b) {
  int64_t borrow = 0;
  int words = get_mantissa_word_count(BIG_DEC_BITS);
  for (int i = 0; i < words; i++) {
    int64_t diff = (int64_t)a->bits[i] - b->bits[i] - borrow;
    if (diff < 0) {
      diff += ((int64_t)1 << 32);
      borrow = 1;
    } else
      borrow = 0;
    a->bits[i] = (uint32_t)diff;
  }
}

// Умножение big_decimal на big_decimal
static inline void big_mul(const s21_big_decimal* a, const s21_big_decimal* b,
                           s21_big_decimal* res) {
  clear_decimal_bits(res->bits, BIG_DEC_BITS, 1);
  for (int i = 0; i < 6; i++) {
    uint64_t carry = 0;
    for (int j = 0; j + i < 6; j++) {
      uint64_t cur = (uint64_t)a->bits[i] * (uint64_t)b->bits[j] +
                     res->bits[i + j] + carry;
      res->bits[i + j] = (uint32_t)cur;
      carry = cur >> 32;
    }
  }
}

static void big_div_digit(const s21_big_decimal* divisor,
                          s21_big_decimal* remainder, uint32_t* digit) {
  *digit = 0;
  s21_big_decimal tmp = {0};
  s21_big_decimal prod = {0};

  for (uint32_t i = 1; i <= 9; i++) {
    clear_decimal_bits(&tmp.bits[0], BIG_DEC_BITS, 1);
    tmp.bits[0] = i;

    clear_decimal_bits(&prod.bits[0], BIG_DEC_BITS, 1);
    big_mul(divisor, &tmp, &prod);

    if (compare_big_decimals(&prod, remainder) > 0) {
      break;  // i слишком большое, берём предыдущую цифру
    }
    *digit = i;
  }

  if (*digit > 0) {
    clear_decimal_bits(&tmp.bits[0], BIG_DEC_BITS, 1);
    tmp.bits[0] = *digit;

    clear_decimal_bits(&prod.bits[0], BIG_DEC_BITS, 1);
    big_mul(divisor, &tmp, &prod);

    big_sub(remainder, &prod);
  }
}
// -------------------- Banker's rounding --------------------

// Банковское округление big_decimal
int apply_bankers_rounding(s21_big_decimal* value, int* scale) {
  int status = S21_OK;

  while (!fits_in_decimal(value->bits)) {
    if (*scale == 0) {
      // переполнение/underflow
      return get_sign(value->bits, BIG_DEC_BITS) ? S21_TOO_SMALL : S21_TOO_BIG;
    }

    int remainder = div10(value->bits, BIG_DEC_BITS);
    (*scale)--;

    // banker's rounding
    if (should_round(remainder, 0, value->bits)) {
      add_one(value->bits, BIG_DEC_BITS);
    }
  }

  if (*scale > 28) *scale = 28;
  if (*scale < 0) *scale = 0;
  set_scale(value->bits, BIG_DEC_BITS, *scale);

  return status;
}

// -------------------- Арифметические операции --------------------
int s21_add(s21_decimal value_1, s21_decimal value_2, s21_decimal* result) {
  if (!result) return S21_TOO_BIG;

  s21_big_decimal a = {0}, b = {0}, res = {0};
  decimal_to_big_decimal(&value_1, &a);
  decimal_to_big_decimal(&value_2, &b);

  normalize_big_decimals(&a, &b);

  int sign_a = get_sign(a.bits, BIG_DEC_BITS);
  int sign_b = get_sign(b.bits, BIG_DEC_BITS);

  if (sign_a == sign_b) {
    big_add(&a, &b);
    res = a;
    set_sign(res.bits, BIG_DEC_BITS, sign_a);
  } else {
    if (compare_big_decimals(&a, &b) >= 0) {
      big_sub(&a, &b);
      res = a;
      set_sign(res.bits, BIG_DEC_BITS, sign_a);
    } else {
      big_sub(&b, &a);
      res = b;
      set_sign(res.bits, BIG_DEC_BITS, sign_b);
    }
  }

  int scale = get_scale(a.bits, BIG_DEC_BITS);
  int status = apply_bankers_rounding(&res, &scale);
  big_decimal_to_decimal(&res, result);

  return status;
}

int s21_sub(s21_decimal value_1, s21_decimal value_2, s21_decimal* result) {
  s21_decimal neg_b;
  s21_negate(value_2, &neg_b);
  return s21_add(value_1, neg_b, result);
}

int s21_mul(s21_decimal value_1, s21_decimal value_2, s21_decimal* result) {
  if (!result) return S21_TOO_BIG;

  s21_big_decimal a = {0}, b = {0}, res_big = {0};
  decimal_to_big_decimal(&value_1, &a);
  decimal_to_big_decimal(&value_2, &b);

  int result_sign =
      get_sign(a.bits, BIG_DEC_BITS) ^ get_sign(b.bits, BIG_DEC_BITS);
  int scale = get_scale(a.bits, BIG_DEC_BITS) + get_scale(b.bits, BIG_DEC_BITS);

  big_mul(&a, &b, &res_big);
  set_sign(res_big.bits, BIG_DEC_BITS, result_sign);

  int status = apply_bankers_rounding(&res_big, &scale);
  big_decimal_to_decimal(&res_big, result);

  return status;
}

int s21_div(s21_decimal value_1, s21_decimal value_2, s21_decimal* result) {
  if (!result) return S21_TOO_BIG;

  s21_big_decimal a = {0}, b = {0}, q = {0}, r = {0};
  decimal_to_big_decimal(&value_1, &a);
  decimal_to_big_decimal(&value_2, &b);

  if (is_zero(b.bits, BIG_DEC_BITS)) return S21_DIV_BY_ZERO;

  // знак результата
  int result_sign =
      get_sign(a.bits, BIG_DEC_BITS) ^ get_sign(b.bits, BIG_DEC_BITS);

  int scale_a_orig = get_scale(a.bits, BIG_DEC_BITS);
  int scale_b_orig = get_scale(b.bits, BIG_DEC_BITS);

  // нормализуем scale обоих чисел
  normalize_big_decimals(&a, &b);

  r = a;
  clear_decimal_bits(q.bits, BIG_DEC_BITS, 1);

  int frac_scale = 0;

  // --- деление "в столбик" ---
  while (!is_zero(r.bits, BIG_DEC_BITS) && fits_in_decimal(q.bits) &&
         frac_scale < 28) {
    mul10(r.bits, BIG_DEC_BITS);

    uint32_t digit = 0;
    big_div_digit(&b, &r, &digit);

    s21_big_decimal tmp = {0};
    tmp.bits[0] = digit;

    // проверяем, помещается ли в 96 бит
    s21_big_decimal q_test = q;
    mul10(q_test.bits, BIG_DEC_BITS);
    big_add(&q_test, &tmp);
    if (!fits_in_decimal(q_test.bits)) break;

    mul10(q.bits, BIG_DEC_BITS);
    big_add(&q, &tmp);

    frac_scale++;
  }

  // финальный scale = текущий frac_scale
  int final_scale = frac_scale + (scale_a_orig - scale_b_orig);
  if (final_scale > 28) final_scale = 28;

  set_sign(q.bits, BIG_DEC_BITS, result_sign);

  int status = apply_bankers_rounding(&q, &final_scale);

  big_decimal_to_decimal(&q, result);

  return status;
}