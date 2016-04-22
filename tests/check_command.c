#include <stdlib.h>
#include <check.h>

#include "../src/context.h"

#define NELEMS(x)  (sizeof(x) / sizeof((x)[0]))

START_TEST (test_cpw_find_token)
{                                      /*             1    1    2    2  */
  /* unit test code */                 /*   0    5    0    5    0    5  */
  char line1[CPW_COMMAND_MAX_ARG_LENGTH] = "      arg    -i <input> -o <output> -x y\n";
  char line2[CPW_COMMAND_MAX_ARG_LENGTH] = "      arg    -i <input > -o output -x y\n";
  char line3[CPW_COMMAND_MAX_ARG_LENGTH] = "      arg    -i <input>";
  char line4[CPW_COMMAND_MAX_ARG_LENGTH] = "      arg    -i <inp";
  char line5[CPW_COMMAND_MAX_ARG_LENGTH] = "<input>";
  char line6[CPW_COMMAND_MAX_ARG_LENGTH] = "input>";
  char line7[CPW_COMMAND_MAX_ARG_LENGTH] = "   arg   <input>    org <input>";

  ck_assert_int_eq(cpw_find_token(line1, "<input>", 0), 16); 
  ck_assert_int_eq(cpw_find_token(line1, "<output>", 0), 27); 
  ck_assert_int_eq(cpw_find_token(line2, "<input>", 0), -1);  
  ck_assert_int_eq(cpw_find_token(line2, "<output>", 0), -1);  
  ck_assert_int_eq(cpw_find_token(line3, "<input>", 0), 16);  
  ck_assert_int_eq(cpw_find_token(line4, "<input>", 0), -1);  
  ck_assert_int_eq(cpw_find_token(line5, "<input>", 0), 0);  
  ck_assert_int_eq(cpw_find_token(line6, "<input>", 0), -1);  
  ck_assert_int_eq(cpw_find_token(line7, "<input>", 0), 9);  
  ck_assert_int_eq(cpw_find_token(line7, "<input>", 1), 24);  
  ck_assert_int_eq(cpw_find_token(line7, "<input>", 2), -1);  
}
END_TEST


START_TEST (test_cpw_command_create)
{
  /* unit test code */
  cpwcommand *command;

  command = cpw_command_new();
  ck_assert(command != NULL);
  
}
END_TEST

START_TEST (test_cpw_command_set_empty_path)
{
  /* unit test code */
  cpwcommand *command;

  command = cpw_command_new();
  ck_assert(command != NULL);
  
  cpw_command_init(command);

  ck_assert( cpw_command_set_empty_path(command, "/path/to/file") );
  ck_assert( ! cpw_command_set_empty_path(command, "/path/to/otherfile") );
}
END_TEST

START_TEST (test_cpw_command_set_empty_name)
{
  /* unit test code */
  cpwcommand *command;

  command = cpw_command_new();
  ck_assert(command != NULL);
  
  cpw_command_init(command);

  ck_assert( cpw_command_set_empty_name(command, "commandname>") );
  ck_assert( ! cpw_command_set_empty_name(command, "othercommandname") );

  ck_assert_str_eq( command->name, "commandname" );
  ck_assert( cpw_command_set_name(command, "othercommandname") );
  ck_assert_str_eq( command->name, "othercommandname" );
}
END_TEST

START_TEST (test_cpw_command_add_arg)
{
  /* unit test code */
  cpwcommand *command;

  command = cpw_command_new();
  ck_assert(command != NULL);
  
  cpw_command_init(command);

  ck_assert( cpw_command_add_arg(command, "-o option") );
  ck_assert_str_eq( command->args->arg, "-o option");
}
END_TEST

START_TEST (test_cpw_command_done)
{
  /* unit test code */
  cpwcommand *command;

  command = cpw_command_new();
  ck_assert(command != NULL);
  
  cpw_command_init(command);

  ck_assert( cpw_command_set_empty_path(command, "/path/to/file") );
  ck_assert( cpw_command_set_name(command, "commandname") );
  ck_assert( cpw_command_add_arg(command, "arg") );
  ck_assert( cpw_command_add_arg(command, "-o option") );

  cpw_command_done(&command);
  ck_assert( command == NULL );
}
END_TEST

Suite * command_suite(void)
{
    Suite *s;
    TCase *tc_core;

    s = suite_create("Command");

    /* Core test case */
    tc_core = tcase_create("Core");

    tcase_add_test(tc_core, test_cpw_find_token);
    tcase_add_test(tc_core, test_cpw_command_create);
    tcase_add_test(tc_core, test_cpw_command_set_empty_path);
    tcase_add_test(tc_core, test_cpw_command_set_empty_name);
    tcase_add_test(tc_core, test_cpw_command_add_arg);
    tcase_add_test(tc_core, test_cpw_command_done);
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

