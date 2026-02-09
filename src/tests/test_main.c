#include <check.h>

Suite *decimal_helpers_suite(void);
Suite* conversion_suite(void);

int main(void) {
    int failed;
    SRunner *sr = srunner_create(decimal_helpers_suite());
    srunner_add_suite(sr, conversion_suite());

    srunner_run_all(sr, CK_VERBOSE);
    failed = srunner_ntests_failed(sr);
    srunner_free(sr);

    return failed == 0 ? 0 : 1;
}