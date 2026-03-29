#include <check.h>
#include <stdio.h>

Suite* decimal_helpers_suite(void);
Suite* decimal_utils_suite(void);
Suite* conversion_suite(void);
Suite* arithmetic_helpers_suite(void);
Suite* comparisons_suite(void);
Suite* arithmetic_suite(void);
Suite* integrated_conversion_suite(void);  // <- новый интеграционный сьют

int main(void) {
  int failed_total = 0;

  // -------------------- Decimal Helpers --------------------
  {
    SRunner* sr = srunner_create(decimal_helpers_suite());
    printf("\n=== Running Decimal Helpers Tests ===\n");
    srunner_run_all(sr, CK_VERBOSE);
    int failed = srunner_ntests_failed(sr);
    printf("=== Decimal Helpers: %d test(s) failed ===\n", failed);
    srunner_free(sr);
    failed_total += failed;
  }

  // -------------------- Decimal Utils Functions --------------------
  {
    SRunner* sr = srunner_create(decimal_utils_suite());
    printf(
        "\n=== Running Decimal Utils Tests (truncate/negate/floor/round) "
        "===\n");
    srunner_run_all(sr, CK_VERBOSE);
    int failed = srunner_ntests_failed(sr);
    printf("=== Decimal Utils: %d test(s) failed ===\n", failed);
    srunner_free(sr);
    failed_total += failed;
  }

  // -------------------- Conversion Tests --------------------
  {
    SRunner* sr = srunner_create(conversion_suite());
    printf("\n=== Running Conversion Tests ===\n");
    srunner_run_all(sr, CK_VERBOSE);
    int failed = srunner_ntests_failed(sr);
    printf("=== Conversion: %d test(s) failed ===\n", failed);
    srunner_free(sr);
    failed_total += failed;
  }

  // -------------------- Decimal Comparisons --------------------
  {
    SRunner* sr = srunner_create(comparisons_suite());
    printf("\n=== Running Decimal Comparisons Tests ===\n");
    srunner_run_all(sr, CK_VERBOSE);
    int failed = srunner_ntests_failed(sr);
    printf("=== Decimal Comparisons: %d test(s) failed ===\n", failed);
    srunner_free(sr);
    failed_total += failed;
  }

  // -------------------- Arithmetic Helpers --------------------
  {
    SRunner* sr = srunner_create(arithmetic_helpers_suite());
    printf("\n=== Running Arithmetic Helpers Tests ===\n");
    srunner_run_all(sr, CK_VERBOSE);
    int failed = srunner_ntests_failed(sr);
    printf("=== Arithmetic Helpers: %d test(s) failed ===\n", failed);
    srunner_free(sr);
    failed_total += failed;
  }

  // -------------------- Decimal Arithmetic Operations --------------------
  {
    SRunner* sr = srunner_create(arithmetic_suite());
    printf(
        "\n=== Running Decimal Arithmetic Operations Tests (add/sub/mul/div) "
        "===\n");
    srunner_run_all(sr, CK_VERBOSE);
    int failed = srunner_ntests_failed(sr);
    printf("=== Arithmetic Operations: %d test(s) failed ===\n", failed);
    srunner_free(sr);
    failed_total += failed;
  }

  // -------------------- Integrated Conversion + Arithmetic Tests
  // --------------------
  {
    SRunner* sr = srunner_create(integrated_conversion_suite());
    printf("\n=== Running Integrated Conversion + Arithmetic Tests ===\n");
    srunner_run_all(sr, CK_VERBOSE);
    int failed = srunner_ntests_failed(sr);
    printf("=== Integrated Tests: %d test(s) failed ===\n", failed);
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