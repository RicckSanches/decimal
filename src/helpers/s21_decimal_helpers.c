#include "../s21_decimal.h"

static inline int get_mantissa_bits(int size_in_bits) {
  return size_in_bits - 32;
}

int get_bit(const uint32_t* bits, int size_in_bits, int index) {
  int res = 0;
  int mantissa = get_mantissa_bits(size_in_bits);
  if (index >= 0 && index < mantissa) {  // ограничиваемся мантиссой
    int word = index / 32;
    int bit = index % 32;
    res = (bits[word] >> bit) & 1u;
  }
  return res;
}

void set_bit(uint32_t* bits, int size_in_bits, int index, int value) {
  int mantissa = get_mantissa_bits(size_in_bits);
  if (index >= 0 && index < mantissa) {
    int word = index / 32;
    int bit = index % 32;
    bits[word] = (bits[word] & ~(1u << bit)) | ((value ? 1u : 0u) << bit);
  }
}

int get_sign(const uint32_t* bits, int size_in_bits) {
  int high = get_high_word_index(size_in_bits);
  return (bits[high] >> 31) & 1u;
}

void set_sign(uint32_t* bits, int size_in_bits, int sign) {
  int high = get_high_word_index(size_in_bits);
  if (sign)
    bits[high] |= (1u << 31);
  else
    bits[high] &= ~(1u << 31);
}

int get_scale(const uint32_t* bits, int size_in_bits) {
  int high = get_high_word_index(size_in_bits);
  return (bits[high] >> 16) & 0xFF;
}

void set_scale(uint32_t* bits, int size_in_bits, int scale) {
  int high = get_high_word_index(size_in_bits);
  bits[high] &= ~(0xFF << 16);  // обнуляем scale
  bits[high] |= ((scale & 0xFF) << 16);
}

void clear_decimal_bits(uint32_t* bits, int size_in_bits, int clear_metadata) {
  int high = get_high_word_index(size_in_bits);
  for (int i = 0; i < high; i++) {
    bits[i] = 0;
  }
  if (clear_metadata) {
    bits[high] = 0;
  }
}

// Делим big_decimal на 10, возвращаем остаток 0..9
int div10(uint32_t *bits, int size_in_bits) {
    uint64_t remainder = 0;

    int high = get_high_word_index(size_in_bits);

    // идём по ВСЕЙ мантиссе (кроме metadata)
    for (int i = high - 1; i >= 0; i--) {
        uint64_t cur = (remainder << 32) | bits[i];
        bits[i] = (uint32_t)(cur / 10);
        remainder = cur % 10;
    }

    return (int)remainder;
}

// Умножение big_decimal на 10 (для normalize)
int mul10(uint32_t *bits, int size_in_bits) {
    uint64_t carry = 0;

    int high = get_high_word_index(size_in_bits);

    for (int i = 0; i < high; i++) {
        uint64_t cur = (uint64_t)bits[i] * 10 + carry;
        bits[i] = (uint32_t)cur;
        carry = cur >> 32;
    }

    return (carry != 0); // 1 если overflow
}