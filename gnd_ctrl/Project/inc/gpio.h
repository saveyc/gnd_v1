#ifndef __GPIO_H
#define __GPIO_H

#include "main.h"

#define  PORT_NUM           6

#define  INPUT_TRIG_NULL    0
#define  INPUT_TRIG_UP      1
#define  INPUT_TRIG_DOWN    2

#define  LIGHT_OFF          0
#define  LIGHT_FLASH_FAST   1
#define  LIGHT_FLASH_SLOW   2
#define  LIGHT_ON           3

#define  LOW_LEVEL          0
#define  HIGH_LEVEL         1    

#pragma pack (1) 

typedef struct {
    u8  input_state;
    u8  input_middle_state;
    u16 input_confirm_times;
    u8  input_trig_mode; //null,up,down
    u8  level;
}sInput_Info;

typedef struct {
    sInput_Info  cnt_photo;
    sInput_Info  rst_photo;
}sUnload_Port_Info;



#pragma pack () 

extern u16 position_cnt ;
extern u16 position_remain;
extern u16 position_delay;
extern u16 position_servo;
extern u32 position_speed;

void InputScanProc(void);
void position_ouput_out(void);




#endif