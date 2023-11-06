																			   /**
  ******************************************************************************
  * @file    main.h
  * @author  MCD Application Team
  * @version V1.0.0
  * @date    11/20/2009
  * @brief   This file contains all the functions prototypes for the main.c 
  *          file.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"
#include "stm32f107.h"
#include "fun.h"
#include "timExti.h"
#include "speed_ctrl.h"
#include "carfun.h"
#include "carfun.h"
#include "carfuntwo.h"
#include "carfunthree.h"
#include "gpio.h"
//#include "delay.h"
   
#define  USE_UDP

/* Exported function prototypes ----------------------------------------------*/
void Time_Update(void);
void Delay(uint32_t nCount);

/* ETHERNET errors */
#define  ETH_ERROR              ((uint32_t)0)
#define  ETH_SUCCESS            ((uint32_t)1)
#define  DP83848_PHY_ADDRESS       0x01

#define	TCP_RECEV_BUFF_SIZE		1500
#define	CLIENT_DIS_CONNECT		0
#define	CLIENT_CONNECT_OK		1
#define	CLIENT_CONNECT_RECV		2
#define	CLIENT_RE_CONNECT		3

#define DEST_IP_ADDR0   192
#define DEST_IP_ADDR1   168
#define DEST_IP_ADDR2   10
#define DEST_IP_ADDR3   13
#define DEST_PORT       9902


#define DEST_IPTWO_ADDR0   192
#define DEST_IPTWO_ADDR1   168
#define DEST_IPTWO_ADDR2   10
#define DEST_IPTWO_ADDR3   110
#define DEST_PORT_TWO      9000

#define DEST_IPTHREE_ADDR0   192
#define DEST_IPTHREE_ADDR1   168
#define DEST_IPTHREE_ADDR2   10
#define DEST_IPTHREE_ADDR3   111
#define DEST_PORT_THREE      9000

#define DEST_IPFOUR_ADDR0   192
#define DEST_IPFOUR_ADDR1   168
#define DEST_IPFOUR_ADDR2   10
#define DEST_IPFOUR_ADDR3   112
#define DEST_PORT_FOUR      9000

#define LOCAL_UDP_PORT  DEST_PORT
/* MAC ADDRESS: MAC_ADDR0:MAC_ADDR1:MAC_ADDR2:MAC_ADDR3:MAC_ADDR4:MAC_ADDR5 */
#define MAC_ADDR0   0
#define MAC_ADDR1   0
#define MAC_ADDR2   0
#define MAC_ADDR3   0
#define MAC_ADDR4   0
#define MAC_ADDR5   3
/*Static IP ADDRESS: IP_ADDR0.IP_ADDR1.IP_ADDR2.IP_ADDR3 */
#define IP_ADDR0   192
#define IP_ADDR1   168
#define IP_ADDR2   10
#define IP_ADDR3   100
/*NETMASK*/
#define NETMASK_ADDR0   255
#define NETMASK_ADDR1   255
#define NETMASK_ADDR2   255
#define NETMASK_ADDR3   0
/*Gateway Address*/
#define GW_ADDR0   192
#define GW_ADDR1   168
#define GW_ADDR2   10
#define GW_ADDR3   1

#define  EXT1_STATE             GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_9)
#define  EXT2_STATE             GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_0)
#define  EXT3_STATE             GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_1)
#define  EXT4_STATE             GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_2)
#define  EXT5_STATE             GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_3)
#define  EXT6_STATE             GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_4)
#define  EXT7_STATE             GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_5)
#define  EXT8_STATE             GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_6)
#define  EXT9_STATE             GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_13)
#define  EXT10_STATE            GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_7)

#define	 LED_STATE		        GPIO_ReadOutputDataBit(GPIOC, GPIO_Pin_8)
#define	 LED_ON			        GPIO_ResetBits(GPIOC,GPIO_Pin_8)
#define	 LED_OFF		        GPIO_SetBits(GPIOC,GPIO_Pin_8)

