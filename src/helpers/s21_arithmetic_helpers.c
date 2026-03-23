#include "../s21_decimal.h"

// Приведение scale и знака двух big_decimal
void normalize_big_decimals(s21_big_decimal* a, s21_big_decimal* b) {
  int scale_a = get_scale(a->bits, BIG_DEC_BITS);
  int scale_b = get_scale(b->bits, BIG_DEC_BITS);

  while (scale_a < scale_b) {
    if (mul10(a->bits, BIG_DEC_BITS)) {
      // overflow → уменьшаем b
      div10(b->bits, BIG_DEC_BITS);
      scale_b--;
    } else {
      scale_a++;
    }
  }

  while (scale_b < scale_a) {
    if (mul10(b->bits, BIG_DEC_BITS)) {
      // overflow → уменьшаем a
      div10(a->bits, BIG_DEC_BITS);
      scale_a--;
    } else {
      scale_b++;
    }
  }

  set_scale(a->bits, BIG_DEC_BITS, scale_a);
  set_scale(b->bits, BIG_DEC_BITS, scale_b);
}

// Конвертация s21_decimal -> s21_big_decimal
void decimal_to_big_decimal(const s21_decimal* dec, s21_big_decimal* big) {
  for (int i = 0; i < 8; i++) big->bits[i] = 0;
  for (int i = 0; i < 3; i++) big->bits[i] = dec->bits[i];
  set_sign(big->bits, BIG_DEC_BITS, get_sign(dec->bits, DEC_BITS));
  set_scale(big->bits, BIG_DEC_BITS, get_scale(dec->bits, DEC_BITS));
}

int s21_should_round(int last_rem, int has_tail, s21_decimal* value) {
  int need_round = 0;

  if (last_rem > 5) {
    need_round = 1;
  } else if (last_rem == 5) {
    need_round = has_tail || (value->bits[0] % 2 != 0);
  }

  return need_round;
}

void s21_add_one(s21_decimal* value) {
  uint64_t carry = 1;
  for (int i = 0; i < 3 && carry; i++) {
    uint64_t sum = (uint64_t)value->bits[i] + carry;
    value->bits[i] = (uint32_t)sum;
    carry = sum >> 32;
  }
}