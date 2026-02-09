all: clean s21_decimal.a test main

ifeq ($(shell uname),Linux)
    CHECK_LIBS = -lcheck -lm -lpthread -lrt -lsubunit
else
    CHECK_LIBS = -lcheck -lm -lpthread
endif

s21_decimal.a:
	gcc -std=c11 -Wall -Wextra -Werror -Wno-unused-parameter -c helpers/s21_*.c
	ar rcs s21_decimal.a *.o
	rm -f *.o

test: s21_decimal.a
	gcc -std=c11 -Wall -Wextra -Werror -Wno-unused-parameter tests/*.c s21_decimal.a -o test_app $(CHECK_LIBS)
	./test_app

main: s21_decimal.a
	gcc -std=c11 -Wall -Wextra -Werror -Wno-unused-parameter main.c s21_decimal.a -o main_app
	@echo "✅ Программа собрана: ./main_app"

run: main
	./main_app

gcov_report: clean
	gcc -std=c11 -Wall -Wextra -Werror -Wno-unused-parameter --coverage helpers/s21_*.c tests/*.c -o gcov_test $(CHECK_LIBS)
	./gcov_test
	lcov -t "gcov_report" -o report.info -c -d .
	genhtml -o report report.info

clean:
	rm -rf *.o *.a test_app main_app gcov_test *.gcno *.gcda report.info report