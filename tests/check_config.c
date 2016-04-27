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

  ck_assert( cpw_config_init(context->config, "valid_syntax_config.conf") );

  cpw_context_free(&context);
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
  ck_assert( cpw_parsecontext_init(parsecontext, "valid_logic_config.conf") );
  ck_assert( parsecontext->linetoken->is_tag == 0 );
  ck_assert_str_eq( parsecontext->configfile_path, "valid_logic_config.conf");
  ck_assert( parsecontext->configerror == NULL );
}
END_TEST

START_TEST (test_config_cpw_parsecontext_seek_to_tag)
{
  /* unit test code */
  cpwparsecontext *parsecontext;

  parsecontext = cpw_parsecontext_new();
  ck_assert( cpw_parsecontext_init(parsecontext, "check_seek_to_tag.conf") );
  
  printf("config: %s\n", parsecontext->configfile_path);
  ck_assert( cpw_parsecontext_seek_to_tag(parsecontext, "seektag") );
  ck_assert_str_eq( parsecontext->linetoken->token[0], "<seektag>");

  cpw_parsecontext_done(&parsecontext);
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
  cpwcontext *context;

  context = cpw_context_new();
  cpw_context_init(context);

  ck_assert( cpw_config_init(context->config, "valid_syntax_config.conf") );

  parsecontext = cpw_parsecontext_new();
  cpw_parsecontext_init(parsecontext, "valid_syntax_config.conf");

  ck_assert( cpw_config_validate_configfile_syntax(context->config, parsecontext) );
  ck_assert( context->config->command == NULL);
  ck_assert( context->config->process == NULL);

  cpw_parsecontext_done(&parsecontext);
  parsecontext = cpw_parsecontext_new();
  cpw_parsecontext_init(parsecontext, "invalid_syntax_config.conf");

  ck_assert( ! cpw_config_validate_configfile_syntax(context->config, parsecontext) );

}
END_TEST

START_TEST (test_config_cpw_config_validate_configfile_logic)
{
  /* unit test code */
  cpwparsecontext *parsecontext;
  cpwcontext *context;

  context = cpw_context_new();
  cpw_context_init(context);

  ck_assert( cpw_config_init(context->config, "valid_logic_config.conf") );

  parsecontext = cpw_parsecontext_new();
  ck_assert( cpw_parsecontext_init(parsecontext, "valid_logic_config.conf") );

  ck_assert( cpw_config_validate_configfile_logic(context->config, parsecontext) );
  ck_assert( context->config->command != NULL);
  ck_assert( context->config->process != NULL);

  cpw_parsecontext_done(&parsecontext);

  parsecontext = cpw_parsecontext_new();
  cpw_parsecontext_init(parsecontext, "invalid_logic_config.conf");

  ck_assert( ! cpw_config_validate_configfile_logic(context->config, parsecontext) );

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
    tcase_add_test(tc_core, test_config_cpw_tag_is_arg_allowed);
    tcase_add_test(tc_core, test_config_cpw_config_validate_configfile_syntax);
    tcase_add_test(tc_core, test_config_cpw_config_validate_configfile_logic);

    tcase_add_test(tc_core, test_config_cpw_parsecontext_new);
    tcase_add_test(tc_core, test_config_cpw_parsecontext_init);
    tcase_add_test(tc_core, test_config_cpw_parsecontext_next_token);
    tcase_add_test(tc_core, test_config_cpw_parsecontext_seek_to_tag);
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
