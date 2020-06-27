/* Header file for builtin.c
 * author: Tasuku Miura
 * date: 4/15/2017
 */
#ifndef	BUILTIN_H
#define	BUILTIN_H

int is_builtin(char **args, int *resultp);
void varsub(char **args);

#endif
