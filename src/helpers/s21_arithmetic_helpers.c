#include "../s21_decimal.h"

// ===================== Конвертация =====================

// Преобразование обычного decimal в big_decimal
// Копирует мантиссу, а также scale и sign
void decimal_to_big_decimal(s21_decimal* dec, s21_big_decimal* big) {
  clear_decimal_bits(big->bits, BIG_DEC_BITS, 1);
  int words = get_mantissa_word_count(DEC_BITS);
  for (int i = 0; i < words; i++) big->bits[i] = dec->bits[i];

  set_sign(big->bits, BIG_DEC_BITS, get_sign(dec->bits, DEC_BITS));
  set_scale(big->bits, BIG_DEC_BITS, get_scale(dec->bits, DEC_BITS));
}

// Проверка, является ли число нулем
// size_in_bits = DEC_BITS или BIG_DEC_BITS
int is_zero(const uint32_t* bits, int size_in_bits) {
  int high = get_high_word_index(size_in_bits);  // число слов мантиссы
  for (int i = 0; i < high; i++) {
    if (bits[i] != 0) return 0;  // найден ненулевой бит → число не ноль
  }
  return 1;  // все биты мантиссы нули → число равно нулю
}

// ===================== Нормализация =====================

// Выравнивание scale и знака двух big_decimal чисел
// Нужно для операций сложения/вычитания
void normalize_big_decimals(s21_big_decimal* a, s21_big_decimal* b) {
  int scale_a = get_scale(a->bits, BIG_DEC_BITS);
  int scale_b = get_scale(b->bits, BIG_DEC_BITS);

  // Если одно число 0, просто копируем scale
  if (is_zero(a->bits, BIG_DEC_BITS)) scale_a = scale_b;
  if (is_zero(b->bits, BIG_DEC_BITS)) scale_b = scale_a;

  // Увеличиваем scale_a до scale_b, пока не переполним a
  while (scale_a < scale_b && !mul10(a->bits, BIG_DEC_BITS)) scale_a++;

  // Увеличиваем scale_b до scale_a, пока не переполним b
  while (scale_b < scale_a && !mul10(b->bits, BIG_DEC_BITS)) scale_b++;

  // Сохраняем обновлённые scale
  set_scale(a->bits, BIG_DEC_BITS, scale_a);
  set_scale(b->bits, BIG_DEC_BITS, scale_b);
}

// ===================== BigDecimal -> Decimal =====================

// Преобразование big_decimal обратно в обычный decimal
void big_decimal_to_decimal(s21_big_decimal* big, s21_decimal* dec) {
  clear_decimal_bits(dec->bits, DEC_BITS, 1);  // обнуляем результат

  int sign = get_sign(big->bits, BIG_DEC_BITS);
  int scale = get_scale(big->bits, BIG_DEC_BITS);

  int mantissa_words = get_mantissa_word_count(DEC_BITS);
  for (int i = 0; i < mantissa_words; i++) {
    dec->bits[i] = big->bits[i];  // копируем мантиссу
  }

  set_scale(dec->bits, DEC_BITS, scale);  // копируем scale
  set_sign(dec->bits, DEC_BITS, sign);    // копируем sign
}

// ===================== Деление и умножение на 10 =====================

// Делим big_decimal на 10, возвращаем остаток (0..9)
int div10(uint32_t* bits, int size_in_bits) {
  uint64_t remainder = 0;
  int high = get_high_word_index(size_in_bits);

  // идём по всем словам мантиссы, старший сначала
  for (int i = high - 1; i >= 0; i--) {
    uint64_t cur = (remainder << 32) | bits[i];
    bits[i] = (uint32_t)(cur / 10);
    remainder = cur % 10;
  }

  return (int)remainder;  // остаток от деления
}

// Умножение big_decimal на 10 (для normalize)
int mul10(uint32_t* bits, int size_in_bits) {
  uint64_t carry = 0;
  int high = get_high_word_index(size_in_bits);

  for (int i = 0; i < high; i++) {
    uint64_t cur = (uint64_t)bits[i] * 10 + carry;
    bits[i] = (uint32_t)cur;
    carry = cur >> 32;
  }

  return (carry != 0);  // 1 если произошло переполнение
}

// ===================== Округление =====================

