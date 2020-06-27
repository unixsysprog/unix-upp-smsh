/* process.c
 * Command processing layer: handles layers of processing
 * date: 4/15/2017
 * author: Tasuku Miura
 */
#include	"process.h"
#include	<stdio.h>
#include	<stdlib.h>
#include	<unistd.h>
#include	<signal.h>
#include    <string.h>
#include	<sys/wait.h>
#include	"builtin.h"
#include	"controlflow.h"
#include	"smsh.h"
#include	"varlib.h"


static int do_command(char **args);
static int execute(char *argv[]);

/*
 * Checks if string has any variable replacement candidates searching for $ with
 * in the provided string. Called before the processing step.
 * @args: str - Full string to search.
 * @rets: string with any variables replaced with the mapped value.
 */
char* var_sub_main(char *str)
{
    FLEXSTR fname;                      /* Placeholder for new string */
    int found;

    while (1) {
        char tmp[2048];                 /* Placeholder for string after $ */
        char *cp;
        fs_init(&fname, 0);

        if ((cp = strchr(str, '$')) == NULL)
            break;

        if (*--cp == '\\')              /* Checks whether to ignore $ */
            break;
        
        cp++;
        int i = 0;
        while (*++cp != '\0' && strchr(" ./$", *cp) == NULL) { 
            tmp[i] = *cp; i++;
        }
        tmp[i] = '\0';                  /* tmp now contains the target */

        char* val = VLlookup(tmp);      /* Look up tmp and return mapped val */
        if (strcmp(val, "") == 0)
            break;

        found = 0;
        for(cp = str; *cp; cp++) {
            if (*cp != '$' || found)
                fs_addch(&fname, *cp);
            else if (!found ) {
                fs_addstr(&fname, val);
                cp+=i;
                found = 1;
            }
        }
        fs_addch(&fname, '\0');
        str = fs_getstr(&fname);
    }
    return str;
}
/*
 * Process user command: this level handles flow control.
 * @args: array of char*
 * @rets: result of processing command
 */
int process(char *args[])
{
	int		rv = 0;

	if ( args[0] == NULL )
		rv = 0;
	else if ( is_control_command(args[0]) )
		rv = do_control_command(args);
	else if ( ok_to_execute() )
		rv = do_command(args);

	return rv;
}

static int do_command(char **args)
/* Do a command - either builtin or external
 * @args: array of char*
 * @rets: result of the command
 */
{
	void varsub(char **);
	int  is_builtin(char **, int *);
	int  rv;

	varsub(args);
	if ( is_builtin(args, &rv) )
		return rv;
	rv = execute(args);
	return rv;
}

static int execute(char *argv[])
/* Run a program passing it arguments
 * @args: argv - array of char*
 * @rets: status returned via wait, or -1 on error
 * Note: errors -1 on fork() or wait() errors
 */
{
	extern char **environ;		/* note: declared in <unistd.h>	*/
	int	pid ;
	int	child_info = -1;

	if ( argv[0] == NULL )		/* nothing succeeds		*/
		return 0;

	if ( (pid = fork())  == -1 )
		perror("fork");
	else if ( pid == 0 ){
		environ = VLtable2environ();
		signal(SIGINT, SIG_DFL);
		signal(SIGQUIT, SIG_DFL);
		execvp(argv[0], argv);
		perror("cannot execute command");
		exit(1);
	}
	else {
		if ( wait(&child_info) == -1 )
			perror("wait");
	}
	return child_info;
}
