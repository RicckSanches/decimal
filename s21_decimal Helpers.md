# s21_decimal Helpers

Этот модуль предоставляет низкоуровневые функции для работы с битами, знаком и масштабом десятичных чисел (`s21_decimal` и `s21_big_decimal`).

## Типы данных

```c
typedef struct {
    uint32_t bits[4]; // 96 бит числа + знак и scale
} s21_decimal;

typedef struct {
    uint32_t bits[8]; // 256 бит числа + знак и scale
} s21_big_decimal;
```

* `bits[0..2]` — значения числа (`s21_decimal`), младший, средний и старший 32-битные слова.
* `bits[3]` — знак и масштаб (scale):

  * Бит 31 — знак (0 = положительное, 1 = отрицательное)
  * Биты 16..23 — scale (степень 10 для деления числа)
  * Остальные биты не используются

---

## Быстрый справочник по функциям

| Функция                                                                | Назначение                                   | Пример использования                         |
| ---------------------------------------------------------------------- | -------------------------------------------- | -------------------------------------------- |
| `int get_bit(const uint32_t *bits, int size_in_bits, int index)`       | Возвращает значение бита по индексу          | `int b = get_bit(d.bits, 96, 0); // 0 или 1` |
| `void set_bit(uint32_t *bits, int size_in_bits, int index, int value)` | Устанавливает бит в 0 или 1                  | `set_bit(d.bits, 96, 0, 1);`                 |
| `int get_sign(const uint32_t *bits)`                                   | Получает знак числа (0 = +, 1 = -)           | `int s = get_sign(d.bits);`                  |
| `void set_sign(uint32_t *bits, int sign)`                              | Устанавливает знак числа                     | `set_sign(d.bits, 1); // отрицательное`      |
| `int get_scale(const uint32_t *bits)`                                  | Получает scale числа (0..28)                 | `int sc = get_scale(d.bits);`                |
| `void set_scale(uint32_t *bits, int scale)`                            | Устанавливает scale числа                    | `set_scale(d.bits, 5);`                      |
| `void clear_decimal_bits(uint32_t *bits, int size_in_bits)`            | Обнуляет все биты числа, кроме знака и scale | `clear_decimal_bits(bd.bits, 256);`          |

---

## Примеры

### Работа с `s21_decimal`

```c
s21_decimal d = {{0}};

// Устанавливаем и читаем биты
set_bit(d.bits, 96, 0, 1);
int bit = get_bit(d.bits, 96, 0); // 1

// Знак и scale
set_sign(d.bits, 1);  // отрицательное
int sign = get_sign(d.bits); // 1

set_scale(d.bits, 5);
int scale = get_scale(d.bits); // 5

// Очистка числа, сохраняя знак и scale
clear_decimal_bits(d.bits, 96);
```

### Работа с `s21_big_decimal`

```c
s21_big_decimal bd = {{0}};

// Биты
set_bit(bd.bits, 256, 127, 1);
int bit = get_bit(bd.bits, 256, 127); // 1

// Знак и scale
set_sign(bd.bits, 0);  // положительное
int sign = get_sign(bd.bits); // 0

set_scale(bd.bits, 10);
int scale = get_scale(bd.bits); // 10

// Очистка числа, сохраняя знак и scale
clear_decimal_bits(bd.bits, 256);
```