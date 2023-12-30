/*
 * print -- loadable ksh93-style print builtin
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

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

#include "bash/builtins/bashgetopt.h"
#include "bash/builtins/builtext.h"
#include "bash/builtins/common.h"

#if !defined(errno)
extern int errno;
#endif

int myprint_builtin(WORD_LIST *list);
static int printargs(WORD_LIST *list, FILE *ofp);

extern char *this_command_name;
static FILE *ofp;

static char
*myprint_doc[] =
{
  "Display arguments.",
  "",
  "Output the arguments.  The -f option means to use the argument as a",
  "format string as would be supplied to printf(1).  The rest of the",
  "options are as in ksh.",
  (char *)NULL
};

struct builtin
myprint_struct =
{
  "myprint",
  myprint_builtin,
  BUILTIN_ENABLED,
  myprint_doc,
  "myprint [-Rnprs] [-u unit] [-f format] [arguments]",
  (char *)0
};

#if !defined(ISOPTION)
# define ISOPTION(s, c) (s[0] == '-' && s[2] == '\0' && s[1] == c)
#endif /* ifndef ISOPTION */

int
myprint_builtin(WORD_LIST *list)
{
  char      *pfmt = 0;
  int       c;
  int       nflag = 0;
  int       ofd = 1;
  int       r;
  int       raw = 0;
  intmax_t  lfd;
  WORD_LIST *l;

  reset_internal_getopt();

  while ((c = internal_getopt(list, "Rnpru:f:")) != -1) {
    switch (c) {
      case 'R':
        raw = 2;
        loptend = lcurrent;
        if (loptend && ISOPTION(loptend->word->word, 'n')) {
          loptend = loptend->next;
          nflag = 1;
        }
        goto opt_end;

      case 'r':
        raw = 1;
        break;

      case 'n':
        nflag = 1;
        break;

      case 'p':
        break; /* NOP */

      case 'u':
        if (all_digits(list_optarg) && legal_number(list_optarg, &lfd) && lfd == (int)lfd) {
          ofd = lfd;
        } else {
          for (l = list; l->next && l->next != lcurrent; l = l->next);
          lcurrent = loptend = l;
          goto opt_end;
        }
        break;

      case 'f':
        pfmt = list_optarg;
        break;

      CASE_HELPOPT;

      default:
        builtin_usage();
        return EX_USAGE;
    }
  }

opt_end:
  list = loptend;

  ofp = (ofd == 1) ? stdout : fdopen (dup (ofd), "w");

  if (pfmt) {
    WORD_DESC *w;
    WORD_LIST *nlist;

    w           = make_word(pfmt);
    nlist       = make_word_list(w, list);
    r           = printf_builtin(nlist);
    nlist->next = (WORD_LIST *)NULL;
    dispose_words(nlist);

    return r;
  }

  if (raw) {
    for (l = list; l; l = l->next) {
      fprintf(ofp, "%s", l->word->word);
      if (l->next)
        fprintf(ofp, " ");
    }

    if (nflag == 0)
      fprintf(ofp, "\n");
    fflush(ofp);

    return 0;
  }

  r = printargs(list, ofp);

  if (r && nflag == 0)
    fprintf(ofp, "\n");

  if (ofd != 1)
    fclose(ofp);

  return 0;
}

static int
printargs(WORD_LIST *list, FILE *ofp)
{
  char      *ostr;
  int       sawc;
  WORD_LIST *l;

  for (sawc = 0, l = list; l; l = l->next) {
    ostr = ansicstr(l->word->word, strlen(l->word->word), 0, &sawc, (int *)0);

    if (ostr)
      fprintf(ofp, "%s", ostr);

    free(ostr);
    ostr = 0;

    if (sawc)
      return 0;

    if (l->next)
      fprintf(ofp, " ");
  }

   return 1;
}
