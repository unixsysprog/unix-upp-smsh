/*
 * smsh - small-shell version 5
 * Based off of small-shell version 4 provided by Bruce Molay.
 * date: 4/15/2017
 * author: Tasuku Miura
 */
#include    <ctype.h>
#include    <stdio.h>
#include    <stdlib.h>
#include    <unistd.h>
#include    <signal.h>
#include    <sys/wait.h>
#include    "flexstr.h"
#include    "process.h"
#include    "smsh.h"
#include    "splitline.h"
#include    "string.h"
#include    "varlib.h"


#define	DFL_PROMPT	"> "

void	setup();


int main(int argc, char** argv)
{
	char	*cmdline, *prompt, **arglist;
	int	result;
    FILE *fp;

    if (argc > 1) {
        if ((fp = fopen(argv[1], "r")) == NULL) {
            perror("Cant open file:");
            exit(EXIT_FAILURE);
        }
        prompt = NULL;
    } else {
        fp = stdin;
        prompt = DFL_PROMPT ;
    }
    
	setup();
	while ((cmdline = next_cmd(prompt, fp)) != NULL) {
        char* newcmd = var_sub_main(cmdline);

        if (strcmp(newcmd,"") == 0) {
            arglist = splitline(cmdline);
        } else {
            arglist = splitline(newcmd);
        }

		if (arglist != NULL) {
			result = process(arglist);
			freelist(arglist);
		}
		free(cmdline);
	}
	return result;
}

void setup()
/* Initialize shell */
{
	extern char **environ;

	VLenviron2table(environ);
	signal(SIGINT,  SIG_IGN);
	signal(SIGQUIT, SIG_IGN);
}

void fatal(char *s1, char *s2, int n)
/* Exits with status n. */
{
	fprintf(stderr,"Error: %s,%s\n", s1, s2);
	exit(n);
}
