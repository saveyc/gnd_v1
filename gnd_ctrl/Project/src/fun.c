#include "stm32f10x.h"
#include "main.h"
#include "TCPclient.h"

sGndCtrl2WCS_CMD_Data   gnd2WcsCmdData;
sWCS2GndCtrl_Para_Data  wcs2GndParaData;
sRepairCar_CMD_Data     repairCarCmdData;

sGndCtrl2WCS_Interval_Data  gnd2WcsIntervalData[INTERVAL_QUEUE_LEN];
sGndCtrl2WCS_Interval_Queue  gnd2WcsIntervalQueue;


sGnd2Wcs_state_node     gnd2wcsstatenode[GND_STATE_LEN];
sGndCtrl2WCS_state_Queue    gndstatequeue;

#define msgSendQueueSize  100
u16 msgQueueBuff[msgSendQueueSize];
MSG_SEND_QUEUE msgSendQueue;

u16  recv_gndpos_cnt = 0;

sGnd2Wcs_state_node  pregndposnod;

sGndCtrl2WCS_Interval_Data  carfunint;

u16 usart2_sentcount = 0;
u16 usart4_sentcount = 0;

u16 positionreset = 0;

void initQueue(MSG_SEND_QUEUE *q, u16 ms)
{
    q->maxSize = ms;
    q->queue = msgQueueBuff;
    q->front = q->rear = 0;
}
void enQueue(MSG_SEND_QUEUE *q, u16 x)
{
    //队列满
    if((q->rear + 1) % q->maxSize == q->front)
    {
        return;
    }
    q->rear = (q->rear + 1) % q->maxSize; // 求出队尾的下一个位置
    q->queue[q->rear] = x; // 把x的值赋给新的队尾
}
u16 outQueue(MSG_SEND_QUEUE *q)
{
    //队列空
    if(q->front == q->rear)
    {
        return 0;
    }
    q->front = (q->front + 1) % q->maxSize; // 使队首指针指向下一个位置
    return q->queue[q->front]; // 返回队首元素
}
void InitSendMsgQueue(void)
{
    initQueue(&msgSendQueue, msgSendQueueSize);
}
void AddSendMsgToQueue(u16 msg)
{
    enQueue(&msgSendQueue, msg);
}
u16 GetSendMsgFromQueue(void)
{
    return (outQueue(&msgSendQueue));
}

//
void InitGnd2WcsIntervalQueue(void)
{
    sGndCtrl2WCS_Interval_Queue* q;
    
    q = &gnd2WcsIntervalQueue;
    q->queue = gnd2WcsIntervalData;

    q->front = q->rear = 0;

    q->maxsize = INTERVAL_QUEUE_LEN;    
}

//
void AddToGnd2WcsIntervalQueue(sGndCtrl2WCS_Interval_Data x)
{
    sGndCtrl2WCS_Interval_Queue* q;
    
    q = &gnd2WcsIntervalQueue;

    if((q->rear + 1) % q->maxsize == q->front)
    {
        return;
    }
    
    q->rear = (q->rear+1) % q->maxsize;
    q->queue[q->rear] = x; 
}

//
sGndCtrl2WCS_Interval_Data* pxGetMsgFromIntervalQueue(sGndCtrl2WCS_Interval_Queue* q)
{
    if(q->front == q->rear)
    {
        return 0;
    }
    q->front = (q->front + 1) % q->maxsize;
    return (sGndCtrl2WCS_Interval_Data*)(&(q->queue[q->front]));
}


void InitGnd2WcsStateQueue(void)
{
    sGndCtrl2WCS_state_Queue* q = 0;

    q = &gndstatequeue;
    q->queue = gnd2wcsstatenode;

    q->front = q->rear = 0;

    q->maxsize = GND_STATE_LEN;
}

//
void AddToGnd2WcsStateQueue(sGnd2Wcs_state_node x)
{
    sGndCtrl2WCS_state_Queue* q = 0;

    q = &gndstatequeue;

    if (((q->rear + 1) % q->maxsize) == q->front)
    {
        return;
    }

    q->rear = (q->rear + 1) % q->maxsize;
    q->queue[q->rear] = x;
}


//
sGnd2Wcs_state_node* pxGetMsgFromStateQueue(sGndCtrl2WCS_state_Queue* q)
{
    if (q->front == q->rear)
    {
        return 0;
    }
    q->front = (q->front + 1) % q->maxsize;
    return (sGnd2Wcs_state_node*)(&(q->queue[q->front]));
}

