#include "bash/config.h"

#if defined(HAVE_UNISTD_H)
# include <unistd.h>
#endif

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "bash/bashansi.h"
#include "bash/builtins.h"
#include "bash/shell.h"

#include "bash/builtins/common.h"
#include "bash/builtins/bashgetopt.h"

int
hello_builtin(WORD_LIST *list)
{
  SHELL_VAR *greeting = find_variable("GREETING");
  if (greeting)
    printf ("%s\n", get_variable_value (greeting));
  return EXECUTION_SUCCESS;
}

int
hello_builtin_load(char *name)
{
  bind_global_variable("GREETING", "Hello, world?", 0);
  return 1;
}

void
hello_builtin_unload(char *name)
{
}

char
*hello_doc[] =
{
  "Short description.",
  ""
  "Longer description of builtin and usage.",
  (char *)NULL
};

struct builtin
hello_struct =
{
  "hello",         /* Builtin name                        */
  hello_builtin,   /* Function implementing the builtin   */
  BUILTIN_ENABLED, /* Initial flags for builtin           */
  hello_doc,       /* Array of long documentation strings */
  "hello",         /* Usage synopsis; becomes short_doc   */
  0                /* Reserved                            */
};
