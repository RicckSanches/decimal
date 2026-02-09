#include <check.h>

Suite *get_bit_suite(void);
Suite *set_bit_suite(void);
Suite *sign_suite(void);
Suite *scale_suite(void);
Suite *clear_bits_suite(void);

int main(void) {
    int number_failed;

    Suite *s_get  = get_bit_suite();
    Suite *s_set  = set_bit_suite();
    Suite *s_sign  = sign_suite();
    Suite *s_scale  = scale_suite();
    Suite *s_clear  = clear_bits_suite();

    SRunner *sr = srunner_create(s_get);
    srunner_add_suite(sr, s_set);
    srunner_add_suite(sr, s_sign);
    srunner_add_suite(sr, s_scale);
    srunner_add_suite(sr, s_clear);

    srunner_run_all(sr, CK_VERBOSE);
    number_failed = srunner_ntests_failed(sr);

    srunner_free(sr);

    return (number_failed == 0) ? 0 : 1;
}