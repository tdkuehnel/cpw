TESTS = check_context check_config check_process check_command check_helper
check_PROGRAMS = check_context check_config check_process check_command check_helper

check_context_SOURCES = check_context.c $(top_builddir)/src/context.h ../src/log.c ../src/context.c ../src/config.c ../src/command.c ../src/helper.c ../src/process.c ../src/commandarg.c
check_context_CFLAGS = @CHECK_CFLAGS@
check_context_LDADD = @CHECK_LIBS@

check_config_SOURCES = check_config.c $(top_builddir)/src/config.h ../src/context.c ../src/config.c ../src/log.c ../src/process.c ../src/command.c ../src/helper.c ../src/commandarg.c
check_config_CFLAGS = @CHECK_CFLAGS@
check_config_LDADD = @CHECK_LIBS@

check_process_SOURCES = check_process.c $(top_builddir)/src/process.h ../src/log.c ../src/process.c ../src/commandarg.c
check_process_CFLAGS = @CHECK_CFLAGS@
check_process_LDADD = @CHECK_LIBS@

check_command_SOURCES = check_command.c $(top_builddir)/src/command.h ../src/log.c ../src/command.c ../src/helper.c ../src/commandarg.c
check_command_CFLAGS = @CHECK_CFLAGS@
check_command_LDADD = @CHECK_LIBS@

check_helper_SOURCES = check_helper.c $(top_builddir)/src/helper.h ../src/log.c ../src/helper.c 
check_helper_CFLAGS = @CHECK_CFLAGS@
check_helper_LDADD = @CHECK_LIBS@

