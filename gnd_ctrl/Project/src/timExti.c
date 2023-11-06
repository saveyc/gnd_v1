#include "main.h"
#include "math.h"

TIM_EXTI_STRUCT timExtiHandler = {0, 0};
u32 timUpdates = 0;
//u16 exti_cnt = 0;
u8  adjust_stage_flag = 0;
u16 first_stage_cnt = 0;
u16 first_stage_hz = 0;
u16 second_stage_cnt = 0;
u16 second_stage_hz = 0;
//u16 stop_time_delay = 0;
//u32 speed_value[6];

void timExtiRst(void)
{
    timUpdates = 0;
    timExtiHandler.extid = 1;
    timExtiHandler.timCnt = TIM_GetCounter(TIM2);
}
//u32 speed_average(u16 extid,u32 speed)
//{
//    u32 speed_avg = 0;
//    u8  i;
//    
//    if(extid<2 || extid>7) return speed;
//    
//    speed_value[extid-2] = speed;
//    
//    for(i=0 ; i<6 ; i++)
//    {
//        speed_avg += speed_value[i];
//    }
//    speed_avg = speed_avg/6;
//    
//    return speed_avg;
//}
//计算当前位置到目标位置需要走多少个车
u16 cal_move_distance(void)
{
    u16 curPos = gnd2WcsCmdData.carNo;
    u16 targetPos = repair_locate_real;
    u16 car_tot_num = repairCarCmdData.car_tot_num;
    u16 rVal = 0;
    
    if(curPos <= targetPos)
    {
        rVal = targetPos - curPos;
    }
    else
    {
        rVal = car_tot_num - curPos + targetPos;
    }
    
    return rVal;
}
//计算当前位置离目标位置有多少个车(取最短距离)
u16 cal_distance_to_real_locate(void)
{
    u16 curPos = gnd2WcsCmdData.carNo;
    u16 targetPos = repair_locate_real;
    u16 rVal = 0;
    
    if(curPos <= targetPos)
    {
        rVal = targetPos - curPos;
    }
    else
    {
        rVal = curPos - targetPos;
    }
    
    return rVal;
}
void repair_car_para_init(void)
{
    repair_flag = 1;
    
    if((repairCarCmdData.repair_locate+repairCarCmdData.car_no-1) <= repairCarCmdData.car_tot_num)
    {
        repair_locate_real = repairCarCmdData.repair_locate+repairCarCmdData.car_no-1;
    }
    else
    {
        repair_locate_real = (repairCarCmdData.repair_locate+repairCarCmdData.car_no-1)-repairCarCmdData.car_tot_num;
    }
    
    if(reset_flag==1)//车号已经复位,计数是准确值
    {
        if(cal_distance_to_real_locate() > 2)
        {
            if (keep_in_paire == REPAIRE_NONE) {
                RS485_OUT(repairCarCmdData.standard_hz);
            }
        }
        else//跟目标位置小于两个车无需调节
        {
            repair_flag = 0;
        }
    }
    else
    {
        if (keep_in_paire == REPAIRE_NONE) {
            RS485_OUT(repairCarCmdData.standard_hz);
        }
    }
}
void repairLocationCtrl(void)
{
    u16 targetDistance;
    u16 outputHz;
    const u16 maxHz = 3000;//30.00hz
    
    if(reset_flag==0||repair_flag==0) return;
    
    targetDistance = cal_move_distance();
    
    if(targetDistance > 100)
    {
        outputHz = maxHz;
    }
    else if(targetDistance > 50)
    {
        outputHz = (repairCarCmdData.standard_hz + maxHz)/2;
    }
    else if(targetDistance > 2)
    {
        outputHz = repairCarCmdData.standard_hz;
    }
    else
    {
        outputHz = 0;
        repair_flag = 0;
    }
    if (keep_in_paire == REPAIRE_NONE) {
        RS485_OUT(outputHz);
    }
}
void TIM_EXTI_Process(u16 extid)
{
    u32 cnt;
    
    if(repair_flag == 1) return;//维护模式不使用PID调速
    
    if(servo_start == 0)
    {
        if(extid == 1)
        {
            if(adjust_stage_flag == 0)
            {
                first_stage_cnt++;
                if(first_stage_cnt >= 12)
                {
                    first_stage_cnt = 0;
                    if(second_stage_hz != 0)
                    {
                        adjust_stage_flag = 1;
                        fwInitHz = second_stage_hz;
                    }
                    else
                    {
                        servo_start = 1;
                    }
                }
            }
            else if(adjust_stage_flag == 1)
            {
                second_stage_cnt++;
                if(second_stage_cnt >= 60)
                {
                    adjust_stage_flag = 0;
                    second_stage_cnt = 0;
                    servo_start = 1;
                    fwInitHz = first_stage_hz;
                }
            }
        }
        //return;
    }
    
    if(timExtiHandler.extid == 0)
    {
        if(extid == 1)
        {
            timExtiRst();
        }
    }
    else
    {
        if(extid == 1)
        {
            timExtiRst();
            return;
        }
        if(extid == timExtiHandler.extid + 1) //必须是顺序的两个光电开关
        {
            cnt = (TIM_GetCounter(TIM2) + 0xFFFF * timUpdates) - (timExtiHandler.timCnt);
            timUpdates = 0;
            timExtiHandler.extid = extid;
            timExtiHandler.timCnt = TIM_GetCounter(TIM2);
            if(servo_start == 1)
            {
                fwPIDctrl(cnt * 50);
            }
            //UartSendPidCnt(extid, cnt);//DEBUG
            gnd2WcsCmdData.speed = cnt;
        }
        else
        {
            timExtiHandler.extid = 0;
            return;
        }
    }
}