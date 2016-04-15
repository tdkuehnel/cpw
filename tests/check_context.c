#include <stdlib.h>
#include <check.h>
#include "../src/context.h"
/*#include "../src/process.h"*/

#define NELEMS(x)  (sizeof(x) / sizeof((x)[0]))

START_TEST (test_context_cpw_context_new)
{
  /* unit test code */
  cpwcontext *context;
  
  context = cpw_context_new();
  ck_assert(context != NULL);
}
END_TEST

Suite * context_suite(void)
{
    Suite *s;
    TCase *tc_core;

    s = suite_create("Context");

    /* Core test case */
    tc_core = tcase_create("Core");

    tcase_add_test(tc_core, test_context_cpw_context_new);
    suite_add_tcase(s, tc_core);
    return s;
}

int main(void)
{
  int number_failed;
  Suite *s;
  SRunner *sr;
  
  s = context_suite();
  sr = srunner_create(s);
  
  srunner_run_all(sr, CK_NORMAL);
  number_failed = srunner_ntests_failed(sr);
  srunner_free(sr);
  return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
