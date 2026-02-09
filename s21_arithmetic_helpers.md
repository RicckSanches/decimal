# README – s21 BigDecimal Helpers

Этот модуль реализует вспомогательные функции для работы с **расширенной десятичной арифметикой** (`s21_big_decimal`) и конвертацией между стандартным `s21_decimal` и `s21_big_decimal`.

`s21_big_decimal` используется для безопасной работы с числами, превышающими 96 бит мантиссы стандартного `s21_decimal`, а также для выполнения нормализации, округления и масштабирования.

---

## Структуры

```c
typedef struct {
    uint32_t bits[8]; // 224 бит мантиссы + 32 бит sign/scale
} s21_big_decimal;

typedef struct {
    uint32_t bits[4]; // 96 бит мантиссы + 32 бит sign/scale
} s21_decimal;
```

* `bits[0..2]` – мантисса (младшие 96 бит)
* `bits[3..6]` – старшие слова мантиссы (только для `big_decimal`)
* `bits[7]` – sign (бит 31) и scale (биты 16..23)

---

## Основные функции

| Функция                      | Назначение                                          |
| ---------------------------- | --------------------------------------------------- |
| `mantissa_fits_96`           | Проверяет, помещается ли мантисса в 96 бит          |
| `big_decimal_multiply_by_10` | Умножение числа на 10 с переносом                   |
| `big_decimal_divide_by_10`   | Деление числа на 10 с возвратом остатка             |
| `decimal_to_big_decimal`     | Конвертация `decimal` → `big_decimal`               |
| `normalize_big_decimals`     | Выравнивание scale для двух чисел                   |
| `bank_round`                 | Банковское округление (round-half-to-even)          |
| `big_decimal_to_decimal`     | Конвертация `big_decimal` → `decimal` с округлением |

---

## Диаграмма работы

```
         +----------------------+
         |    s21_decimal d     |
         +----------------------+
                   |
        decimal_to_big_decimal()
                   v
         +----------------------+
         |  s21_big_decimal b   |
         |  (man, scale, sign)  |
         +----------------------+
                   |
           arithmetic ops
     +--------------------------+
     | normalize_big_decimals() |
     | big_decimal_multiply_by_10() |
     | big_decimal_divide_by_10()   |
     +--------------------------+
                   |
           bank_round(remainder)
                   v
         +----------------------+
         |  Rounded s21_big_decimal |
         +----------------------+
                   |
       big_decimal_to_decimal()
                   v
         +----------------------+
         |    s21_decimal d     |
         +----------------------+
```

---

## Пример использования

```c
s21_decimal d = {{123456, 0, 0, 0}};
s21_big_decimal b;

// Конвертация decimal → big_decimal
decimal_to_big_decimal(&d, &b);

// Умножение на 10^2
big_decimal_multiply_by_10(&b);
big_decimal_multiply_by_10(&b);

// Проверка, помещается ли в 96 бит
if (!mantissa_fits_96(&b)) {
    int rem = big_decimal_divide_by_10(&b);
    bank_round(&b, rem);
}

// Конвертация обратно в decimal с округлением
s21_decimal result = {{0}};
big_decimal_to_decimal(&b, &result);
```

**Пояснение:**

1. `decimal_to_big_decimal` – расширяет число для безопасной арифметики.
2. `big_decimal_multiply_by_10` – увеличивает число, корректно обрабатывая перенос.
3. `mantissa_fits_96` + `big_decimal_divide_by_10` + `bank_round` – обеспечивают, чтобы число влезло обратно в 96 бит.
4. `big_decimal_to_decimal` – возвращает число в стандартный формат с правильным scale и sign.

---

## Особенности

* **Little-endian формат слов**
* Для сложения и вычитания больших чисел следует использовать `normalize_big_decimals`
* `bank_round` реализует «round half to even»

---