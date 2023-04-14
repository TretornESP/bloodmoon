#include "line_discipline.h"
#include "../util/printf.h"
int ld_ignore(char c) {
    return 0;
}

int ld_bell(char c) {
    return 0;
    printf("A bell is ringing...\n");
    //TODO: implement bell
}

struct line_discipline_action_table_entry ld_table[] = {
    {.output = LD_NULL, .action = ld_ignore},   //0
    {.output = LD_NULL, .action = ld_ignore},   //1 A
    {.output = LD_NULL, .action = ld_ignore},   //2 B 
    {.output = LD_NULL, .action = ld_sigint},   //3 C 
    {.output = LD_NULL, .action = ld_eof},      //4 D 
    {.output = LD_NULL, .action = ld_ignore},   //5 E 
    {.output = LD_NULL, .action = ld_ignore},   //6 F
    {.output = LD_NULL, .action = ld_bell},     //7 G
    {.output = LD_NULL, .action = ld_erase},   //8 H
    {.output = LD_NULL, .action = ld_htab},   //9 I
    {.output = LD_NULL, .action = ld_lfeed},   //10 J
    {.output = LD_NULL, .action = ld_vtab},   //11 K
    {.output = LD_NULL, .action = ld_newpage},   //12 L
    {.output = LD_NULL, .action = ld_cret},   //13 M
    {.output = LD_NULL, .action = ld_ignore},   //14 N
    {.output = LD_NULL, .action = ld_ignore},   //15 O 
    {.output = LD_NULL, .action = ld_ignore},   //16 P
    {.output = LD_NULL, .action = ld_restart_out},   //17 Q 
    {.output = LD_NULL, .action = ld_ignore},   //18 R 
    {.output = LD_NULL, .action = ld_stop_out},   //19 S 
    {.output = LD_NULL, .action = ld_ignore},   //20 T
    {.output = LD_NULL, .action = ld_ignore},   //21 U
    {.output = LD_NULL, .action = ld_ignore},   //22 V
    {.output = LD_NULL, .action = ld_ignore},   //23 W 
    {.output = LD_NULL, .action = ld_ignore},   //24 X 
    {.output = LD_NULL, .action = ld_ignore},   //25 Y
    {.output = LD_NULL, .action = ld_sigstop},   //26 Z
    {.output = LD_NULL, .action = ld_del},   //27

};

