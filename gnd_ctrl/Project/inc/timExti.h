#ifndef _TIMEEXTI_H
#define _TIMEEXTI_H

typedef struct
{
    u16 extid;  //光电开关序号(1~7)
    u16 timCnt; //计数器值
}TIM_EXTI_STRUCT;

extern TIM_EXTI_STRUCT timExtiHandler;
extern u32 timUpdates;
extern u8  adjust_stage_flag;
extern u16 first_stage_cnt;
extern u16 first_stage_hz;
extern u16 second_stage_cnt;
extern u16 second_stage_hz;
//extern u16 stop_time_delay;

void TIM_EXTI_Process(u16 extid);
void repair_car_para_init(void);
void repairLocationCtrl(void);

#endif