// Определяем, нужно ли округлять число
// last_rem - последняя цифра, has_tail - есть ли хвост после last_rem
int should_round(int last_rem, int has_tail, const uint32_t* bits) {
  // округляем, если last_rem > 5 или равно 5 и (есть хвост или младший бит
  // мантиссы нечетный)
  int round = (last_rem > 5) || (last_rem == 5 && (has_tail || (bits[0] & 1)));
  return round;
}

// Добавляем 1 к мантиссе decimal или big_decimal
void add_one(uint32_t* bits, int size_in_bits) {
  uint64_t carry = 1;
  int word_count = get_mantissa_word_count(size_in_bits);

  for (int i = 0; i < word_count && carry; i++) {
    uint64_t sum = (uint64_t)bits[i] + carry;
    bits[i] = (uint32_t)sum;
    carry = sum >> 32;
  }
}

// ===================== Вспомогательные функции =====================

// Проверка, помещается ли big_decimal в обычный decimal
int fits_in_decimal(uint32_t* bits) {
  for (int i = 3; i < 7; i++) {  // проверяем старшие слова
    if (bits[i] != 0) return 0;
  }
  return 1;
}

// Сравнение двух big_decimal
// Возвращает 1 если a > b, -1 если a < b, 0 если равны
int compare_big_decimals(const s21_big_decimal* a, const s21_big_decimal* b) {
  int high = get_high_word_index(BIG_DEC_BITS);
  for (int i = high - 1; i >= 0; i--) {
    if (a->bits[i] != b->bits[i]) return (a->bits[i] > b->bits[i]) ? 1 : -1;
  }
  return 0;
}

// Сложение двух big_decimal
void big_add(s21_big_decimal* a, const s21_big_decimal* b) {
  uint64_t carry = 0;
  int words = get_mantissa_word_count(BIG_DEC_BITS);
  for (int i = 0; i < words; i++) {
    uint64_t sum = (uint64_t)a->bits[i] + b->bits[i] + carry;
    a->bits[i] = (uint32_t)sum;
    carry = sum >> 32;
  }
}


void big_sub(s21_big_decimal* a, const s21_big_decimal* b) {
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


void big_mul(const s21_big_decimal* a, const s21_big_decimal* b,
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


void big_div_digit(const s21_big_decimal* divisor, s21_big_decimal* remainder,
                   uint32_t* digit) {
  *digit = 0;


  if (compare_big_decimals(divisor, remainder) > 0) return;

  uint32_t left = 0, right = 9;
  uint32_t best = 0;

  s21_big_decimal tmp = {0};
  s21_big_decimal prod = {0};

  while (left <= right) {
    uint32_t mid = left + (right - left) / 2;

    clear_decimal_bits(tmp.bits, BIG_DEC_BITS, 1);
    tmp.bits[0] = mid;

    clear_decimal_bits(prod.bits, BIG_DEC_BITS, 1);
    big_mul(divisor, &tmp, &prod);

    int cmp = compare_big_decimals(&prod, remainder);
    if (cmp <= 0) {
      best = mid;  
      left = mid + 1;
    } else {
      if (mid == 0) break;
      right = mid - 1;
    }
  }

  *digit = best;


  if (*digit > 0) {
    clear_decimal_bits(tmp.bits, BIG_DEC_BITS, 1);
    tmp.bits[0] = *digit;

    clear_decimal_bits(prod.bits, BIG_DEC_BITS, 1);
    big_mul(divisor, &tmp, &prod);

    big_sub(remainder, &prod);
  }
}


int apply_bankers_rounding(s21_big_decimal* value, int* scale) {
  int status = S21_OK;

  while (!fits_in_decimal(value->bits)) {  
    if (*scale == 0) {

      return get_sign(value->bits, BIG_DEC_BITS) ? S21_TOO_SMALL : S21_TOO_BIG;
    }

    int remainder = div10(value->bits, BIG_DEC_BITS);
    (*scale)--;

    if (should_round(remainder, 0, value->bits)) {
      add_one(value->bits, BIG_DEC_BITS);
    }
  }

  if (*scale > 28) *scale = 28;
  if (*scale < 0) *scale = 0;
  set_scale(value->bits, BIG_DEC_BITS, *scale);

  return status;
}