u8 IsGnd2WcsStateQueueFree(void)
{
    sGndCtrl2WCS_state_Queue* q = 0;

    q = &gndstatequeue;

    if (q->front == q->rear)
    {
        return 0;
    }
    else {
        return 1;
    }
}


u8 recv_msg_check(u8 *point, u16 len)
{
    u16 i = 0;
    u8  sum = 0;

    if((point[0] != 0xAA) || (point[1] != 0xAA))
        return 0;
    if((point[6] | point[7] << 8) != len)
        return 0;
    sum = point[9];
    for(i = 1 ; i < len - 9 ; i++)
    {
        sum ^= point[9 + i];
    }
    if(sum != point[8])
        return 0;

    return 1;
}
void recv_msg_process(u8 *point)
{
    MSG_HEAD_DATA *head = (MSG_HEAD_DATA *)point;
    
    if( head->MSG_TYPE == RECV_MSG_BOOT_CMD_TYPE )
    {
        BKP_WriteBackupRegister(BKP_DR8, 0x55);
        NVIC_SystemReset();
    }
    switch(head->MSG_TYPE)
    {
    case RECV_MSG_WCS2GNDCTRL_PARA_TYPE:
        repair_flag = 0;
        wcs2GndParaData = *((sWCS2GndCtrl_Para_Data *)point);
        AddSendMsgToQueue(REPLY_RECV_MSG_WCS2GNDCTRL_PARA_TYPE);
        AddSendMsgToQueue(REPLY_WCS_GND_VERSION_TYPE);
        fwInitHz = fwInitServo();
        servo_start = 0;
        break;
    case RECV_MSG_REPAIR_CAR_CMD_TYPE:
        repairCarCmdData = *((sRepairCar_CMD_Data *)point);
        repair_car_para_init();
        AddSendMsgToQueue(REPLY_RECV_MSG_REPAIR_CAR_CMD_TYPE);
        break;
    case REPLY_SEND_MSG_GNDCTRL2WCS_CMD_TYPE:
        recv_gndpos_cnt = 0;
        break;
    default:
        break;
    }
}
void recv_message_from_sever(u8 *point, u16 *len)
{
    if(recv_msg_check(point, *len) == 0)
    {
        *len = 0;
        return;
    }
    recv_msg_process(point);
    *len = 0;
}
void reply_recv_msg(u8 *buf, u16 *len, u16 type)
{
    u8 sum = (type & 0xFF) ^ (type >> 8);

    buf[0] = 0xAA;
    buf[1] = 0xAA;
    buf[2] = 0x01;
    buf[3] = 0x00;
    buf[4] = 0x00;
    buf[5] = 0x00;
    buf[6] = 0x0B;
    buf[7] = 0x00;
    buf[8] = sum;
    buf[9] = type & 0xFF;
    buf[10] = (type >> 8) & 0xFF;

    *len = 11;
}
extern int fwHz;
void send_msg_gndctrl2wcs_cmd(u8 *buf, u16 *len, u16 type)
{
    sGnd2Wcs_state_node* node = 0;
    u8  sum = 0;
    u16 sendlen = 0;
    u16 i = 0;

    node = pxGetMsgFromStateQueue(&gndstatequeue);
    pregndposnod = *node;
    //UartSendCarNo(usrCarNo);//debug
    sendlen = sizeof(sGndCtrl2WCS_CMD_Data);
    buf[9] = type & 0xFF;
    buf[10] = (type >> 8) & 0xFF;
    buf[11] = (node->speed) & 0xFF;
    buf[12] = ((node->speed) >>8) & 0xFF;
    buf[13] = ((node->speed) >>16) & 0xFF;
    buf[14] = ((node->speed) >>24) & 0xFF;
    buf[15] = (node->carnum) & 0xFF;
    buf[16] = ((node->carnum) >>8) & 0xFF;
    buf[17] = 0;
    buf[18] = 0;
    buf[19] = (node->servo) & 0xFF; //170810
    buf[20] = ((node->servo) >>8) & 0xFF;
    sum = buf[9];
    for(i = 1 ; i < sendlen - 9 ; i++)
    {
        sum ^= buf[9 + i];
    }
    buf[0] = 0xAA;
    buf[1] = 0xAA;
    buf[2] = 0x01;
    buf[3] = 0x00;
    buf[4] = 0x00;
    buf[5] = 0x00;
    buf[6] = sendlen & 0xFF;
    buf[7] = (sendlen >> 8) & 0xFF;
    buf[8] = sum;

    *len = sendlen;
}

