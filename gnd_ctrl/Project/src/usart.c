#include "stm32f10x.h"
#include "stm32f107.h"
#include "main.h"

u8  uart4_send_buff[UART4_BUFF_SIZE];
u8  uart4_recv_buff[UART4_BUFF_SIZE];
u16 uart4_send_count;
u16 uart4_recv_count;
u8  uart4_commu_state;
u16 uart4_tmr = 0;

u8  uart5_send_buff[UART5_BUFF_SIZE];
u8  uart5_recv_buff[UART5_BUFF_SIZE];
u16 uart5_send_count;
u16 uart5_recv_count;
u8  uart5_commu_state;
u16 uart5_tmr = 0;

u8  uart2_send_buff[UART2_BUFF_SIZE];
u8  uart2_recv_buff[UART2_BUFF_SIZE];
u16 uart2_send_count;
u16 uart2_recv_count;
u8  uart2_commu_state;
u16 uart2_tmr = 0;

u8  uart1_send_buff[UART1_BUFF_SIZE];
u8  uart1_recv_buff[UART1_BUFF_SIZE];
u16 uart1_send_count;
u16 uart1_recv_count;
u8  uart1_commu_state;
u16 uart1_tmr = 0;

void uart_recv_timeout(void)
{
    if( uart4_tmr != 0 )
    {
        uart4_tmr--;
        if( uart4_tmr == 0 )
        {
            uart4_commu_state = RECV_DATA_END;
        }
    }
    if( uart5_tmr != 0 )
    {
        uart5_tmr--;
        if( uart5_tmr == 0 )
        {
            uart5_commu_state = RECV_DATA_END;
        }
    }
    if( uart2_tmr != 0 )
    {
        uart2_tmr--;
        if( uart2_tmr == 0 )
        {
            uart2_commu_state = RECV_DATA_END;
        }
    }
    if( uart1_tmr != 0 )
    {
        uart1_tmr--;
        if( uart1_tmr == 0 )
        {
            uart1_commu_state = RECV_DATA_END;
        }
    }
}

void uart4_send(void)
{
    //    u16 i;
    //
    //    UART4_TX_485;
    //
    //    for(i=0 ; i<uart4_send_count ; i++)
    //    {
    //        USART_SendData(UART4, uart4_send_buff[i]);
    //	while (USART_GetFlagStatus(UART4, USART_FLAG_TC) == RESET);
    //    }
    //    UART4_RX_485;
    //    uart4_commu_state = RECV_DATA;
    //    uart4_recv_count = 0;
    //    uart4_tmr = UART_SEND_DELAY;
    UART4_TX_485;

    USART_DMACmd(UART4, USART_DMAReq_Tx, DISABLE);
    DMA_Cmd(DMA2_Channel5, DISABLE);
    DMA_SetCurrDataCounter(DMA2_Channel5, uart4_send_count);
    USART_DMACmd(UART4, USART_DMAReq_Tx, ENABLE);
    DMA_Cmd(DMA2_Channel5, ENABLE);
}

void uart5_send(void)
{
    u16 i;

    UART5_TX_485;

    for(i = 0 ; i < uart5_send_count ; i++)
    {
        USART_SendData(UART5, uart5_send_buff[i]);
        while (USART_GetFlagStatus(UART5, USART_FLAG_TC) == RESET);
    }
    //UART5_RX_485;
    //uart5_commu_state = RECV_DATA;
    //uart5_recv_count = 0;
    //uart5_tmr = UART_SEND_DELAY;
}

void uart2_send(void)
{
    //    u16 i;
    //
    //    USART2_TX_485;
    //
    //    for(i=0 ; i<uart2_send_count ; i++)
    //    {
    //        USART_SendData(USART2, uart2_send_buff[i]);
    //	while (USART_GetFlagStatus(USART2, USART_FLAG_TC) == RESET);
    //    }
    //    USART2_RX_485;
    //    uart2_commu_state = RECV_DATA;
    //    uart2_recv_count = 0;
    //    uart2_tmr = UART_SEND_DELAY;
    USART2_TX_485;

    USART_DMACmd(USART2, USART_DMAReq_Tx, DISABLE);
    DMA_Cmd(DMA1_Channel7, DISABLE);
    DMA_SetCurrDataCounter(DMA1_Channel7, uart2_send_count);
    USART_DMACmd(USART2, USART_DMAReq_Tx, ENABLE);
    DMA_Cmd(DMA1_Channel7, ENABLE);
}

