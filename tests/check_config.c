#include <stdlib.h>
#include <check.h>
#include "../src/context.h"

#include "../src/config.h"
#include "../src/process.h"

#define NELEMS(x)  (sizeof(x) / sizeof((x)[0]))

START_TEST (test_config_cpw_config_init)
{
  /* unit test code */
  cpwcontext *context;

  ck_assert(cpw_config_init(context) != 0);
}
END_TEST


START_TEST (test_config_cpw_get_arg)
{
  /* unit test code */
  char line1[CPW_CONFIG_MAX_LINE_LENGTH] = "<Tag Argument>";
  char line2[CPW_CONFIG_MAX_LINE_LENGTH] = "'One Argument' \"Another\" ";

  char line3[CPW_CONFIG_MAX_LINE_LENGTH] = "<ThisIsATag >";
  char line4[CPW_CONFIG_MAX_LINE_LENGTH] = "<ThisIsATag";
  char line5[CPW_CONFIG_MAX_LINE_LENGTH] = "<ThisIsATag> ";

  char cmd[64];
  const char *p;
  p = line1;
  cpw_get_arg(cmd, sizeof(cmd), &p);
  ck_assert_str_eq(cmd, "<Tag");

  cpw_get_arg(cmd, sizeof(cmd), &p);
  ck_assert_str_eq(cmd, "Argument>");

  p = line2;

  cpw_get_arg(cmd, sizeof(cmd), &p);
  ck_assert_str_eq(cmd, "One Argument");

  cpw_get_arg(cmd, sizeof(cmd), &p);
  ck_assert_str_eq(cmd, "Another");
}
END_TEST

START_TEST (test_config_cpw_is_tag)
{
  /* unit test code */
  char line1[CPW_CONFIG_MAX_LINE_LENGTH] = "<Tag";
  char line2[CPW_CONFIG_MAX_LINE_LENGTH] = "<Tag>";
  char line3[CPW_CONFIG_MAX_LINE_LENGTH] = "<Tag Arg>";
  char line4[CPW_CONFIG_MAX_LINE_LENGTH] = "</Tag";
  char line5[CPW_CONFIG_MAX_LINE_LENGTH] = "No Tag";
  
  char cmd[64];
  const char *p;

  p = line1;
  ck_assert(cpw_is_tag(p) == 0);
  p = line2;
  ck_assert(cpw_is_tag(p) == 0);
  p = line3;
  ck_assert(cpw_is_tag(p) == 0);
  p = line4;
  ck_assert(cpw_is_tag(p) == 0);
  p = line5;
  ck_assert(cpw_is_tag(p) != 0);

}
END_TEST

START_TEST (test_config_cpw_get_tag)
{
  /* unit test code */
  char *lines[5] = {
    "<Tag",
    "<Tag>",
    "<Tag Arg>",
    "  </Tag",
    "</Tag>"
  };

  int i;
 
  char cmd[64];
  char tag[64];
  const char *p;

  for ( i = 0; i < NELEMS(lines); i++ ) {
    p = lines[i];
    cpw_get_arg(cmd, sizeof(cmd), &p);
    p = cmd;
    cpw_get_tag(tag, sizeof(tag), &p);
    ck_assert_str_eq(tag, "Tag");
  }
}
END_TEST

START_TEST (test_config_cpw_has_arg)
{
  /* unit test code */
  char *lines[5] = {
    "<Tag arg>",
    "<Tag> arg ",
    "arg arg",
    "",
    "</Tag>"
  };
  int i;
  char cmd[64];
  char tag[64];
  const char *p;

  for ( i = 0; i < 3; i++ ) {
    p = lines[i];
    cpw_get_arg(cmd, sizeof(cmd), &p);
    ck_assert(cpw_has_arg(p) == 1);
  }
  for ( i = 3; i < NELEMS(lines); i++ ) {
    p = lines[i];
    cpw_get_arg(cmd, sizeof(cmd), &p);
    ck_assert(cpw_has_arg(p) == 0);
  }
}
END_TEST

cpwprocess *process;

START_TEST (test_config_cpw_config_process)
{
  /* unit test code */
  FILE *f;
  char line[CPW_CONFIG_MAX_LINE_LENGTH];
  char cmd[64];
  char tag[64];
  const char *p;

  process = cpw_process_new();
  ck_assert(process != NULL);
  f = fopen("process.conf", "r");
  ck_assert(f != NULL);
  ck_assert(fgets(line, sizeof(line), f) != NULL);
  p = line;
  cpw_get_arg(cmd, sizeof(cmd), &p);
  p = cmd;
  cpw_get_tag(tag, sizeof(tag), &p);
  ck_assert_str_eq(tag, "Process");
  cpw_config_process(process, f);
}
END_TEST

Suite * config_suite(void)
{
    Suite *s;
    TCase *tc_core;

    s = suite_create("Config");

    /* Core test case */
    tc_core = tcase_create("Core");

    tcase_add_test(tc_core, test_config_cpw_config_init);
    suite_add_tcase(s, tc_core);
    return s;
}

int main(void)
{
  int number_failed;
  Suite *s;
  SRunner *sr;
  
  s = config_suite();
  sr = srunner_create(s);
  
  srunner_run_all(sr, CK_NORMAL);
  number_failed = srunner_ntests_failed(sr);
  srunner_free(sr);
  return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