//主动发送小车发生位置变化的间隔
void send_msg_gndctrl2wcs_cmd_interval_data(u8 *buf, u16 *len, u16 type)
{
    u8  sum;
    u16 sendlen;
    u16 i;
    sGndCtrl2WCS_Interval_Data* intervalData = 0;
    
    sendlen = 11 + 6;
   
    intervalData = pxGetMsgFromIntervalQueue(&gnd2WcsIntervalQueue);
    
    if(intervalData == 0)return;

    carfunint = *intervalData;
    
    buf[9] = type & 0xFF;
    buf[10] = (type >> 8) & 0xFF;
    buf[11] = intervalData->PreCarNum & 0xFF;
    buf[12] = (intervalData->PreCarNum >>8) & 0xFF;
    buf[13] = intervalData->CurCarNum & 0xFF;
    buf[14] = (intervalData->CurCarNum >>8) & 0xFF;
    buf[15] = intervalData->interval & 0xFF;
    buf[16] = (intervalData->interval >>8) & 0xFF;  
    
    sum = buf[9];
    for (i = 1; i < sendlen - 9; i++)
    {
        sum ^= buf[9 + i];
    }
    buf[0] = 0xAA;
    buf[1] = 0xAA;
    buf[2] = 0x01;
    buf[3] = 0x00;
    buf[4] = 0x00;
    buf[5] = 0x00;
    buf[6] = sendlen & 0xFF;
    buf[7] = (sendlen >> 8) & 0xFF;
    buf[8] = sum;

    *len = sendlen;

    carAddSendMsgToQueue(CAR_MSG_GNDCTRL2WCS_CMD_INTERVAL_TYPE);
    positionreset = 30;
}


void send_msg_uart_two_data(u8* buf, u16* len, u16 type)
{
    u8  sum;
    u16 sendlen;
    u16 i;

    if (record_uart2_len == 0) {
      *len = 0;
        return;
    }
    if(usart2_sentcount !=0){
      *len = 0;
       return;
    }

    sendlen = 11 + record_uart2_len;



    buf[9] = type & 0xFF;
    buf[10] = (type >> 8) & 0xFF;

    for (i = 0; i < record_uart2_len; i++) {
        buf[11 + i] = record_uart2_buff[i];
    }

    sum = buf[9];
    for (i = 1; i < sendlen - 9; i++)
    {
        sum ^= buf[9 + i];
    }
    buf[0] = 0xAA;
    buf[1] = 0xAA;
    buf[2] = 0x01;
    buf[3] = 0x00;
    buf[4] = 0x00;
    buf[5] = 0x00;
    buf[6] = sendlen & 0xFF;
    buf[7] = (sendlen >> 8) & 0xFF;
    buf[8] = sum;

    *len = sendlen;

    record_uart2_len = 0;
    usart2_sentcount = 20;
}


void send_msg_uart_four_data(u8* buf, u16* len, u16 type)
{
    u8  sum;
    u16 sendlen;
    u16 i;

    if (record_uart4_len == 0) {
      *len = 0;
        return;
    }
    
    if(usart4_sentcount !=0){
      *len = 0;
       return;
    }

    sendlen = 11 + record_uart4_len;



    buf[9] = type & 0xFF;
    buf[10] = (type >> 8) & 0xFF;

    for (i = 0; i < record_uart4_len; i++) {
        buf[11 + i] = record_uart4_buff[i];
    }

    sum = buf[9];
    for (i = 1; i < sendlen - 9; i++)
    {
        sum ^= buf[9 + i];
    }
    buf[0] = 0xAA;
    buf[1] = 0xAA;
    buf[2] = 0x01;
    buf[3] = 0x00;
    buf[4] = 0x00;
    buf[5] = 0x00;
    buf[6] = sendlen & 0xFF;
    buf[7] = (sendlen >> 8) & 0xFF;
    buf[8] = sum;

    *len = sendlen;

    record_uart4_len = 0;
    usart4_sentcount = 20;
}

void send_msg_gnd_version_type(u8* buf, u16* len, u16 type)
{
    u8  sum;
    u16 sendlen;
    u16 i;

    sendlen = 11 + 4;

    buf[9] = type & 0xFF;
    buf[10] = (type >> 8) & 0xFF;
    buf[11] = version[0];
    buf[12] = version[1];
    buf[13] = version[2];
    buf[14] = version[3];

    sum = buf[9];
    for (i = 1; i < sendlen - 9; i++)
    {
        sum ^= buf[9 + i];
    }
    buf[0] = 0xAA;
    buf[1] = 0xAA;
    buf[2] = 0x01;
    buf[3] = 0x00;
    buf[4] = 0x00;
    buf[5] = 0x00;
    buf[6] = sendlen & 0xFF;
    buf[7] = (sendlen >> 8) & 0xFF;
    buf[8] = sum;

    *len = sendlen;

}

