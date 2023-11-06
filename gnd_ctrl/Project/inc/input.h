#ifndef _INPUT_H
#define _INPUT_H
#include "stm32f10x.h"

#define REPAIRE_NONE  0
#define REPAIRE_MODEL 1

typedef struct {
    u16  input_state;
    u16  input_middle_state;
    u16  input_confirm_times;
    u16  input_trig_mode; //null,up,down
}sInput_Info;

extern sInput_Info   port_info[];

extern u16   keep_in_paire;
extern u16   last_keep_in_paire;
extern u16   paire_start_time_cnt;

void Input_keep_in_repaire_process(void);
void InputScanProc(void);

#endif