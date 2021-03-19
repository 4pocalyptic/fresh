
#include <linux/limits.h>
#include <unistd.h>
#include <stdio.h>
#include <readline/readline.h>
#include <errno.h>
#include <x86_64-linux-gnu/bits/local_lim.h>

#include "parser.h"
#include "builtin.h"

/* Der "Programmname", wie in der main Funktion in argv[0] übergeben */
char *program_name = NULL;

int
do_fork_wait (int argc, char *argv[])
{
  /****************************************************************************
   * fork / exec                                                              *
   * -----------                                                              *
   *                                                                          *
   *            ...                    Der fork Systemaufruf erzeugt in UNIX  *
   *             |                     einen neuen Prozess, indem er eine     *
   *             v                     Kopie des laufenden Prozesses erzeugt. *
   *      fork() * ---- x              Beide Kopien setzen ihre Ausführung    *
   *             |      |              unmittelbar nach dem fork Aufruf fort. *
   *    pid != 0 |      | pid == 0                                            *
   *             v      v              Im neu erstellten Kindprozess ist der  *
   *      wait() *      * exec(...)    Rückgabewert von fork 0, während im    *
   *            ---     |              Elternprozess die Prozess ID des neuen *
   *                   ...             Prozesses zurückgegeben wird.          *
   *                    |                                                     *
   *                    v              Führt man im Kindprozess den System-   *
   *                    * exit()       aufruf exec* aus, ersetzt man das      *
   *            ---    ---             Programm des laufenden Prozesses durch *
   *             |                     ein neu geladenes Programm. Durch die  *
   *             v                     Kombination von fork und exec kann die *
   *            ...                    shell neue Programme starten.          *
   *                                                                          *
   *  Der Systemaufruf wait im Elternprozess bewirkt, dass die shell das Ende *
   *  des neu gestarteten Prozesses abwartet, bevor die Ausführung fort-      *
   *  gesetzt wird. wait wird darüber hinaus dem Betriebssystem vermitteln,   *
   *  dass die Informationen des beendeten Kindprozesses aufgeräumt, und      *
   *  dessen Prozess ID wiederverwendet werden kann. Lesen Sie dazu auch die  *
   *  Manual-Seite `man 2 wait`.                                              *
   ****************************************************************************/

  // Try fork
  pid_t child, p;
  int status;
  child = fork();
  if (child == (pid_t)-1) {
    fprintf(stderr, "Cannot fork: %s.\n", strerror(errno));
    return 1;
  }

  // Child job
  if(!child) {
    errno = ENOENT;
    execvp(argv[0], argv);
    fprintf(stderr, "%s: %s.\n", program_name, strerror(errno));
    exit(127); // always CNF ??
  }

  // wait for child
  do {
    p = waitpid(child, &status, 0);
  } while (p == (pid_t)-1 && errno == EINTR);
  if (p == (pid_t)-1) {
    fprintf(stderr, "Lost child process: %s.\n", strerror(errno));
    return 127; // CNF ??
  }
  if (p != child) {
    fprintf(stderr, "waitpid() library bug occurred.\n");
    return 127; // CNF ??
  }

  // check child status
  if (WIFEXITED(status)) {
    if (WEXITSTATUS(status))
      fprintf(stderr, "Command failed with exit status %d.\n", WEXITSTATUS(status));
    return WEXITSTATUS(status);
  }
  if (WIFSIGNALED(status)) {
    fprintf(stderr, "Command died by signal %s.\n", strsignal(WTERMSIG(status)));
    return 126; // Command invoked cannot execute
  }
  fprintf(stderr, "Command died from unknown causes.\n");
  return 125;
}

int
main (int argc, char *argv[])
{
  program_name = argv[0];

  char *input = NULL;
  size_t nwords;
  char **words = NULL;
  while (1)
    {
      char shell_pre[PATH_MAX+32+HOST_NAME_MAX] = "";
      char *uid, *home;
      char hostname[HOST_NAME_MAX];
      int hngetreturn = gethostname(hostname, HOST_NAME_MAX);
      if((uid = getenv("USER")) != NULL) {
        strcat(shell_pre, uid);
        if(hngetreturn == 0) {
          strcat(shell_pre, "@");
          strcat(shell_pre, hostname);
        }
        strcat(shell_pre, ":");
      } else {
        strcat(shell_pre, hostname);
        strcat(shell_pre, ":");
      }
      char cwd[PATH_MAX];
      if (getcwd(cwd, sizeof(cwd)) != NULL) {
        if((home = getenv("HOME")) != NULL) {
	  char *tmp;
	  tmp = (char *) str_repl(home, cwd, "~");
	  if(tmp != NULL) {
            printf("DEBUG: %i\n", tmp);
            printf("DEBUG: %s\n", tmp);
//            strcat(shell_pre, tmp);
//            free(tmp);
          } else {
            printf("DEBUG: %s\n", cwd);
//            strcat(shell_pre, cwd);
          }
        } else {
          strcat(shell_pre, cwd);
        }
        strcat(shell_pre, "> ");
      }
      input = readline(shell_pre);

      if (input == NULL)
        {
          puts("exit");
          break;
        }

      words = parse(input, &nwords);

      if (nwords == 0)
        continue;

      builtin_func_t builtin = get_builtin(words[0]);
      if (builtin != NULL)
        {
          builtin(nwords, words);
        }
      else
        {
          do_fork_wait(nwords, words);
        }

      free(words);
      free(input);
    }

  return 0;
}
