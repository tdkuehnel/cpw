#include <stdlib.h>
#include <check.h>
#include "../src/context.h"

#include "../src/config.h"
#include "../src/helper.h"

#define NELEMS(x)  (sizeof(x) / sizeof((x)[0]))

START_TEST (test_helper_cpw_find_token)
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

START_TEST (test_helper_cpw_is_tag)
{
  /* unit test code */
  char line1[CPW_CONFIG_MAX_LINE_LENGTH] = "<Tag";
  char line2[CPW_CONFIG_MAX_LINE_LENGTH] = "<Tag>";
  char line3[CPW_CONFIG_MAX_LINE_LENGTH] = "<Tag Arg>";
  char line4[CPW_CONFIG_MAX_LINE_LENGTH] = "</Tag";
  char line5[CPW_CONFIG_MAX_LINE_LENGTH] = "No Tag";
  char line6[CPW_CONFIG_MAX_LINE_LENGTH] = "<>";
  char line7[CPW_CONFIG_MAX_LINE_LENGTH] = "<";
  char line8[CPW_CONFIG_MAX_LINE_LENGTH] = ">";
  
  char cmd[64];
  const char *p;

  p = line1;
  ck_assert( cpw_is_tag(p) );
  p = line2;
  ck_assert( cpw_is_tag(p) );
  p = line3;
  ck_assert( cpw_is_tag(p) );
  p = line4;
  ck_assert( cpw_is_tag(p) );
  p = line5;
  ck_assert( ! cpw_is_tag(p) );
  p = line6;
  ck_assert( ! cpw_is_tag(p) );
  p = line7;
  ck_assert( ! cpw_is_tag(p) );
  p = line8;
  ck_assert( ! cpw_is_tag(p) );

}
END_TEST

START_TEST (test_helper_cpw_is_opening_tag)
{
  /* unit test code */
  char line1[CPW_CONFIG_MAX_LINE_LENGTH] = "<Tag>";
  char line2[CPW_CONFIG_MAX_LINE_LENGTH] = "</Tag>";
  char line3[CPW_CONFIG_MAX_LINE_LENGTH] = "Tag";

  ck_assert(cpw_is_opening_tag(line1) == 1);
  ck_assert(cpw_is_opening_tag(line2) == 0);
  ck_assert(cpw_is_opening_tag(line3) == 0);
}
END_TEST

START_TEST (test_helper_cpw_is_closing_tag)
{
  /* unit test code */
  char line1[CPW_CONFIG_MAX_LINE_LENGTH] = "<Tag>";
  char line2[CPW_CONFIG_MAX_LINE_LENGTH] = "</Tag>";

  ck_assert(cpw_is_closing_tag(line1) == 0);
  ck_assert(cpw_is_closing_tag(line2) == 1);
}
END_TEST

START_TEST (test_helper_cpw_get_arg)
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

START_TEST (test_helper_cpw_get_tag)
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
  const char *p, *q;;

  q = cmd;
  for ( i = 0; i < NELEMS(lines); i++ ) {
    p = lines[i];
    cpw_get_arg(cmd, sizeof(cmd), &p);
    p = cmd;
    cpw_get_tag(tag, sizeof(tag), &p);
    ck_assert_str_eq(tag, "Tag");
    ck_assert( q == cmd ); 
  }
}
END_TEST

