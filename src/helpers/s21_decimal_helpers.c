#include "../s21_decimal.h"

int get_bit(s21_decimal d, int index) {
    if (index < 0 || index > 95)
        return 0; // или assert / ошибка — как решишь

    int word = index / 32;
    int bit  = index % 32;

    return (d.bits[word] >> bit) & 1u;
}

void set_bit(s21_decimal *d, int index, int value);
int get_sign(s21_decimal d);
void set_sign(s21_decimal *d, int sign);
int get_scale(s21_decimal d);
void set_scale(s21_decimal *d, int scale);
void clear_decimal(s21_decimal *d);