void uart1_send(void)
{
    u16 i;

    USART1_TX_485;

    for(i = 0 ; i < uart1_send_count ; i++)
    {
        USART_SendData(USART1, uart1_send_buff[i]);
        while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);
    }
//    USART1_RX_485;
//    uart1_commu_state = RECV_DATA;
//    uart1_recv_count = 0;
//    uart1_tmr = UART_SEND_DELAY;
}

const u8 CRCHi[ ] =
{
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
    0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
    0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
};
const u8 CRCLo[ ] =
{
    0x00, 0xC0, 0xC1, 0x01, 0xC3, 0x03, 0x02, 0xC2, 0xC6, 0x06, 0x07, 0xC7, 0x05, 0xC5, 0xC4, 0x04,
    0xCC, 0x0C, 0x0D, 0xCD, 0x0F, 0xCF, 0xCE, 0x0E, 0x0A, 0xCA, 0xCB, 0x0B, 0xC9, 0x09, 0x08, 0xC8,
    0xD8, 0x18, 0x19, 0xD9, 0x1B, 0xDB, 0xDA, 0x1A, 0x1E, 0xDE, 0xDF, 0x1F, 0xDD, 0x1D, 0x1C, 0xDC,
    0x14, 0xD4, 0xD5, 0x15, 0xD7, 0x17, 0x16, 0xD6, 0xD2, 0x12, 0x13, 0xD3, 0x11, 0xD1, 0xD0, 0x10,
    0xF0, 0x30, 0x31, 0xF1, 0x33, 0xF3, 0xF2, 0x32, 0x36, 0xF6, 0xF7, 0x37, 0xF5, 0x35, 0x34, 0xF4,
    0x3C, 0xFC, 0xFD, 0x3D, 0xFF, 0x3F, 0x3E, 0xFE, 0xFA, 0x3A, 0x3B, 0xFB, 0x39, 0xF9, 0xF8, 0x38,
    0x28, 0xE8, 0xE9, 0x29, 0xEB, 0x2B, 0x2A, 0xEA, 0xEE, 0x2E, 0x2F, 0xEF, 0x2D, 0xED, 0xEC, 0x2C,
    0xE4, 0x24, 0x25, 0xE5, 0x27, 0xE7, 0xE6, 0x26, 0x22, 0xE2, 0xE3, 0x23, 0xE1, 0x21, 0x20, 0xE0,
    0xA0, 0x60, 0x61, 0xA1, 0x63, 0xA3, 0xA2, 0x62, 0x66, 0xA6, 0xA7, 0x67, 0xA5, 0x65, 0x64, 0xA4,
    0x6C, 0xAC, 0xAD, 0x6D, 0xAF, 0x6F, 0x6E, 0xAE, 0xAA, 0x6A, 0x6B, 0xAB, 0x69, 0xA9, 0xA8, 0x68,
    0x78, 0xB8, 0xB9, 0x79, 0xBB, 0x7B, 0x7A, 0xBA, 0xBE, 0x7E, 0x7F, 0xBF, 0x7D, 0xBD, 0xBC, 0x7C,
    0xB4, 0x74, 0x75, 0xB5, 0x77, 0xB7, 0xB6, 0x76, 0x72, 0xB2, 0xB3, 0x73, 0xB1, 0x71, 0x70, 0xB0,
    0x50, 0x90, 0x91, 0x51, 0x93, 0x53, 0x52, 0x92, 0x96, 0x56, 0x57, 0x97, 0x55, 0x95, 0x94, 0x54,
    0x9C, 0x5C, 0x5D, 0x9D, 0x5F, 0x9F, 0x9E, 0x5E, 0x5A, 0x9A, 0x9B, 0x5B, 0x99, 0x59, 0x58, 0x98,
    0x88, 0x48, 0x49, 0x89, 0x4B, 0x8B, 0x8A, 0x4A, 0x4E, 0x8E, 0x8F, 0x4F, 0x8D, 0x4D, 0x4C, 0x8C,
    0x44, 0x84, 0x85, 0x45, 0x87, 0x47, 0x46, 0x86, 0x82, 0x42, 0x43, 0x83, 0x41, 0x81, 0x80, 0x40,
};
u16 CRC_sub( u8 *head, u8 *tail, u16 InitVal, u8 exchange )
{
    u8	CRC_Hi;
    u8	CRC_Lo;
    u8	i;
    u16 result;
    CRC_Hi = InitVal >> 8;
    CRC_Lo = InitVal;

    while( head < tail )
    {
        i = CRC_Hi ^ *head++;
        CRC_Hi = CRC_Lo ^ CRCHi[ i ];
        CRC_Lo = CRCLo[ i ];
    }

    if( ! exchange )
    {
        result = 0;
        result = CRC_Hi;
        result = result << 8;
        result |= CRC_Lo;
    }
    else
    {
        result = 0;
        result = CRC_Lo;
        result = result << 8;
        result |= CRC_Hi;
    }
    return(result);
}
u16 get_CRC( u8 *head, u16 len )
{
    return( CRC_sub( head, head + len, 0xFFFF, 0 ) );
}
void ServoFreqSet(u16 hz)
{
    u8 pbuf[50];
    u16 crc;
    u16 i;
    u16 hz_t;

    if(inverter_type == 0)//东芝变频器
    {
        pbuf[0] = 0x00;
        pbuf[1] = 0x06;
        pbuf[2] = 0xFA;
        pbuf[3] = 0x01;
        pbuf[4] = (hz >> 8) & 0xFF;
        pbuf[5] = hz & 0xFF;
        crc = get_CRC(pbuf, 6);
        pbuf[6] = (crc >> 8) & 0xFF;
        pbuf[7] = crc & 0xFF;

        for (i = 0; i < 8; i++)
        {
            uart2_send_buff[i] = pbuf[i];
            uart4_send_buff[i] = pbuf[i];
        }
        uart2_send_count = 8;
        uart4_send_count = 8;
    }
    else if(inverter_type == 1)//台达变频器
    {
        pbuf[0] = 0x00;
        pbuf[1] = 0x06;
        pbuf[2] = 0x20;
        pbuf[3] = 0x01;
        pbuf[4] = (hz >> 8) & 0xFF;
        pbuf[5] = hz & 0xFF;
        crc = get_CRC(pbuf, 6);
        pbuf[6] = (crc >> 8) & 0xFF;
        pbuf[7] = crc & 0xFF;

        for (i = 0; i < 8; i++)
        {
            uart2_send_buff[i] = pbuf[i];
            uart4_send_buff[i] = pbuf[i];
        }
        uart2_send_count = 8;
        uart4_send_count = 8;
    }
    else if (inverter_type == 2)
    {
    hz_t = (u16)((hz / 5000.0) * 0x4000);

    pbuf[0] = 0x01;
    pbuf[1] = 0x10;
    pbuf[2] = 0x00;
    pbuf[3] = 0x63;
    pbuf[4] = 0x00;
    pbuf[5] = 0x02;
    pbuf[6] = 0x04;
    pbuf[7] = 0x04;
    pbuf[8] = 0x7F;
    pbuf[9] = (hz_t >> 8) & 0xFF;
    pbuf[10] = hz_t & 0xFF;
    crc = get_CRC(pbuf, 11);
    pbuf[11] = (crc >> 8) & 0xFF;
    pbuf[12] = crc & 0xFF;

    for (i = 0; i < 13; i++)
    {
        uart2_send_buff[i] = pbuf[i];
        uart4_send_buff[i] = pbuf[i];
    }
    uart2_send_count = 13;
    uart4_send_count = 13;
    }
    else
    {
    return;
    }
    
    uart2_send();//dma
    uart4_send();//dma
}
#if 0
void UartSendPidCnt(u16 extid, u32 cnt)
{
    if(extid == 2)
    {
        uart1_send_buff[0] = (cnt >> 0) & 0xFF;
        uart1_send_buff[1] = (cnt >> 8) & 0xFF;
        uart1_send_buff[2] = (cnt >> 16) & 0xFF;
        uart1_send_buff[3] = (cnt >> 24) & 0xFF;
    }
    else if(extid == 3)
    {
        uart1_send_buff[4] = (cnt >> 0) & 0xFF;
        uart1_send_buff[5] = (cnt >> 8) & 0xFF;
        uart1_send_buff[6] = (cnt >> 16) & 0xFF;
        uart1_send_buff[7] = (cnt >> 24) & 0xFF;
    }
    else if(extid == 4)
    {
        uart1_send_buff[8] = (cnt >> 0) & 0xFF;
        uart1_send_buff[9] = (cnt >> 8) & 0xFF;
        uart1_send_buff[10] = (cnt >> 16) & 0xFF;
        uart1_send_buff[11] = (cnt >> 24) & 0xFF;
    }
    else if(extid == 5)
    {
        uart1_send_buff[12] = (cnt >> 0) & 0xFF;
        uart1_send_buff[13] = (cnt >> 8) & 0xFF;
        uart1_send_buff[14] = (cnt >> 16) & 0xFF;
        uart1_send_buff[15] = (cnt >> 24) & 0xFF;
    }
    else if(extid == 6)
    {
        uart1_send_buff[16] = (cnt >> 0) & 0xFF;
        uart1_send_buff[17] = (cnt >> 8) & 0xFF;
        uart1_send_buff[18] = (cnt >> 16) & 0xFF;
        uart1_send_buff[19] = (cnt >> 24) & 0xFF;
    }
    else if(extid == 7)
    {
        uart1_send_buff[20] = (cnt >> 0) & 0xFF;
        uart1_send_buff[21] = (cnt >> 8) & 0xFF;
        uart1_send_buff[22] = (cnt >> 16) & 0xFF;
        uart1_send_buff[23] = (cnt >> 24) & 0xFF;
        uart1_send_count = 24;
        //uart1_send();
    }
}
#else
extern int   fwHz;
void UartSendPidCnt(u16 extid,u32 cnt)
{
    if(extid==2)
    {
        //uart5_send_count = sprintf(uart5_send_buff, "extid2:%d," , cnt);
        //uart5_send_count += sprintf(uart5_send_buff+uart5_send_count, "%d\n" , fwHz);
        uart5_send_count = sprintf(uart5_send_buff, "2:%d\n" , cnt);
        uart5_send();
    }
    else if(extid==3)
    {
        //uart5_send_count = sprintf(uart5_send_buff, "extid3:%d," , cnt);
        //uart5_send_count += sprintf(uart5_send_buff+uart5_send_count, "%d\n" , fwHz);
        uart5_send_count = sprintf(uart5_send_buff, "3:%d\n" , cnt);
        uart5_send();
    }
    else if(extid==4)
    {
        //uart5_send_count = sprintf(uart5_send_buff, "extid4:%d," , cnt);
        //uart5_send_count += sprintf(uart5_send_buff+uart5_send_count, "%d\n" , fwHz);
        uart5_send_count = sprintf(uart5_send_buff, "4:%d\n" , cnt);
        uart5_send();
    }
    else if(extid==5)
    {
        //uart5_send_count = sprintf(uart5_send_buff, "extid5:%d," , cnt);
        //uart5_send_count += sprintf(uart5_send_buff+uart5_send_count, "%d\n" , fwHz);
        uart5_send_count = sprintf(uart5_send_buff, "5:%d\n" , cnt);
        uart5_send();
    }
    else if(extid==6)
    {
        //uart5_send_count = sprintf(uart5_send_buff, "extid6:%d," , cnt);
        //uart5_send_count += sprintf(uart5_send_buff+uart5_send_count, "%d\n" , fwHz);
        uart5_send_count = sprintf(uart5_send_buff, "6:%d\n" , cnt);
        uart5_send();
    }
    else if(extid==7)
    {
        //uart5_send_count = sprintf(uart5_send_buff, "extid7:%d," , cnt);
        //uart5_send_count += sprintf(uart5_send_buff+uart5_send_count, "%d\n" , fwHz);
        uart5_send_count = sprintf(uart5_send_buff, "7:%d\n" , cnt);
        uart5_send();
    }
}
#endif
void UartSendCarNo(u16 carNo)
{
    //uart5_send_count = sprintf(uart5_send_buff, "carNo:%d\n" , carNo);
    uart5_send_buff[0] = 0xAB;
    uart5_send_buff[1] = 0x01;
    uart5_send_buff[2] = 0xAC;
    uart5_send_count = 3;
    uart5_send();
}
void RS485_Test(u32 cnt)
{
    uart4_send_count = sprintf(uart4_send_buff, "uart4_r8_send:%d\n" , cnt);
    uart5_send_count = sprintf(uart5_send_buff, "uart5_r6_send:%d\n" , cnt);
    uart2_send_count = sprintf(uart2_send_buff, "uart2_r7_send:%d\n" , cnt);
    uart1_send_count = sprintf(uart1_send_buff, "uart1_r9_send:%d\n" , cnt);
    if(uart4_commu_state != RECV_DATA)
    {
        uart4_send();
    }
    if(uart5_commu_state != RECV_DATA)
    {
        uart5_send();
    }
    if(uart2_commu_state != RECV_DATA)
    {
        uart2_send();
    }
    if(uart1_commu_state != RECV_DATA)
    {
        uart1_send();
    }
}