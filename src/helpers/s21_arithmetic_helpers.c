#include "../s21_decimal.h"

// Конвертация s21_decimal -> s21_big_decimal
void decimal_to_big_decimal(const s21_decimal* dec, s21_big_decimal* big) {
  for (int i = 0; i < 8; i++) big->bits[i] = 0;
  for (int i = 0; i < 3; i++) big->bits[i] = dec->bits[i];
  set_sign(big->bits, BIG_DEC_BITS, get_sign(dec->bits, DEC_BITS));
  set_scale(big->bits, BIG_DEC_BITS, get_scale(dec->bits, DEC_BITS));
}

int is_zero(const uint32_t* bits, int size_in_bits) {
    int high = get_high_word_index(size_in_bits);
    for (int i = 0; i < high; i++) {
        if (bits[i] != 0) return 0;  // ненулевой бит найден → число не ноль
    }
    return 1;  // все биты мантиссы нули → число ноль
}

// Приведение scale и знака двух big_decimal
void normalize_big_decimals(s21_big_decimal* a, s21_big_decimal* b) {
    int scale_a = get_scale(a->bits, BIG_DEC_BITS);
    int scale_b = get_scale(b->bits, BIG_DEC_BITS);

    // если одно число 0, просто копируем scale
    if (is_zero(a->bits, BIG_DEC_BITS)) scale_a = scale_b;
    if (is_zero(b->bits, BIG_DEC_BITS)) scale_b = scale_a;

    // увеличиваем scale_a до scale_b, пока не переполним a
    while (scale_a < scale_b && !mul10(a->bits, BIG_DEC_BITS)) scale_a++;

    // увеличиваем scale_b до scale_a, пока не переполним b
    while (scale_b < scale_a && !mul10(b->bits, BIG_DEC_BITS)) scale_b++;

    // сохраняем обновленные scale
    set_scale(a->bits, BIG_DEC_BITS, scale_a);
    set_scale(b->bits, BIG_DEC_BITS, scale_b);
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