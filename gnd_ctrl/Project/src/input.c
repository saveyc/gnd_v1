#include"input.h"
#include "main.h"

sInput_Info   port_info[5];
u16   keep_in_paire = 0;
u16   last_keep_in_paire = 0;
u16   paire_start_time_cnt = 0;

void InputScanProc(void)
{
    u8 i;
    u8 input[2];

    
    input[0] = IN_16_STATE;
    if (port_info[0].input_state != input[0]
        && port_info[0].input_confirm_times == 0)
    {
        port_info[0].input_middle_state = input[0];
    }
    if (port_info[0].input_middle_state == input[0]
        && port_info[0].input_middle_state != port_info[0].input_state)
    {
        port_info[0].input_confirm_times++;
        if (port_info[0].input_confirm_times > 10)
        {
            port_info[0].input_state = port_info[0].input_middle_state;
            port_info[0].input_confirm_times = 0;
            if (port_info[0].input_state == 1)
            {
                port_info[0].input_trig_mode = 1;
                keep_in_paire = REPAIRE_MODEL;
                paire_start_time_cnt = 20;
            }
            else
            {
                port_info[0].input_trig_mode = 0;
                keep_in_paire = REPAIRE_NONE;
                paire_start_time_cnt = 0;
            }
        }
    }
    else
    {
        port_info[0].input_middle_state = port_info[0].input_state;
        port_info[0].input_confirm_times = 0;
    }
}


void Input_keep_in_repaire_process(void)
{
    if (keep_in_paire == REPAIRE_MODEL && paire_start_time_cnt !=0)
    {
        if (keep_in_paire != last_keep_in_paire)
        {
            last_keep_in_paire = keep_in_paire;
        }
        RS485_OUT(3000);
    }

    if (keep_in_paire == REPAIRE_MODEL && paire_start_time_cnt == 0)
    {
        if (keep_in_paire != last_keep_in_paire)
        {
            last_keep_in_paire = keep_in_paire;
        }
        RS485_OUT(1500);
    }

    if (keep_in_paire == REPAIRE_NONE && keep_in_paire != last_keep_in_paire)
    {
        last_keep_in_paire = keep_in_paire;
        RS485_OUT(0);
    }
}

