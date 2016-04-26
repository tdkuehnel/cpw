#include <stdlib.h>
#include <check.h>
#include "../src/context.h"

#define NELEMS(x)  (sizeof(x) / sizeof((x)[0]))

START_TEST (test_cpw_process_create)
{
  /* unit test code */
  cpwprocess *process;

  process = cpw_process_new();
  ck_assert(process != NULL);
  
}
END_TEST

START_TEST (test_cpw_process_add_arg)
{
  /* unit test code */
  cpwprocess *process;

  process = cpw_process_new();
   
  ck_assert(cpw_process_add_arg(process, "-y"));
  ck_assert_str_eq( process->args->arg, "-y");
  ck_assert(cpw_process_add_arg(process, "-f lavfi"));
  ck_assert_str_eq( process->args->next->arg, "-f lavfi");
}
END_TEST

START_TEST (test_cpw_process_set_value)
{
  /* unit test code */
  cpwprocess *process;

  process = cpw_process_new();
  ck_assert(cpw_process_set_value(process, "arg", "-y", NULL));
  ck_assert(!cpw_process_set_value(process, "badarg", "-y", NULL));
}
END_TEST

START_TEST (test_cpw_process_free)
{
  /* unit test code */
  cpwprocess *process;

  process = cpw_process_new();
  ck_assert(process != NULL);
  
  cpw_process_init(process);

  ck_assert( cpw_process_set_name(process, "processname") );
  ck_assert( cpw_process_add_arg(process, "arg") );
  ck_assert( cpw_process_add_arg(process, "-o option") );

  cpw_process_free(&process);
  ck_assert( process == NULL );
}
END_TEST

Suite * process_suite(void)
{
    Suite *s;
    TCase *tc_core;

    s = suite_create("Process");

    /* Core test case */
    tc_core = tcase_create("Core");

    tcase_add_test(tc_core, test_cpw_process_create);
    tcase_add_test(tc_core, test_cpw_process_add_arg);
    tcase_add_test(tc_core, test_cpw_process_free);
    tcase_add_test(tc_core, test_cpw_process_set_value);
    suite_add_tcase(s, tc_core);
    return s;
}

int main(void)
{
  int number_failed;
  Suite *s;
  SRunner *sr;
  
  s = process_suite();
  sr = srunner_create(s);
  
  srunner_run_all(sr, CK_NORMAL);
  number_failed = srunner_ntests_failed(sr);
  srunner_free(sr);
  return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
