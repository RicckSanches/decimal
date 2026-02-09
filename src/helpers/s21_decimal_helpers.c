#include "../s21_decimal.h"

int get_bit(const uint32_t *bits, int size_in_bits, int index) {
    int word = index / 32;
    int bit  = index % 32;

    int in_range = (index >= 0 && index < size_in_bits);
    return in_range * ((bits[word] >> bit) & 1u);
}

void set_bit(uint32_t *bits, int size_in_bits, int index, int value) {
    int word = index / 32;
    int bit  = index % 32;

    int in_range = (index >= 0 && index < size_in_bits);
    if (in_range) {
        bits[word] = (bits[word] & ~(1u << bit)) | ((value ? 1u : 0u) << bit);
    }
}

int get_sign(const uint32_t *bits) {
    return (bits[3] >> 31) & 1u;
}

void set_sign(uint32_t *bits, int sign) {
    if (sign)
        bits[3] |= (1u << 31);  // ставим бит
    else
        bits[3] &= ~(1u << 31); // сбрасываем бит
}

int get_scale(const uint32_t *bits) {
    return (bits[3] >> 16) & 0xFF;  // биты 16..23
}

void set_scale(uint32_t *bits, int scale) {
    bits[3] &= ~(0xFF << 16);       // обнуляем биты 16..23
    bits[3] |= ((scale & 0xFF) << 16); // записываем новый scale
}

void clear_decimal_bits(uint32_t *bits, int size_in_bits) {
    int words = size_in_bits / 32;
    for (int i = 0; i < words; i++) {
        if (i == 3) {
            // сохраняем бит знака (31) и scale (16..23)
            uint32_t sign_scale = bits[3] & 0x80000000;  // знак
            sign_scale |= bits[3] & 0x00FF0000;          // scale
            bits[3] = sign_scale;
        } else {
            bits[i] = 0;
        }
    }
}