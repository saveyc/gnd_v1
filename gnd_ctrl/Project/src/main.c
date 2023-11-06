/**
  ******************************************************************************
  * @file    main.c
  * @author  MCD Application Team
  * @version V1.0.0
  * @date    11/20/2009
  * @brief   Main program body
  ******************************************************************************
  * @copy
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2009 STMicroelectronics</center></h2>
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32_eth.h"
#include "netconf.h"
#include "main.h"
#include "TCPclient.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define SYSTEMTICK_PERIOD_MS  10

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
__IO uint32_t LocalTime = 0; /* this variable is used to create a time reference incremented by 10ms */
uint32_t timingdelay;
u16 sec_reg = 100;
u16 sec_flag = 0;
u16 interval_ms_flag = 0;

u16 fwInitHz = 0;
u16 servo_hz = 0;
u8  servo_start = 0;
u8  heart_dely = 0;
u8  reset_flag = 0;
u8  repair_flag = 0;
u16 repair_locate_real = 0;
u8  inverter_type = 0;//±‰∆µ∆˜¿‡–Õ
u8  msg_send_delay = 10;
u8  version[4] = { 0 };



/* Private function prototypes -----------------------------------------------*/
void System_Periodic_Handle(void);

void scan_dip_state(void)
{
    u8 tmp, i;
    u8 dip_value;

    tmp = 0;
    for(i = 0; i < 20; i++)
    {
        if( DIP1_STATE )
        {
            tmp |= 0x01;
        }
        if( DIP2_STATE )
        {
            tmp |= 0x02;
        }
        if( DIP3_STATE )
        {
            tmp |= 0x04;
        }
        if( dip_value != tmp )
        {
            dip_value = tmp;
            i = 0;
        }
    }
    inverter_type = (~dip_value)&0x7;
    version[0] = 12;
    version[1] = 0;
    version[2] = 0;
    version[3] = 2;
//    inverter_type = 4;
}
//1s
void sec_process(void)
{
    //u8 i;

    if( sec_flag == 1 )
    {
        sec_flag = 0;

        if( heart_dely != 0 )
        {
            heart_dely--;
        }
        if( LED_STATE )
        {
            LED_ON;
        }
        else
        {
            LED_OFF;
        }
        if(servo_start == 0 && repair_flag == 0)
        {
            RS485_OUT(fwInitHz);
        }
//        ServoFreqSet(350, 0);
//        ServoFreqSet(4000);
//        if(msg_send_delay != 0)
//        {
//            msg_send_delay--;
//            if(msg_send_delay == 0)
//            {
//                AddSendMsgToQueue(SEND_MSG_GNDCTRL2WCS_CMD_TYPE);
//                msg_send_delay = 10;
//            }
//        }
    }
}
/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Main program.
  * @param  None
  * @retval None
  */
int main(void)
{
    System_Setup();
    InitSendMsgQueue();
    InitGnd2WcsIntervalQueue();
    scan_dip_state();
    InitGnd2WcsStateQueue();
    gnd2WcsCmdData.speed = 0;
    carInitSendMsgQueue();
    carthreeInitSendMsgQueue();
    cartwoInitSendMsgQueue();
    /* Infinite loop */
    while (1)
    {
        if(EthInitStatus == 0 )
        {
            Ethernet_Configuration();
            if( EthInitStatus != 0 )
            {
                LwIP_Init();
            }
        }
        else
        {
            if (ETH_GetRxPktSize() != 0)
            {
                LwIP_Pkt_Handle();
            }
            LwIP_Periodic_Handle(LocalTime);
            udp_client_process();
            send_message_to_sever();
            fun_gnd2wcs_state_msg();
            carsend_message_to_sever();
            carthreesend_message_to_sever();
            cartwosend_message_to_sever();
            usart2_recv_process();

        }
        sec_process();
    }
}

/**
  * @brief  Inserts a delay time.
  * @param  nCount: number of 10ms periods to wait for.
  * @retval None
  */
void Delay(uint32_t nCount)
{
    /* Capture the current local time */
    timingdelay = LocalTime + nCount;

    /* wait until the desired delay finish */
    while(timingdelay > LocalTime)
    {
    }
}

/**
  * @brief  Updates the system local time
  * @param  None
  * @retval None
  */
void Time_Update(void)
{
    LocalTime += SYSTEMTICK_PERIOD_MS;

    if( sec_reg != 0 )
    {
        sec_reg--;
        if( sec_reg == 0 )
        {
            sec_reg = 1000;
            sec_flag = 1;
        }
    }
    if (recv_gndpos_cnt != 0) {
        recv_gndpos_cnt--;
        if (recv_gndpos_cnt == 0) {
//            AddToGnd2WcsStateQueue(pregndposnod);
        }
    }
    if(usart4_sentcount !=0){
      usart4_sentcount--;
    }
    if(usart2_sentcount !=0){
      usart2_sentcount--;
    }
    if (positionreset != 0) {
        positionreset--;
        if (positionreset == 0) {
            carAddSendMsgToQueue(CAR_MSG_GNDCTRL2WCS_CMD_INTERVAL_TYPE);
        }
    }
    InputScanProc();
    position_ouput_out();
//    if((stop_time_delay != 0) && (servo_start == 1))
//    {
//        stop_time_delay--;
//        if(stop_time_delay == 0)
//        {
//            fwInitHz = fwInitServo();
//            servo_start = 0;
//        }
//    }
    //    uart_recv_timeout();


      
}


#ifdef  USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *   where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
    /* User can add his own implementation to report the file name and line number,
       ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

    /* Infinite loop */
    while (1)
    {}
}
#endif


/******************* (C) COPYRIGHT 2009 STMicroelectronics *****END OF FILE****/
