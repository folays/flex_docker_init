#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>

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

int main(int argc, char **argv)
{
  signal(SIGTERM, signal_term);
  signal(SIGCHLD, signal_chld);

  while (1)
    {
      int status;

      sleep(1);

      while (wait(&status) != -1)
	;
    }
  exit (0);
}
