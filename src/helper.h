#ifndef __CPW_HELPER_H__
#define __CPW_HELPER_H__

#define CPW_CONFIG_MAX_LINE_TOKEN 5
#define CPW_CONFIG_MAX_TAG_LENGTH 64

typedef struct cpwlinetoken {
  /* Actual an array of char * of size MAX_NUM_LINE_TOKEN*/
  char *token[CPW_CONFIG_MAX_LINE_TOKEN];
  int is_tag;
  int tag_name_index;
  int is_opening_tag;
  int is_closing_tag;
  int num;
} cpwlinetoken;

int cpw_is_space(int c);
int cpw_is_tag(const char *tag);
int cpw_is_closing_tag(const char *arg);
int cpw_is_opening_tag(const char *arg);
void cpw_get_arg(char *buf, int buf_size, const char **pp);
void cpw_get_tag(char *buf, int buf_size, char **pp);
int cpw_find_token(const char* line, const char* token, int index);
void cpw_split_line( const char *line, cpwlinetoken *linetoken);
int cpw_get_tag_name_index(cpwlinetoken *linetoken);

#endif