START_TEST (test_helper_cpw_split_line)
{
  /* unit test code */
  int i;
  cpwlinetoken linetoken;

  for (i=0; i<CPW_CONFIG_MAX_LINE_TOKEN; i++) linetoken.token[i] = malloc(CPW_CONFIG_MAX_TAG_LENGTH);

  char line1[CPW_CONFIG_MAX_LINE_LENGTH] = "<Command>\n";
  char line2[CPW_CONFIG_MAX_LINE_LENGTH] = "<Command Name>\n";
  char line3[CPW_CONFIG_MAX_LINE_LENGTH] = "<Command Name> #Comment\n";
  char line4[CPW_CONFIG_MAX_LINE_LENGTH] = "     arg simple\n";
  char line5[CPW_CONFIG_MAX_LINE_LENGTH] = "     arg \'simple quoted\' \n";
  char line6[CPW_CONFIG_MAX_LINE_LENGTH] = "     arg \'simple quoted\' argument\n";
  char line7[CPW_CONFIG_MAX_LINE_LENGTH] = "";

  cpw_split_line(line1, &linetoken);
  ck_assert(linetoken.num == 1);
  ck_assert_str_eq(linetoken.token[0], "<Command>");
  
  cpw_split_line(line2, &linetoken);
  ck_assert(linetoken.num == 2);
  ck_assert_str_eq(linetoken.token[0], "<Command");
  ck_assert_str_eq(linetoken.token[1], "Name>");  
  
  cpw_split_line(line3, &linetoken);
  ck_assert(linetoken.num == 2);
  ck_assert_str_eq(linetoken.token[0], "<Command");
  ck_assert_str_eq(linetoken.token[1], "Name>");  
  
  cpw_split_line(line4, &linetoken);
  ck_assert(linetoken.num == 2);
  ck_assert_str_eq(linetoken.token[0], "arg");
  ck_assert_str_eq(linetoken.token[1], "simple");  
  
  cpw_split_line(line5, &linetoken);
  ck_assert(linetoken.num == 2);
  ck_assert_str_eq(linetoken.token[0], "arg");
  ck_assert_str_eq(linetoken.token[1], "simple quoted");  

  cpw_split_line(line6, &linetoken);
  ck_assert(linetoken.num == 3);
  ck_assert_str_eq(linetoken.token[0], "arg");
  ck_assert_str_eq(linetoken.token[1], "simple quoted");  
  ck_assert_str_eq(linetoken.token[2], "argument");  
  
  cpw_split_line(line7, &linetoken);
  ck_assert(linetoken.num == 0);
}
END_TEST

START_TEST (test_helper_cpw_get_tag_name_index)
{
  /* unit test code */
  cpwlinetoken *linetoken;
  int i;
  char line1[CPW_CONFIG_MAX_LINE_LENGTH] = "<tag name> arg\0";
  char line2[CPW_CONFIG_MAX_LINE_LENGTH] = "<tag name > arg\0";
  char line3[CPW_CONFIG_MAX_LINE_LENGTH] = "<tag> arg\0";
  
  linetoken = calloc(1, sizeof(cpwlinetoken));
  for (i=0; i<CPW_CONFIG_MAX_LINE_TOKEN; i++) {
    linetoken->token[i] = malloc(CPW_CONFIG_MAX_TAG_LENGTH);
  }
  linetoken->is_tag = 1;
  linetoken->tag_name_index = 0;
  linetoken->is_opening_tag = 1;
  linetoken->is_closing_tag = 0;

  cpw_split_line(line1, linetoken); 
  ck_assert_int_eq( cpw_get_tag_name_index(linetoken), 1); 
  
  cpw_split_line(line2, linetoken); 
  ck_assert_int_eq( cpw_get_tag_name_index(linetoken), 1); 
  
  cpw_split_line(line3, linetoken); 
  ck_assert_int_eq( cpw_get_tag_name_index(linetoken), 0); 

}
END_TEST

Suite * helper_suite(void)
{
    Suite *s;
    TCase *tc_core;

    s = suite_create("Helper");

    /* Core test case */
    tc_core = tcase_create("Core");

    tcase_add_test(tc_core, test_helper_cpw_find_token);
    tcase_add_test(tc_core, test_helper_cpw_is_tag);
    tcase_add_test(tc_core, test_helper_cpw_is_opening_tag);
    tcase_add_test(tc_core, test_helper_cpw_is_closing_tag);
    tcase_add_test(tc_core, test_helper_cpw_get_arg);
    tcase_add_test(tc_core, test_helper_cpw_get_tag);
    tcase_add_test(tc_core, test_helper_cpw_split_line);
    tcase_add_test(tc_core, test_helper_cpw_get_tag_name_index);

    suite_add_tcase(s, tc_core);
    return s;
}

int main(void)
{
  int number_failed;
  Suite *s;
  SRunner *sr;
  
  s = helper_suite();
  sr = srunner_create(s);
  
  srunner_run_all(sr, CK_NORMAL);
  number_failed = srunner_ntests_failed(sr);
  srunner_free(sr);
  return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
