#include "../s21_decimal.h"

// Проверяем, помещается ли мантисса в 96 бит
int mantissa_fits_96(const s21_big_decimal* big) {
  for (int i = 3; i < 7; i++) {
    if (big->bits[i] != 0) return 0;
  }
  return 1;
}

void big_decimal_multiply_by_10(s21_big_decimal* big) {
  uint64_t carry = 0;

  for (int i = 0; i < 7; i++) {
    uint64_t tmp = (uint64_t)big->bits[i] * 10 + carry;
    big->bits[i] = (uint32_t)(tmp & 0xFFFFFFFFu);
    carry = tmp >> 32;
  }
}

int big_decimal_divide_by_10(s21_big_decimal* big) {
  uint64_t remainder = 0;

  for (int i = 6; i >= 0; i--) {
    uint64_t cur = (remainder << 32) | big->bits[i];
    big->bits[i] = (uint32_t)(cur / 10);
    remainder = cur % 10;
  }

  return (int)remainder;  // 0..9
}

void decimal_to_big_decimal(const s21_decimal* dec, s21_big_decimal* big) {
  for (int i = 0; i < 8; i++) big->bits[i] = 0;

  // мантисса
  for (int i = 0; i < 3; i++) big->bits[i] = dec->bits[i];

  // sign + scale
  set_sign(big->bits, BIG_DEC_BITS, get_sign(dec->bits, DEC_BITS));
  set_scale(big->bits, BIG_DEC_BITS, get_scale(dec->bits, DEC_BITS));
}

void normalize_big_decimals(s21_big_decimal* a, s21_big_decimal* b) {
  int scale_a = get_scale(a->bits, BIG_DEC_BITS);
  int scale_b = get_scale(b->bits, BIG_DEC_BITS);

  while (scale_a < scale_b) {
    big_decimal_multiply_by_10(a);
    scale_a++;
  }

  while (scale_b < scale_a) {
    big_decimal_multiply_by_10(b);
    scale_b++;
  }

  set_scale(a->bits, BIG_DEC_BITS, scale_a);
  set_scale(b->bits, BIG_DEC_BITS, scale_b);
}

void bank_round(s21_big_decimal *big, int remainder) {
  if (remainder < 5)
    return;

  int lsb = big->bits[0] & 1u;

  if (remainder > 5 || (remainder == 5 && lsb)) {
    uint64_t carry = 1;

    for (int i = 0; i < 7 && carry; i++) {
      uint64_t tmp = (uint64_t)big->bits[i] + carry;
      big->bits[i] = (uint32_t)(tmp & 0xFFFFFFFFu);
      carry = tmp >> 32;
    }
  }
}

int big_decimal_to_decimal(s21_big_decimal* big, s21_decimal* dec) {
  int scale = get_scale(big->bits, BIG_DEC_BITS);

  // Уменьшаем scale, пока decimal не вмещается
  while (scale > 28 || !mantissa_fits_96(big)) {
    int remainder = big_decimal_divide_by_10(big);
    bank_round(big, remainder);
    scale--;
  }

  clear_decimal_bits(dec->bits, DEC_BITS);

  for (int i = 0; i < 3; i++) dec->bits[i] = big->bits[i];

  set_sign(dec->bits, DEC_BITS, get_sign(big->bits, BIG_DEC_BITS));
  set_scale(dec->bits, DEC_BITS, scale);

  return 0;
}