#include <check.h>

Suite *get_bit_suite(void);
Suite *set_bit_suite(void);
Suite *get_sign_suite(void);
Suite *set_sign_suite(void);
Suite *get_scale_suite(void);
Suite *set_scale_suite(void);
Suite *clear_decimal_bits_suite(void);

int main(void) {
    int failed;
    SRunner *sr = srunner_create(get_bit_suite());

    srunner_add_suite(sr, set_bit_suite());
    srunner_add_suite(sr, get_sign_suite());
    srunner_add_suite(sr, set_sign_suite());
    srunner_add_suite(sr, get_scale_suite());
    srunner_add_suite(sr, set_scale_suite());
    srunner_add_suite(sr, clear_decimal_bits_suite());

    srunner_run_all(sr, CK_VERBOSE);
    failed = srunner_ntests_failed(sr);
    srunner_free(sr);

    return failed == 0 ? 0 : 1;
}