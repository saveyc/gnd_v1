#include "stm32f10x.h"
#include "main.h"
#include "TCPclient.h"

#define carSendQueueSize  50
u16 carQueueBuff[carSendQueueSize];
MSG_SEND_QUEUE carSendQueue;

void carinitQueue(MSG_SEND_QUEUE* q, u16 ms)
{
    q->maxSize = ms;
    q->queue = carQueueBuff;
    q->front = q->rear = 0;
}
void carenQueue(MSG_SEND_QUEUE* q, u16 x)
{
    //队列满
    if ((q->rear + 1) % q->maxSize == q->front)
    {
        return;
    }
    q->rear = (q->rear + 1) % q->maxSize; // 求出队尾的下一个位置
    q->queue[q->rear] = x; // 把x的值赋给新的队尾
}
u16 caroutQueue(MSG_SEND_QUEUE* q)
{
    //队列空
    if (q->front == q->rear)
    {
        return 0;
    }
    q->front = (q->front + 1) % q->maxSize; // 使队首指针指向下一个位置
    return q->queue[q->front]; // 返回队首元素
}
void carInitSendMsgQueue(void)
{
    carinitQueue(&carSendQueue, carSendQueueSize);
}
void carAddSendMsgToQueue(u16 msg)
{
    carenQueue(&carSendQueue, msg);
}
u16 carGetSendMsgFromQueue(void)
{
    return (caroutQueue(&carSendQueue));
}



u8 carrecv_msg_check(u8* point, u16 len)
{
    u16 i = 0;
    u8  sum = 0;

    if ((point[0] != 0xAA) || (point[1] != 0xAA))
        return 0;
    if ((point[6] | point[7] << 8) != len)
        return 0;
    sum = point[9];
    for (i = 1; i < len - 9; i++)
    {
        sum ^= point[9 + i];
    }
    //if (sum != point[8])
    //    return 0;

    return 1;
}
void carrecv_msg_process(u8* point)
{
    MSG_HEAD_DATA* head = (MSG_HEAD_DATA*)point;

    if (head->MSG_TYPE == RECV_MSG_BOOT_CMD_TYPE)
    {
        BKP_WriteBackupRegister(BKP_DR8, 0x55);
        NVIC_SystemReset();
    }
    switch (head->MSG_TYPE)
    {
    case RECV_MSG_REPAIR_CAR_CMD_TYPE:
        break;
    default:
        break;
    }
}
void carrecv_message_from_sever(u8* point, u16* len)
{
    if (carrecv_msg_check(point, *len) == 0)
    {
        *len = 0;
        return;
    }
    carrecv_msg_process(point);
    *len = 0;
}



//主动发送小车发生位置变化的间隔
void carsend_msg_interval_data(u8* buf, u16* len, u16 type)
{
    u8  sum;
    u16 sendlen;
    u16 i;

    sendlen = 11 + 4;

    buf[9] = type & 0xFF;
    buf[10] = (type >> 8) & 0xFF;
    buf[11] = carfunint.CurCarNum & 0xFF;
    buf[12] = (carfunint.CurCarNum >> 8) & 0xFF;
    buf[13] = carfunint.interval & 0xFF;
    buf[14] = (carfunint.interval >> 8) & 0xFF;

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


void carsend_message_to_sever(void)
{
    u16 msg_type;
    u8 j = 0;

    if (tcp_client_list[1].tcp_send_en == 0)//tcp发送成功或没在发送
    {
        msg_type = carGetSendMsgFromQueue();
    }
    else//tcp正在发送
    {
        return;
    }

    switch (msg_type)
    {

    case CAR_MSG_GNDCTRL2WCS_CMD_INTERVAL_TYPE:
        carsend_msg_interval_data(&(tcp_client_list[1].tcp_send_buf[0]), &(tcp_client_list[1].tcp_send_len), msg_type);
        tcp_client_list[1].tcp_send_en = 1;
        break;
    default:
        break;
    }
}

