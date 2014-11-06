#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>

static void signal_term(int sig)
{
  kill(-1, SIGTERM);
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
