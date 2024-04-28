#ifndef _DBGSHELL_H
#define _DBGSHELL_H

#include <stdint.h>

#define DBGSHELL_SIGNAL_MOUSE       0x1
#define DBGSHELL_SIGNAL_KEYBOARD    0x2
#define DBGSHELL_SIGNAL_TTY         0x3

void init_dbgshell();
void kill_dbgshell();
void ex_dbgshell(const char * command);
#endif