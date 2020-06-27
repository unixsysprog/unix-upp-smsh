/* controlflow.c
 *
 * "if" processing is done with two state variables
 *    if_state and if_result
 */
#include    "controlflow.h"
#include	<stdio.h>
#include 	<string.h>
#include	"smsh.h"
#include	"process.h"

enum states   { NEUTRAL, WANT_THEN, THEN_BLOCK, ELSE_BLOCK };
enum results  { FAIL, SUCCESS };

static int if_state  = NEUTRAL;
static int if_result = SUCCESS;
static int last_state = 0;

static int syn_err(char *);

/* 
 : Determine the shell should execute a command
 * @rets: 1 for yes, 0 for no
 * Note: if in THEN_BLOCK and if_result was SUCCESS then yes
 *       if in THEN_BLOCK and if_result was FAIL    then no
 *       if in WANT_THEN  then syntax error (sh is different)
 */
int ok_to_execute()
{
	int	rv = 1;		/* default is positive */

	if (if_state == WANT_THEN) {
		syn_err("not in block");
		rv = 0;
	}
	else if (if_state == THEN_BLOCK && if_result == SUCCESS)
		rv = 1;
	else if (if_state == THEN_BLOCK && if_result == FAIL)
		rv = 0;
	else if (if_state == ELSE_BLOCK && if_result == FAIL)
        rv = 1;
	else if (if_state == ELSE_BLOCK && if_result == SUCCESS)
        rv = 0;

	return rv;
}


/*
 * Boolean to report if the command is a shell control command
 * @args: s - string to check if control command.
 * @rets: 0 or 1
 */
int is_control_command(char *s)
{
    return (strcmp(s,"if")==0
            || strcmp(s,"then")==0
            || strcmp(s, "else")==0 
            || strcmp(s,"fi")==0);
}


/*
 * Process "if", "then", "fi" - change state or detect error
 * @args: args - array of char*
 * @rets: if ok, -1 for syntax error
 */
int do_control_command(char **args)
{
	char	*cmd = args[0];
	int	rv = -1;

	if(strcmp(cmd,"if")==0){
		if (if_state != NEUTRAL)
			rv = syn_err("if unexpected");
		else {
			last_state = process(args+1);
			if_result = (last_state == 0 ? SUCCESS : FAIL );
            if_state = WANT_THEN; 
			rv = 0;
		}
	}
	else if (strcmp(cmd,"then")==0) {
        if (if_state != WANT_THEN)
            rv = syn_err("then unexpected");
        else {
            if_state = THEN_BLOCK;
            rv = 0;
        }
	}
    else if (strcmp(cmd,"else")==0) {
        if (if_state != THEN_BLOCK)
            rv = syn_err("else unexpected");
        else {
            if_state = ELSE_BLOCK;
            rv = 0;
        }
    }
	else if (strcmp(cmd,"fi")==0) {
		if (if_state != ELSE_BLOCK)
			rv = syn_err("fi unexpected");
		else {
			if_state = NEUTRAL;
			rv = 0;
		}
	}
	else 
		fatal("internal error processing:", cmd, 2);
	return rv;
}

static int syn_err(char *msg)
/* Handles syntax errors in control structures
 * @args: msg - message to print to stderr.
 * @rets: -1 in interactive mode. Should call fatal in scripts
 * Note:  resets state to NEUTRAL
 */
{
	if_state = NEUTRAL;
	fprintf(stderr,"syntax error: %s\n", msg);
	return -1;
}
