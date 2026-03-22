#include "../s21_decimal.h"

// Умножение big_decimal на 10 (для normalize)
static void big_decimal_multiply_by_10(s21_big_decimal* big) {
    uint64_t carry = 0;
    for (int i = 0; i < 7; i++) {
        uint64_t tmp = (uint64_t)big->bits[i] * 10 + carry;
        big->bits[i] = (uint32_t)tmp;
        carry = tmp >> 32;
    }
}

// Приведение scale и знака двух big_decimal
void normalize_big_decimals(s21_big_decimal* a, s21_big_decimal* b) {
    int scale_a = get_scale(a->bits, BIG_DEC_BITS);
    int scale_b = get_scale(b->bits, BIG_DEC_BITS);

    while (scale_a < scale_b) { big_decimal_multiply_by_10(a); scale_a++; }
    while (scale_b < scale_a) { big_decimal_multiply_by_10(b); scale_b++; }

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