void send_heart_msg(u8 *buf, u16 *len, u16 type)
{
    u8 sum = (type & 0xFF) ^ (type >> 8);

    buf[0] = 0xAA;
    buf[1] = 0xAA;
    
    buf[2] = 0x02;//版本号
    buf[3] = 0x00;
    buf[4] = 0x01;
    buf[5] = 0x00;
    
    buf[6] = 0x0B;
    buf[7] = 0x00;
    buf[8] = sum;
    buf[9] = type & 0xFF;
    buf[10] = (type >> 8) & 0xFF;

    *len = 11;
}
void send_message_to_sever(void)
{
    u16 msg_type;
    u8 j = 0;

    if(tcp_client_list[0].tcp_send_en == 0)//tcp发送成功或没在发送
    {
        msg_type = GetSendMsgFromQueue();
    }
    else//tcp正在发送
    {
        return;
    }

    switch(msg_type)
    {
    case REPLY_RECV_MSG_WCS2GNDCTRL_PARA_TYPE:
        reply_recv_msg(&(tcp_client_list[0].tcp_send_buf[0]), &(tcp_client_list[0].tcp_send_len), msg_type);
        tcp_client_list[0].tcp_send_en = 1;
        break;
    case SEND_MSG_GNDCTRL2WCS_CMD_TYPE:
        j = IsGnd2WcsStateQueueFree();
        if (j == 0) {
            return;
        }
        send_msg_gndctrl2wcs_cmd(&(tcp_client_list[0].tcp_send_buf[0]), &(tcp_client_list[0].tcp_send_len), msg_type);
        tcp_client_list[0].tcp_send_en = 1;
        break;
//    case HEART_MSG_TYPE:
//        send_heart_msg(&(tcp_client_list[0].tcp_send_buf[0]), &(tcp_client_list[0].tcp_send_len), msg_type);
//        tcp_client_list[0].tcp_send_en = 1;
//        break;
    case SEND_MSG_GNDCTRL2WCS_CMD_INTERVAL_TYPE:
        send_msg_gndctrl2wcs_cmd_interval_data(&(tcp_client_list[0].tcp_send_buf[0]), &(tcp_client_list[0].tcp_send_len), msg_type);
        tcp_client_list[0].tcp_send_en = 1;
        break;
    case REPLY_RECV_MSG_REPAIR_CAR_CMD_TYPE:
        reply_recv_msg(&(tcp_client_list[0].tcp_send_buf[0]), &(tcp_client_list[0].tcp_send_len), msg_type);
        tcp_client_list[0].tcp_send_en = 1;
        break;
    case REPLY_WCS_GND_VERSION_TYPE:
        send_msg_gnd_version_type(&(tcp_client_list[0].tcp_send_buf[0]), &(tcp_client_list[0].tcp_send_len), msg_type);
        tcp_client_list[0].tcp_send_en = 1;
        break;
    case SEND_UART2_MSG:
        send_msg_uart_two_data(&(tcp_client_list[0].tcp_send_buf[0]), &(tcp_client_list[0].tcp_send_len), msg_type);
        tcp_client_list[0].tcp_send_en = 1;
        break;
    case SEND_UART4_MSG:
        send_msg_uart_four_data(&(tcp_client_list[0].tcp_send_buf[0]), &(tcp_client_list[0].tcp_send_len), msg_type);
        tcp_client_list[0].tcp_send_en = 1;
        break;
    default:
        if( tcp_client_list[0].tcp_client_statue == CLIENT_CONNECT_OK)
        {
            if( heart_dely == 0 )
            {
                send_heart_msg(&(tcp_client_list[0].tcp_send_buf[0]), &(tcp_client_list[0].tcp_send_len), HEART_MSG_TYPE);
                tcp_client_list[0].tcp_send_en = 1;
                heart_dely = HEART_DELAY;
            }
        }
        break;
    }
}

void fun_gnd2wcs_state_msg(void)
{
    u8 i = 0;

    i = IsGnd2WcsStateQueueFree();

    if (i != 0) {
        AddSendMsgToQueue(SEND_MSG_GNDCTRL2WCS_CMD_TYPE);
    }

}
