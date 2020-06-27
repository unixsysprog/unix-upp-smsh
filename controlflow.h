/* Header file for controlflow.c
 * controlflow.h
 * date: 4/15/2017
 * author: Tasuku Miura
 */

#ifndef CONTROLFLOW_H
#define CONTROLFLOW_H

int is_control_command(char *);
int do_control_command(char **);
int ok_to_execute();

#endif
