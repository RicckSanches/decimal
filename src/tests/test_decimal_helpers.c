#include <check.h>

#include "../s21_decimal.h"

/* ============================================================================
 * get_bit / set_bit / sign / scale / clear_decimal_bits
 * ==========================================================================*/

/* ---------------- get_bit ---------------- */
START_TEST(test_get_bit_decimal_all_zero) {
  s21_decimal d = {{0}};
  for (int i = 0; i < 96; i++)
    ck_assert_int_eq(get_bit(d.bits, DEC_BITS, i), 0);
}
END_TEST

START_TEST(test_get_bit_decimal_set_bits) {
  s21_decimal d = {{0}};
  d.bits[0] = 0b1010;
  d.bits[1] = 0x80000000;
  d.bits[2] = 0x1;

  ck_assert_int_eq(get_bit(d.bits, DEC_BITS, 1), 1);
  ck_assert_int_eq(get_bit(d.bits, DEC_BITS, 3), 1);
  ck_assert_int_eq(get_bit(d.bits, DEC_BITS, 63), 1);
  ck_assert_int_eq(get_bit(d.bits, DEC_BITS, 64), 1);
}
END_TEST

START_TEST(test_get_bit_big_decimal_all_zero) {
  s21_big_decimal d = {{0}};
  for (int i = 0; i < 224; i++)
    ck_assert_int_eq(get_bit(d.bits, BIG_DEC_BITS, i), 0);
}
END_TEST

START_TEST(test_get_bit_big_decimal_set_bits) {
  s21_big_decimal d = {{0}};
  d.bits[0] = 0b101;
  d.bits[3] = 0x80000000;
  d.bits[6] = 1u << 31;

  ck_assert_int_eq(get_bit(d.bits, BIG_DEC_BITS, 0), 1);
  ck_assert_int_eq(get_bit(d.bits, BIG_DEC_BITS, 2), 1);
  ck_assert_int_eq(get_bit(d.bits, BIG_DEC_BITS, 127), 1);
  ck_assert_int_eq(get_bit(d.bits, BIG_DEC_BITS, 223), 1);
}
END_TEST

START_TEST(test_get_bit_out_of_range) {
  s21_decimal d = {{0}};
  ck_assert_int_eq(get_bit(d.bits, DEC_BITS, -1), 0);
  ck_assert_int_eq(get_bit(d.bits, DEC_BITS, 224), 0);

  s21_big_decimal bd = {{0}};
  ck_assert_int_eq(get_bit(bd.bits, BIG_DEC_BITS, -10), 0);
  ck_assert_int_eq(get_bit(bd.bits, BIG_DEC_BITS, 300), 0);
}
END_TEST

/* ---------------- set_bit ---------------- */
START_TEST(test_set_bit_decimal) {
  s21_decimal d = {{0}};
  set_bit(d.bits, DEC_BITS, 0, 1);
  set_bit(d.bits, DEC_BITS, 31, 1);
  set_bit(d.bits, DEC_BITS, 64, 1);

  ck_assert_int_eq(get_bit(d.bits, DEC_BITS, 0), 1);
  ck_assert_int_eq(get_bit(d.bits, DEC_BITS, 31), 1);
  ck_assert_int_eq(get_bit(d.bits, DEC_BITS, 64), 1);

  set_bit(d.bits, DEC_BITS, 31, 0);
  ck_assert_int_eq(get_bit(d.bits, DEC_BITS, 31), 0);
}
END_TEST

START_TEST(test_set_bit_big_decimal) {
  s21_big_decimal d = {{0}};
  set_bit(d.bits, BIG_DEC_BITS, 0, 1);
  set_bit(d.bits, BIG_DEC_BITS, 127, 1);
  set_bit(d.bits, BIG_DEC_BITS, 223, 1);

  ck_assert_int_eq(get_bit(d.bits, BIG_DEC_BITS, 0), 1);
  ck_assert_int_eq(get_bit(d.bits, BIG_DEC_BITS, 127), 1);
  ck_assert_int_eq(get_bit(d.bits, BIG_DEC_BITS, 223), 1);
}
END_TEST

/* ---------------- sign ---------------- */
START_TEST(test_decimal_sign) {
  s21_decimal d = {{0}};
  set_sign(d.bits, DEC_BITS, 1);
  ck_assert_int_eq(get_sign(d.bits, DEC_BITS), 1);
  set_sign(d.bits, DEC_BITS, 0);
  ck_assert_int_eq(get_sign(d.bits, DEC_BITS), 0);
}
END_TEST

START_TEST(test_big_decimal_sign) {
  s21_big_decimal d = {{0}};
  set_sign(d.bits, BIG_DEC_BITS, 1);
  ck_assert_int_eq(get_sign(d.bits, BIG_DEC_BITS), 1);
  set_sign(d.bits, BIG_DEC_BITS, 0);
  ck_assert_int_eq(get_sign(d.bits, BIG_DEC_BITS), 0);
}
END_TEST

