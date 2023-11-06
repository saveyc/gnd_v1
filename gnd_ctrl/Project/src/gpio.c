#include "main.h"
#include "gpio.h"

sUnload_Port_Info port_info[4];

u16 position_cnt = 0;
u16 position_remain = 8;
u16 position_delay = 0;
u16 position_servo = 0;
u32 position_speed = 0;
void gpio_upload_position_process(void);

void InputScanProc(void)
{
    u8 i;
    u8 input[2];
   
    input[0] = EXT6_STATE;
    input[1] = EXT10_STATE;

    if (port_info[0].cnt_photo.input_state != input[0]
        && port_info[0].cnt_photo.input_confirm_times == 0)
    {
        port_info[0].cnt_photo.input_middle_state = input[0];
    }
    if (port_info[0].cnt_photo.input_middle_state == input[0]
        && port_info[0].cnt_photo.input_middle_state != port_info[0].cnt_photo.input_state)
    {
        port_info[0].cnt_photo.input_confirm_times++;
        if (port_info[0].cnt_photo.input_confirm_times > 3)//按钮消抖时间2s
        {
            port_info[0].cnt_photo.input_state = port_info[0].cnt_photo.input_middle_state;
            port_info[0].cnt_photo.input_confirm_times = 0;
            position_cnt++;
            gpio_upload_position_process();

        }
    }
    else
    {
        port_info[0].cnt_photo.input_middle_state = port_info[0].cnt_photo.input_state;
        port_info[0].cnt_photo.input_confirm_times = 0;
    }

    if (port_info[1].rst_photo.input_state != input[1]
        && port_info[1].rst_photo.input_confirm_times == 0)
    {
        port_info[1].rst_photo.input_middle_state = input[1];
    }
    if (port_info[1].rst_photo.input_middle_state == input[1]
        && port_info[1].rst_photo.input_middle_state != port_info[1].rst_photo.input_state)
    {
        port_info[1].rst_photo.input_confirm_times++;
        if (port_info[1].rst_photo.input_confirm_times > 3)//按钮消抖时间2s
        {
            port_info[1].rst_photo.input_state = port_info[1].rst_photo.input_middle_state;
            port_info[1].rst_photo.input_confirm_times = 0;
            position_cnt = 0;
            gnd2WcsCmdData.carNo = 0;

        }
    }
    else
    {
        port_info[1].rst_photo.input_middle_state = port_info[1].rst_photo.input_state;
        port_info[1].rst_photo.input_confirm_times = 0;
    }

    
}

void gpio_upload_position_process(void)
{
    u16 i;
    sGndCtrl2WCS_Interval_Data  interval_Data;
    sGnd2Wcs_state_node statenode;

    if (position_cnt == 0) {
        return;
    }

    if ((((position_cnt + 1)  % position_remain) == 0)) {
        position_delay = 50;
        OUTTWO_ON;
        interval_Data.PreCarNum = gnd2WcsCmdData.carNo;
        gnd2WcsCmdData.carNo++;
        interval_Data.CurCarNum = gnd2WcsCmdData.carNo;
        interval_Data.interval = interval_ms_flag;//发送光电时间间隔
        statenode.speed = position_speed;
        if (gnd2WcsCmdData.carNo > 0) {
            statenode.carnum = gnd2WcsCmdData.carNo - 1;
        }
        statenode.servo = position_servo;

        AddToGnd2WcsStateQueue(statenode);
        recv_gndpos_cnt = 50;

        AddToGnd2WcsIntervalQueue(interval_Data);

        AddSendMsgToQueue(SEND_MSG_GNDCTRL2WCS_CMD_INTERVAL_TYPE);

        AddSendMsgToQueue(SEND_MSG_GNDCTRL2WCS_CMD_TYPE);

        interval_ms_flag = 0;
        ServoFreqSet(0, 1);
        //TIM_EXTI_Process(6);
        //TIM_EXTI_Process(1);
        //repairLocationCtrl();
    }
}

void position_ouput_out(void)
{
    if (position_delay != 0) {
        position_delay--;
        if (position_delay == 0) {
            OUTTWO_OFF;
        }
    }
}