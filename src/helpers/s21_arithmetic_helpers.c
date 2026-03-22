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