/* ---------------- scale ---------------- */
START_TEST(test_decimal_scale) {
  s21_decimal d = {{0}};
  set_scale(d.bits, DEC_BITS, 5);
  ck_assert_int_eq(get_scale(d.bits, DEC_BITS), 5);
}
END_TEST

START_TEST(test_big_decimal_scale) {
  s21_big_decimal d = {{0}};
  set_scale(d.bits, BIG_DEC_BITS, 28);
  ck_assert_int_eq(get_scale(d.bits, BIG_DEC_BITS), 28);
}
END_TEST

/* -------------------- Decimal clear tests -------------------- */
START_TEST(test_decimal_clear_keep_metadata) {
  s21_decimal d = {{0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0x80050000}};

  // Очищаем только мантиссу, оставляем metadata
  clear_decimal_bits(d.bits, DEC_BITS, 0);

  // Мантисса обнулена
  for (int i = 0; i < 3; i++) ck_assert_int_eq(d.bits[i], 0);

  // Metadata остались
  ck_assert_int_eq(get_sign(d.bits, DEC_BITS), 1);
  ck_assert_int_eq(get_scale(d.bits, DEC_BITS), 5);
}
END_TEST

START_TEST(test_decimal_clear_full) {
  s21_decimal d = {{0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0x80050000}};

  // Полное обнуление, включая metadata
  clear_decimal_bits(d.bits, DEC_BITS, 1);

  for (int i = 0; i < 4; i++) ck_assert_int_eq(d.bits[i], 0);
}
END_TEST

START_TEST(test_big_decimal_clear_keep_metadata) {
  s21_big_decimal d = {{0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
                        0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0x80050000}};

  // Очищаем мантиссу (первые 7 слов), оставляем metadata
  clear_decimal_bits(d.bits, BIG_DEC_BITS, 0);

  for (int i = 0; i < 7; i++) ck_assert_int_eq(d.bits[i], 0);

  // Metadata остались
  ck_assert_int_eq(get_sign(d.bits, BIG_DEC_BITS), 1);
  ck_assert_int_eq(get_scale(d.bits, BIG_DEC_BITS), 5);
}
END_TEST

START_TEST(test_big_decimal_clear_full) {
  s21_big_decimal d = {{0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
                        0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0x80050000}};

  // Полное обнуление
  clear_decimal_bits(d.bits, BIG_DEC_BITS, 1);

  for (int i = 0; i < 8; i++) ck_assert_int_eq(d.bits[i], 0);
}
END_TEST

START_TEST(test_decimal_clear_zero) {
  s21_decimal d = {{0, 0, 0, 0}};
  clear_decimal_bits(d.bits, DEC_BITS, 0);

  for (int i = 0; i < 3; i++) ck_assert_int_eq(d.bits[i], 0);

  ck_assert_int_eq(get_sign(d.bits, DEC_BITS), 0);
  ck_assert_int_eq(get_scale(d.bits, DEC_BITS), 0);
}
END_TEST

START_TEST(test_big_decimal_clear_zero) {
  s21_big_decimal d = {{0, 0, 0, 0, 0, 0, 0, 0}};
  clear_decimal_bits(d.bits, BIG_DEC_BITS, 0);

  for (int i = 0; i < 7; i++) ck_assert_int_eq(d.bits[i], 0);

  ck_assert_int_eq(get_sign(d.bits, BIG_DEC_BITS), 0);
  ck_assert_int_eq(get_scale(d.bits, BIG_DEC_BITS), 0);
}
END_TEST

/* -------------------- Decimal div10 tests -------------------- */

START_TEST(test_div10_simple) {
  uint32_t bits[4] = {100, 0, 0, 0};

  int rem = div10(bits, DEC_BITS);

  ck_assert_int_eq(rem, 0);
  ck_assert_uint_eq(bits[0], 10);
}
END_TEST

START_TEST(test_div10_with_remainder) {
  uint32_t bits[4] = {123, 0, 0, 0};

  int rem = div10(bits, DEC_BITS);

  ck_assert_int_eq(rem, 3);
  ck_assert_uint_eq(bits[0], 12);
}
END_TEST

START_TEST(test_div10_zero) {
  uint32_t bits[4] = {0, 0, 0, 0};

  int rem = div10(bits, DEC_BITS);

  ck_assert_int_eq(rem, 0);
  ck_assert_uint_eq(bits[0], 0);
}
END_TEST

START_TEST(test_div10_large_value) {
  uint32_t bits[4] = {0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0};

  int rem = div10(bits, DEC_BITS);

  ck_assert(rem >= 0 && rem <= 9);
}
END_TEST

