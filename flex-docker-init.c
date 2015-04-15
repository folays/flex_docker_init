#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

#define FLEX_TERM_WAIT_TIME 5

static void _wait_reap_all(void)
{
  int start_time;

  start_time = time(NULL);
  while (kill(-1, 0) != -1 && time(NULL) - FLEX_TERM_WAIT_TIME < start_time)
    {
      int status;

      // reap all process, probably killed by SIGTERM above.
      while (wait(&status) != -1)
	;

      usleep(500000);
    }
}

static void signal_term(int sig)
{
  kill(-1, SIGTERM);
  _wait_reap_all();

  kill(-1, SIGKILL);
  _wait_reap_all();

  signal(SIGTERM, SIG_DFL);
  kill(getpid(), SIGTERM);

  exit(1);
}

static void signal_chld(int sig)
{
  // exists only so that sleep(1) is awaken as soon as a children needs to be reap.
}

static void run_rc(int argc, char **argv)
{
  int pid;

  if ((pid = fork()) < 0)
    err(1, "fork");

  if (pid)
    return;

  char **argv_new;
  {
    int i;

    argv_new = calloc(argc + 1 + 1, sizeof(*argv_new));
    if (!argv_new)
      err(1, "calloc");

    argv_new[0] = "/bin/sh";
    argv_new[1] = "/etc/rc.local.flex-docker";
    for (i = 1; i <= argc; ++i)
      {
	argv_new[i + 1] = argv[i];
      }
  }

  execv(argv_new[0], argv_new);
  exit(0); /* if the rc file doesn't exist */
}

int main(int argc, char **argv)
{
  signal(SIGTERM, signal_term);
  signal(SIGCHLD, signal_chld);

  run_rc(argc, argv);

  while (1)
    {
      int status;

      sleep(1);

      while (wait(&status) != -1)
	;
    }
  exit (0);
}
