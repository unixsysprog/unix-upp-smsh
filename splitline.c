/* splitline.c - commmand reading and parsing functions for smsh
 *    
 *    char *next_cmd(char *prompt, FILE *fp) - get next command
 *    char **splitline(char *str);           - parse a string
 */

#include	<stdio.h>
#include	<stdlib.h>
#include	<string.h>
#include	"splitline.h"
#include	"smsh.h"
#include	"flexstr.h"

char * next_cmd(char *prompt, FILE *fp)
/*
 * purpose: read next command line from fp
 * returns: dynamically allocated string holding command line
 *  errors: NULL at EOF (not really an error)
 *          calls fatal from emalloc()
 *   notes: allocates space in BUFSIZ chunks.  
 */
{
	int	c;				/* input char		*/
	FLEXSTR	s;				/* the command		*/
	int	pos = 0;

	fs_init(&s, 0);				/* initialize the str	*/
	printf("%s", prompt);				/* prompt user	*/
	while( ( c = getc(fp)) != EOF ) 
	{
		/* end of command? */
		if ( c == '\n' )
			break;

		/* no, add to buffer */
		fs_addch(&s, c);
		pos++;
	}
	if ( c == EOF && pos == 0 )		/* EOF and no input	*/
		return NULL;			/* say so		*/
	fs_addch(&s, '\0');			/* terminate string	*/
	return fs_getstr(&s);
}

/**
 **	splitline ( parse a line into an array of strings )
 **/
#define	is_delim(x) ((x)==' '||(x)=='\t')

char ** splitline(char *line)
/*
 * purpose: split a line into array of white-space separated tokens
 * returns: a NULL-terminated array of pointers to copies of the tokens
 *          or NULL if line is NULL.
 *          (If no tokens on the line, then the array returned by splitline
 *           contains only the terminating NULL.)
 *  action: traverse the array, locate strings, make copies
 *    note: strtok() could work, but we may want to add quotes later
 */
{
	char	*newstr();
	char	*cp = line;			/* pos in string	*/
	char	*start;
	int	len;
	FLEXLIST strings;

	if ( line == NULL )			/* handle special case	*/
		return NULL;

	fl_init(&strings,0);

	while( *cp != '\0' )
	{
		while ( is_delim(*cp) )		/* skip leading spaces	*/
			cp++;
		if ( *cp == '\0' )		/* end of string? 	*/
			break;			/* yes, get out		*/

		/* mark start, then find end of word */
		start = cp;
		len   = 1;
		while (*++cp != '\0' && !(is_delim(*cp)) )
			len++;
		fl_append(&strings, newstr(start, len));
	}
	fl_append(&strings, NULL);
	return fl_getlist(&strings);
}

/*
 * purpose: constructor for strings
 * returns: a string, never NULL
 */
char *newstr(char *s, int l)
{
	char *rv = emalloc(l+1);

	rv[l] = '\0';
	strncpy(rv, s, l);
	return rv;
}

void 
freelist(char **list)
/*
 * purpose: free the list returned by splitline
 * returns: nothing
 *  action: free all strings in list and then free the list
 */
{
	char	**cp = list;
	while( *cp != NULL )
		free(*cp++);
	free(list);
}

void * emalloc(size_t n)
{
	void *rv ;
	if ( (rv = malloc(n)) == NULL )
		fatal("out of memory","",1);
	return rv;
}
void * erealloc(void *p, size_t n)
{
	void *rv;
	if ( (rv = realloc(p,n)) == NULL )
		fatal("realloc() failed","",1);
	return rv;
}

