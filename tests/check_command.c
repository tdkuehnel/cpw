#include <stdlib.h>
#include <check.h>

#include "../src/context.h"

#define NELEMS(x)  (sizeof(x) / sizeof((x)[0]))

START_TEST (test_cpw_command_create)
{
  /* unit test code */
  cpwcommand *command;

  command = cpw_command_new();
  ck_assert(command != NULL);
  
}
END_TEST

Suite * command_suite(void)
{
    Suite *s;
    TCase *tc_core;

    s = suite_create("Command");

    /* Core test case */
    tc_core = tcase_create("Core");

    tcase_add_test(tc_core, test_cpw_command_create);
    suite_add_tcase(s, tc_core);
    return s;
}

int main(void)
{
  int number_failed;
  Suite *s;
  SRunner *sr;
  
  s = command_suite();
  sr = srunner_create(s);
  
  srunner_run_all(sr, CK_NORMAL);
  number_failed = srunner_ntests_failed(sr);
  srunner_free(sr);
  return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