#define	 OUTONE_ON			GPIO_SetBits(GPIOC,GPIO_Pin_0)
#define	 OUTONE_OFF		        GPIO_ResetBits(GPIOC,GPIO_Pin_0)

#define	 OUTTWO_ON		     	GPIO_SetBits(GPIOB,GPIO_Pin_14)
#define	 OUTTWO_OFF		        GPIO_ResetBits(GPIOB,GPIO_Pin_14)

#define  DIP1_STATE             GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_4)
#define  DIP2_STATE             GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_5)
#define  DIP3_STATE             GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_6)
#define  DIP4_STATE             GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_7)

#define	 UART5_RX_485		GPIO_ResetBits( GPIOD, GPIO_Pin_3);
#define	 UART5_TX_485		GPIO_SetBits( GPIOD, GPIO_Pin_3);

#define	 USART2_RX_485	        GPIO_ResetBits( GPIOD, GPIO_Pin_4);
#define	 USART2_TX_485	        GPIO_SetBits( GPIOD, GPIO_Pin_4);

#define	 UART4_RX_485		GPIO_ResetBits( GPIOD, GPIO_Pin_0);
#define	 UART4_TX_485		GPIO_SetBits( GPIOD, GPIO_Pin_0);

#define	 USART1_RX_485		GPIO_ResetBits( GPIOA, GPIO_Pin_11);
#define	 USART1_TX_485		GPIO_SetBits( GPIOA, GPIO_Pin_11);

#define	 RECV_DATA		1
#define	 SEND_DATA		2
#define  SENDING_DATA           3
#define	 RECV_DATA_END	        4

#define UART_BANDRATE  19200

#define UART4_BUFF_SIZE  100
extern u8  uart4_send_buff[UART4_BUFF_SIZE];
extern u8  uart4_recv_buff[UART4_BUFF_SIZE];
extern u16 uart4_send_count;
extern u16 uart4_recv_count;
extern u8  uart4_commu_state;
extern u16 uart4_tmr;
#define UART5_BUFF_SIZE  100
extern u8  uart5_send_buff[UART5_BUFF_SIZE];
extern u8  uart5_recv_buff[UART5_BUFF_SIZE];
extern u16 uart5_send_count;
extern u16 uart5_recv_count;
extern u8  uart5_commu_state;
extern u16 uart5_tmr;
#define UART2_BUFF_SIZE  100
extern u8  uart2_send_buff[UART2_BUFF_SIZE];
extern u8  uart2_recv_buff[UART2_BUFF_SIZE];
extern u16 uart2_send_count;
extern u16 uart2_recv_count;
extern u8  uart2_commu_state;
extern u16 uart2_tmr;
#define UART1_BUFF_SIZE  100
extern u8  uart1_send_buff[UART1_BUFF_SIZE];
extern u8  uart1_recv_buff[UART1_BUFF_SIZE];
extern u16 uart1_send_count;
extern u16 uart1_recv_count;
extern u8  uart1_commu_state;
extern u16 uart1_tmr;

#define UART_SEND_DELAY  200
void uart_recv_timeout(void);
void uart4_send(void);
void uart5_send(void);
void uart2_send(void);
void uart1_send(void);
void ServoFreqSet(u16 hz, u8 type);
void scan_dip_state(void);

#define HEART_DELAY   5
#define EXT_DELAY  20
extern u16 fwInitHz;
extern u16 servo_hz;
extern u8  servo_start;
extern u8  heart_dely;
extern u8  repair_flag;
extern u8  reset_flag;
extern u16 repair_locate_real;
extern u8  inverter_type;
extern u8  msg_send_delay;
extern u8  version[];
extern u16  interval_ms_flag;


extern u8  record_uart2_buff[];
extern u8  record_uart2_len;

extern u8  record_uart4_buff[];
extern u8  record_uart4_len;

void usart2_recv_process(void);

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */


/******************* (C) COPYRIGHT 2009 STMicroelectronics *****END OF FILE****/

