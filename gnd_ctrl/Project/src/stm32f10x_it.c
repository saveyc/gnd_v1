/**
  ******************************************************************************
  * @file    stm32f10x_it.c
  * @author  MCD Application Team
  * @version V1.0.0
  * @date    11/20/2009
  * @brief   Main Interrupt Service Routines.
  *          This file provides template for all exceptions handler and
  *          peripherals interrupt service routine.
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
#include "stm32f10x_it.h"
#include "stm32_eth.h"
#include "main.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
extern void LwIP_Pkt_Handle(void);

/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
/*            Cortex-M3 Processor Exceptions Handlers                         */
/******************************************************************************/

/**
  * @brief   This function handles NMI exception.
  * @param  None
  * @retval None
  */
void NMI_Handler(void)
{}

/**
  * @brief  This function handles Hard Fault exception.
  * @param  None
  * @retval None
  */
void HardFault_Handler(void)
{
    /* Go to infinite loop when Hard Fault exception occurs */
    while (1)
    {}
}

/**
  * @brief  This function handles Memory Manage exception.
  * @param  None
  * @retval None
  */
void MemManage_Handler(void)
{
    /* Go to infinite loop when Memory Manage exception occurs */
    while (1)
    {}
}

/**
  * @brief  This function handles Bus Fault exception.
  * @param  None
  * @retval None
  */
void BusFault_Handler(void)
{
    /* Go to infinite loop when Bus Fault exception occurs */
    while (1)
    {}
}

/**
  * @brief  This function handles Usage Fault exception.
  * @param  None
  * @retval None
  */
void UsageFault_Handler(void)
{
    /* Go to infinite loop when Usage Fault exception occurs */
    while (1)
    {}
}

/**
  * @brief  This function handles SVCall exception.
  * @param  None
  * @retval None
  */
void SVC_Handler(void)
{}

/**
  * @brief  This function handles Debug Monitor exception.
  * @param  None
  * @retval None
  */
void DebugMon_Handler(void)
{}

/**
  * @brief  This function handles PendSVC exception.
  * @param  None
  * @retval None
  */
void PendSV_Handler(void)
{}

/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  */
void SysTick_Handler(void)
{
    /* Update the LocalTime by adding SYSTEMTICK_PERIOD_MS each SysTick interrupt */
    Time_Update();
}
/*************************************************
1MS
*************************************************/
void TIM2_IRQHandler(void)
{
    if (TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET)
    {
        timUpdates++;
        if(timUpdates >= 0xFFFF)
        {
            timUpdates = 0;
            timExtiHandler.extid = 0;
            timExtiHandler.timCnt = 0;
        }
        TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
    }

}

/*************************************************
1MS
*************************************************/
void TIM3_IRQHandler(void)
{
    if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET)
    {
        if(wcs2GndParaData.speed_target != 0)
        {
            interval_ms_flag++;    
        }
        TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
    }

}
/******************************************************************************/
/*                 STM32F10x Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f10x_xx.s).                                            */
/******************************************************************************/

/**
  * @brief  This function handles ETH interrupt request.
  * @param  None
  * @retval None
  */
//void ETH_IRQHandler(void)
//{
//    /* Handles all the received frames */
//    while(ETH_GetRxPktSize() != 0)
//    {
//        LwIP_Pkt_Handle();
//    }
//
//    /* Clear the Eth DMA Rx IT pending bits */
//    ETH_DMAClearITPendingBit(ETH_DMA_IT_R);
//    ETH_DMAClearITPendingBit(ETH_DMA_IT_NIS);
//}

void EXTI4_IRQHandler(void)
{
    u16 i;
    sGndCtrl2WCS_Interval_Data  interval_Data ;
    sGnd2Wcs_state_node statenode;
    //IN6
    if(EXTI_GetITStatus(EXTI_Line4) != RESET)
    {
        EXTI_ClearITPendingBit(EXTI_Line4);
        for(i = 0 ; i < EXT_DELAY ; i++)
        {
            if(EXT6_STATE == 0)
                break;
        }
        if(i == EXT_DELAY)
        {
            interval_Data.PreCarNum = gnd2WcsCmdData.carNo;
            gnd2WcsCmdData.carNo++;
            interval_Data.CurCarNum = gnd2WcsCmdData.carNo;
            interval_Data.interval = interval_ms_flag;//发送光电时间间隔
            statenode.speed = gnd2WcsCmdData.speed;
            if (gnd2WcsCmdData.carNo > 0) {
                statenode.carnum = gnd2WcsCmdData.carNo - 1;
            }
            statenode.servo = servo_hz;

            AddToGnd2WcsStateQueue(statenode);
            recv_gndpos_cnt = 50;
            
            AddToGnd2WcsIntervalQueue(interval_Data);
     
            AddSendMsgToQueue(SEND_MSG_GNDCTRL2WCS_CMD_INTERVAL_TYPE);
            
            AddSendMsgToQueue(SEND_MSG_GNDCTRL2WCS_CMD_TYPE);
            
            interval_ms_flag = 0;
             
            //TIM_EXTI_Process(6);
            TIM_EXTI_Process(1);
            repairLocationCtrl();
            msg_send_delay = 10;
        }
    }
}
void EXTI9_5_IRQHandler(void)
{
    u16 i;
    //IN7
    if(EXTI_GetITStatus(EXTI_Line5) != RESET)
    {
        EXTI_ClearITPendingBit(EXTI_Line5);
        for(i = 0 ; i < EXT_DELAY ; i++)
        {
            if(EXT7_STATE == 0)
                break;
        }
        if(i == EXT_DELAY)
        {
            //TIM_EXTI_Process(7);
            TIM_EXTI_Process(2);
        }
    }
    //IN10
    //接近开关车号复位
    if(EXTI_GetITStatus(EXTI_Line7) != RESET)
    {
        EXTI_ClearITPendingBit(EXTI_Line7);
        for(i = 0 ; i < EXT_DELAY ; i++)
        {
            if(EXT10_STATE == 0)
                break;
        }
        if(i == EXT_DELAY)
        {
            gnd2WcsCmdData.carNo = 0;
            reset_flag = 1;
        }
    }
}


/******************* (C) COPYRIGHT 2009 STMicroelectronics *****END OF FILE****/
