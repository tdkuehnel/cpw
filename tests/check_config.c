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

  context = cpw_context_new();
  cpw_context_init(context);

  ck_assert( cpw_config_init(context->config, "valid_config.conf") );

}
END_TEST

START_TEST (test_config_cpw_tag_is_arg_allowed)
{
  /* unit test code */
  cpwconfigtag configtags[] = {
    {"Command", {"arg1", "arg2", "arg3", NULL}},
    {"Process", {"arg4", "arg5", "arg6", NULL}},
    {"Job",{NULL}},
  };

  int i;
  cpwlinetoken linetoken;
  
  ck_assert(cpw_tag_is_arg_allowed(configtags, NELEMS(configtags), "Process", "arg5"));
  ck_assert(!cpw_tag_is_arg_allowed(configtags, NELEMS(configtags), "Process", "arg1"));
  ck_assert(!cpw_tag_is_arg_allowed(configtags, NELEMS(configtags), "Job", NULL));

}
END_TEST

START_TEST (test_config_cpw_split_line)
{
  /* unit test code */
  cpwconfigtag configtags[] = {
    {"Command", {"arg", "arg2", "arg4"}},
    {"Process", {"arg", "arg2", "arg4"}},
    {"Job",{}},
  };

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

START_TEST (test_config_cpw_is_closing_tag)
{
  /* unit test code */
  char line1[CPW_CONFIG_MAX_LINE_LENGTH] = "<Tag>";
  char line2[CPW_CONFIG_MAX_LINE_LENGTH] = "</Tag>";

  ck_assert(cpw_is_closing_tag(line1) == 0);
  ck_assert(cpw_is_closing_tag(line2) == 1);
}
END_TEST

START_TEST (test_config_cpw_is_opening_tag)
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

START_TEST (test_config_cpw_parsecontext_new)
{
  /* unit test code */
  cpwparsecontext *parsecontext;

  parsecontext = cpw_parsecontext_new();
  ck_assert( parsecontext != NULL );
}
END_TEST

START_TEST (test_config_cpw_parsecontext_init)
{
  /* unit test code */
  cpwparsecontext *parsecontext;

  parsecontext = cpw_parsecontext_new();
  cpw_parsecontext_init(parsecontext, "valid_config.conf");
  ck_assert( parsecontext->linetoken->is_tag == 0 );
  ck_assert_str_eq( parsecontext->configfile_path, "valid_config.conf");
  ck_assert( parsecontext->configerror == NULL );
}
END_TEST

START_TEST (test_config_cpw_parsecontext_next_token)
{
  /* unit test code */
  cpwparsecontext *parsecontext;

  parsecontext = cpw_parsecontext_new();
  cpw_parsecontext_init(parsecontext, "check_next_token.conf");
  
  ck_assert( cpw_parsecontext_next_token(parsecontext) );
  ck_assert( parsecontext->line_num == 3 );
  ck_assert_str_eq( parsecontext->linetoken->token[0], "<token1" );
  ck_assert_str_eq( parsecontext->linetoken->token[1], "token2>" );
  ck_assert_str_eq( parsecontext->linetoken->token[2], "token3" );
  ck_assert( parsecontext->linetoken->is_tag );
  ck_assert( parsecontext->linetoken->is_opening_tag );
  ck_assert( ! parsecontext->linetoken->is_closing_tag );

  ck_assert( cpw_parsecontext_next_token(parsecontext) );
  ck_assert( parsecontext->line_num == 5 );
  ck_assert_str_eq( parsecontext->linetoken->token[0], "</token1>" );
  ck_assert( parsecontext->linetoken->is_tag );
  ck_assert( ! parsecontext->linetoken->is_opening_tag );
  ck_assert( parsecontext->linetoken->is_closing_tag );

}
END_TEST

START_TEST (test_config_cpw_parsecontext_add_config_error)
{
  /* unit test code */
  cpwparsecontext *parsecontext;

  parsecontext = cpw_parsecontext_new();
  cpw_parsecontext_init(parsecontext, "check_next_token.conf");
  cpw_parsecontext_next_token(parsecontext);
  cpw_parsecontext_add_config_error(parsecontext, "test error. value: %s", parsecontext->linetoken->token[0]);
  ck_assert( parsecontext->configerror != NULL);
  ck_assert( parsecontext->configerror->line_num == 3 );
  ck_assert_str_eq( parsecontext->configerror->error_message, "test error. value: <token1");
}
END_TEST

START_TEST (test_config_cpw_parsecontext_done)
{
  /* unit test code */
  cpwparsecontext *parsecontext;

  parsecontext = cpw_parsecontext_new();
  cpw_parsecontext_init(parsecontext, "check_next_token.conf");
  cpw_parsecontext_done(&parsecontext);
  ck_assert( ! parsecontext );
}
END_TEST

START_TEST (test_config_cpw_config_validate_configfile_syntax)
{
  /* unit test code */
  cpwparsecontext *parsecontext;

  parsecontext = cpw_parsecontext_new();
  cpw_parsecontext_init(parsecontext, "valid_config.conf");

  ck_assert( cpw_config_validate_configfile_syntax(parsecontext) );
  
  cpw_parsecontext_done(&parsecontext);
  parsecontext = cpw_parsecontext_new();
  cpw_parsecontext_init(parsecontext, "invalid_config.conf");

  ck_assert( ! cpw_config_validate_configfile_syntax(parsecontext) );

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
    tcase_add_test(tc_core, test_config_cpw_split_line);
    tcase_add_test(tc_core, test_config_cpw_get_arg);
    tcase_add_test(tc_core, test_config_cpw_get_tag);
    tcase_add_test(tc_core, test_config_cpw_is_closing_tag);
    tcase_add_test(tc_core, test_config_cpw_is_opening_tag);
    tcase_add_test(tc_core, test_config_cpw_tag_is_arg_allowed);
    tcase_add_test(tc_core, test_config_cpw_config_validate_configfile_syntax);

    tcase_add_test(tc_core, test_config_cpw_parsecontext_new);
    tcase_add_test(tc_core, test_config_cpw_parsecontext_init);
    tcase_add_test(tc_core, test_config_cpw_parsecontext_next_token);
    tcase_add_test(tc_core, test_config_cpw_parsecontext_add_config_error);
    tcase_add_test(tc_core, test_config_cpw_parsecontext_done);
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