START_TEST(test_div10_big_decimal) {
  uint32_t bits[8] = {1000, 0, 0, 0, 0, 0, 0, 0};

  int rem = div10(bits, BIG_DEC_BITS);

  ck_assert_int_eq(rem, 0);
  ck_assert_uint_eq(bits[0], 100);
}
END_TEST

START_TEST(test_div10_cross_word) {
  uint32_t bits[4] = {0, 1, 0, 0};  // число > 2^32

  int rem = div10(bits, DEC_BITS);

  ck_assert(rem >= 0 && rem <= 9);
}
END_TEST

/* -------------------- Decimal mul10 tests -------------------- */

START_TEST(test_mul10_simple) {
  uint32_t bits[4] = {10, 0, 0, 0};

  int overflow = mul10(bits, DEC_BITS);

  ck_assert_int_eq(overflow, 0);
  ck_assert_uint_eq(bits[0], 100);
}
END_TEST

START_TEST(test_mul10_zero) {
  uint32_t bits[4] = {0, 0, 0, 0};

  int overflow = mul10(bits, DEC_BITS);

  ck_assert_int_eq(overflow, 0);
  ck_assert_uint_eq(bits[0], 0);
}
END_TEST

START_TEST(test_mul10_overflow) {
  uint32_t bits[4] = {0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0};

  int overflow = mul10(bits, DEC_BITS);

  ck_assert_int_eq(overflow, 1);
}
END_TEST

START_TEST(test_mul10_big_decimal) {
  uint32_t bits[8] = {5, 0, 0, 0, 0, 0, 0, 0};

  int overflow = mul10(bits, BIG_DEC_BITS);

  ck_assert_int_eq(overflow, 0);
  ck_assert_uint_eq(bits[0], 50);
}
END_TEST

START_TEST(test_mul10_chain) {
  uint32_t bits[4] = {1, 0, 0, 0};

  mul10(bits, DEC_BITS);
  mul10(bits, DEC_BITS);
  mul10(bits, DEC_BITS);

  ck_assert_uint_eq(bits[0], 1000);
}
END_TEST

START_TEST(test_mul10_cross_word) {
  uint32_t bits[4] = {0xFFFFFFFF, 0, 0, 0};

  int overflow = mul10(bits, DEC_BITS);

  ck_assert_int_eq(overflow, 0);
}
END_TEST

// -------------------- should_round --------------------
START_TEST(test_should_round_less_5) {
  uint32_t bits[3] = {12, 0, 0};
  ck_assert_int_eq(should_round(3, 0, bits),
                   0);  // last_rem < 5 → не округляем
}
END_TEST

START_TEST(test_should_round_greater_5) {
  uint32_t bits[3] = {12, 0, 0};
  ck_assert_int_eq(should_round(7, 0, bits),
                   1);  // last_rem > 5 → округляем
}
END_TEST

START_TEST(test_should_round_equal_5_no_tail_even) {
  uint32_t bits[3] = {12, 0, 0};  // младший бит 0 → не округляем
  ck_assert_int_eq(should_round(5, 0, bits), 0);
}
END_TEST

START_TEST(test_should_round_equal_5_no_tail_odd) {
  uint32_t bits[3] = {13, 0, 0};  // младший бит 1 → округляем
  ck_assert_int_eq(should_round(5, 0, bits), 1);
}
END_TEST

START_TEST(test_should_round_equal_5_with_tail) {
  uint32_t bits[3] = {12, 0, 0};
  ck_assert_int_eq(should_round(5, 1, bits), 1);
}
END_TEST

START_TEST(test_should_round_big_decimal_even) {
  uint32_t bits[8] = {0xFFFFFFFE, 0, 0, 0, 0, 0, 0, 0};
  ck_assert_int_eq(should_round(5, 0, bits), 0);
}
END_TEST

START_TEST(test_should_round_big_decimal_odd) {
  uint32_t bits[8] = {0xFFFFFFFF, 0, 0, 0, 0, 0, 0, 0};
  ck_assert_int_eq(should_round(5, 0, bits), 1);
}
END_TEST

// -------------------- add_one --------------------
START_TEST(test_add_one_simple) {
  uint32_t bits[3] = {1, 0, 0};
  add_one(bits, DEC_BITS);  // использует DEC_BITS вместо 96
  ck_assert_uint_eq(bits[0], 2);
  ck_assert_uint_eq(bits[1], 0);
  ck_assert_uint_eq(bits[2], 0);
}
END_TEST

START_TEST(test_add_one_with_carry) {
  uint32_t bits[3] = {0xFFFFFFFF, 0, 0};
  add_one(bits, DEC_BITS);
  ck_assert_uint_eq(bits[0], 0);
  ck_assert_uint_eq(bits[1], 1);
  ck_assert_uint_eq(bits[2], 0);
}
END_TEST

