all: clean s21_decimal.a test main

s21_decimal.a:
	gcc -std=c11 -Wall -Wextra -Werror -Wno-c2x-extensions -c src/helpers/s21_*.c
	ar rcs s21_decimal.a *.o
	rm -f *.o

test: s21_decimal.a
	gcc -std=c11 -Wall -Wextra -Werror -Wno-c2x-extensions tests/*.c s21_decimal.a -o test_app -lcheck -lm -lpthread
	./test_app

main: s21_decimal.a
	gcc -std=c11 -Wall -Wextra -Werror -Wno-c2x-extensions main.c s21_decimal.a -o main_app
	@echo "✅ Программа собрана: ./main_app"

run: main
	./main_app

gcov_report:
	gcc -std=c11 -Wall -Wextra -Werror -Wno-c2x-extensions --coverage src/helpers/s21_*.c tests/*.c -o gcov_test -lcheck -lm -lpthread
	./gcov_test
	lcov -t "gcov_report" -o report.info -c -d .
	genhtml -o report report.info

clean:
	rm -rf *.o *.a test_app main_app gcov_test *.gcno *.gcda report.info report