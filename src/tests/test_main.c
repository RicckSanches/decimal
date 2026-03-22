#include <check.h>
#include <stdio.h>

Suite *decimal_helpers_suite(void);
Suite* arithmetic_helpers_suite(void);
Suite* conversion_suite(void);

int main(void) {
    int failed_total = 0;

    // -------------------- Decimal Helpers --------------------
    {
        SRunner *sr = srunner_create(decimal_helpers_suite());
        printf("\n=== Running Decimal Helpers Tests ===\n");
        srunner_run_all(sr, CK_VERBOSE);
        int failed = srunner_ntests_failed(sr);
        printf("=== Decimal Helpers: %d test(s) failed ===\n", failed);
        srunner_free(sr);
        failed_total += failed;
    }

    // -------------------- Conversion Tests --------------------
    {
        SRunner *sr = srunner_create(conversion_suite());
        printf("\n=== Running Conversion Tests ===\n");
        srunner_run_all(sr, CK_VERBOSE);
        int failed = srunner_ntests_failed(sr);
        printf("=== Conversion: %d test(s) failed ===\n", failed);
        srunner_free(sr);
        failed_total += failed;
    }

    // -------------------- Arithmetic Helpers (опционально) --------------------
    {
        SRunner *sr = srunner_create(arithmetic_helpers_suite());
        printf("\n=== Running Arithmetic Helpers Tests ===\n");
        srunner_run_all(sr, CK_VERBOSE);
        int failed = srunner_ntests_failed(sr);
        printf("=== Arithmetic Helpers: %d test(s) failed ===\n", failed);
        srunner_free(sr);
        failed_total += failed;
    }

    // -------------------- Итог --------------------
    if (failed_total == 0) {
        printf("\nALL TESTS PASSED ✅\n");
        return 0;
    } else {
        printf("\nTOTAL FAILED TESTS: %d ❌\n", failed_total);
        return 1;
    }
}