START_TEST(test_add_one_multiple_carry) {
  uint32_t bits[3] = {0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF};
  add_one(bits, DEC_BITS);
  ck_assert_uint_eq(bits[0], 0);
  ck_assert_uint_eq(bits[1], 0);
  ck_assert_uint_eq(bits[2], 0);
}
END_TEST

START_TEST(test_add_one_no_overflow) {
  uint32_t bits[3] = {123456, 0, 0};
  add_one(bits, DEC_BITS);
  ck_assert_uint_eq(bits[0], 123457);
  ck_assert_uint_eq(bits[1], 0);
  ck_assert_uint_eq(bits[2], 0);
}
END_TEST

START_TEST(test_add_one_big_decimal) {
  uint32_t bits[7] = {0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFE, 0, 0, 0};
  add_one(bits, BIG_DEC_BITS);  // использует BIG_DEC_BITS вместо 224
  ck_assert_uint_eq(bits[0], 0);
  ck_assert_uint_eq(bits[1], 0);
  ck_assert_uint_eq(bits[2], 0);
  ck_assert_uint_eq(bits[3], 0xFFFFFFFF);
  ck_assert_uint_eq(bits[4], 0);
  ck_assert_uint_eq(bits[5], 0);
  ck_assert_uint_eq(bits[6], 0);
}
END_TEST

/* ============================================================================
 * Suite
 * ==========================================================================*/

Suite* decimal_helpers_suite(void) {
  Suite* s = suite_create("Decimal Helpers");
  TCase* tc_core = tcase_create("Core");

  /* get_bit */
  tcase_add_test(tc_core, test_get_bit_decimal_all_zero);
  tcase_add_test(tc_core, test_get_bit_decimal_set_bits);
  tcase_add_test(tc_core, test_get_bit_big_decimal_all_zero);
  tcase_add_test(tc_core, test_get_bit_big_decimal_set_bits);
  tcase_add_test(tc_core, test_get_bit_out_of_range);

  /* set_bit */
  tcase_add_test(tc_core, test_set_bit_decimal);
  tcase_add_test(tc_core, test_set_bit_big_decimal);

  /* sign */
  tcase_add_test(tc_core, test_decimal_sign);
  tcase_add_test(tc_core, test_big_decimal_sign);

  /* scale */
  tcase_add_test(tc_core, test_decimal_scale);
  tcase_add_test(tc_core, test_big_decimal_scale);

  /* clear_decimal_bits */
  tcase_add_test(tc_core, test_decimal_clear_keep_metadata);
  tcase_add_test(tc_core, test_decimal_clear_full);
  tcase_add_test(tc_core, test_big_decimal_clear_keep_metadata);
  tcase_add_test(tc_core, test_big_decimal_clear_full);
  tcase_add_test(tc_core, test_decimal_clear_zero);
  tcase_add_test(tc_core, test_big_decimal_clear_zero);

  // div10
  tcase_add_test(tc_core, test_div10_simple);
  tcase_add_test(tc_core, test_div10_with_remainder);
  tcase_add_test(tc_core, test_div10_zero);
  tcase_add_test(tc_core, test_div10_large_value);
  tcase_add_test(tc_core, test_div10_big_decimal);
  tcase_add_test(tc_core, test_div10_cross_word);

  // mul10
  tcase_add_test(tc_core, test_mul10_simple);
  tcase_add_test(tc_core, test_mul10_zero);
  tcase_add_test(tc_core, test_mul10_overflow);
  tcase_add_test(tc_core, test_mul10_big_decimal);
  tcase_add_test(tc_core, test_mul10_chain);
  tcase_add_test(tc_core, test_mul10_cross_word);

  // should_round
  tcase_add_test(tc_core, test_should_round_less_5);
  tcase_add_test(tc_core, test_should_round_greater_5);
  tcase_add_test(tc_core, test_should_round_equal_5_no_tail_even);
  tcase_add_test(tc_core, test_should_round_equal_5_no_tail_odd);
  tcase_add_test(tc_core, test_should_round_equal_5_with_tail);
  tcase_add_test(tc_core, test_should_round_big_decimal_even);
  tcase_add_test(tc_core, test_should_round_big_decimal_odd);

  // add_one
  tcase_add_test(tc_core, test_add_one_simple);
  tcase_add_test(tc_core, test_add_one_with_carry);
  tcase_add_test(tc_core, test_add_one_multiple_carry);
  tcase_add_test(tc_core, test_add_one_no_overflow);
  tcase_add_test(tc_core, test_add_one_big_decimal);

  suite_add_tcase(s, tc_core);
  return s;
}