#include "stm32f10x.h"
#include "main.h"
#include "TCPclient.h"

#define carthreeSendQueueSize  50
u16 carthreeQueueBuff[carthreeSendQueueSize];
MSG_SEND_QUEUE carthreeSendQueue;


u8  GndThreemsg[100];
u8  GndThreeDebug[100];

void carthreeinitQueue(MSG_SEND_QUEUE* q, u16 ms)
{
    q->maxSize = ms;
    q->queue = carthreeQueueBuff;
    q->front = q->rear = 0;
}

void carthreeenQueue(MSG_SEND_QUEUE* q, u16 x)
{
    //队列满
    if ((q->rear + 1) % q->maxSize == q->front)
    {
        return;
    }
    q->rear = (q->rear + 1) % q->maxSize; // 求出队尾的下一个位置
    q->queue[q->rear] = x; // 把x的值赋给新的队尾
}

u16 carthreeoutQueue(MSG_SEND_QUEUE* q)
{
    //队列空
    if (q->front == q->rear)
    {
        return 0;
    }
    q->front = (q->front + 1) % q->maxSize; // 使队首指针指向下一个位置
    return q->queue[q->front]; // 返回队首元素
}

void carthreeInitSendMsgQueue(void)
{
    carthreeinitQueue(&carthreeSendQueue, carthreeSendQueueSize);
}
void carthreeAddSendMsgToQueue(u16 msg)
{
    carthreeenQueue(&carthreeSendQueue, msg);
}
u16 carthreeGetSendMsgFromQueue(void)
{
    return (carthreeoutQueue(&carthreeSendQueue));
}



u8 carthreerecv_msg_check(u8* point, u16 len)
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
void carthreerecv_msg_process(u8* point)
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
void carthreerecv_message_from_sever(u8* point, u16* len)
{
    if (carthreerecv_msg_check(point, *len) == 0)
    {
        *len = 0;
        return;
    }
    carthreerecv_msg_process(point);
    *len = 0;
}



//主动发送小车发生位置变化的间隔
void carthreesend_msg_interval_data(u8* buf, u16* len, u16 type)
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

void send_msg_gndthree2wcs_cmd(u8* buf, u16* len, u16 type)
{
    sGnd2Wcs_state_node* node = 0;
    u8  sum = 0;
    u16 sendlen = 0;
    u16 i = 0;

    sendlen = sizeof(sGndCtrl2WCS_CMD_Data);
    buf[9] = type & 0xFF;
    buf[10] = (type >> 8) & 0xFF;
    buf[11] = GndThreemsg[0];
    buf[12] = GndThreemsg[1];
    buf[13] = GndThreemsg[2];
    buf[14] = GndThreemsg[3];
    buf[15] = GndThreemsg[4];
    buf[16] = GndThreemsg[5];
    buf[17] = GndThreemsg[6];
    buf[18] = GndThreemsg[7];
    buf[19] = GndThreemsg[8]; //170810
    buf[20] = GndThreemsg[9];
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

//主动发送小车发生位置变化的间隔
void send_msg_gndthree2wcs_data(u8* buf, u16* len, u16 type)
{
    u8  sum;
    u16 sendlen;
    u16 i;

    sendlen = 11 + 6;



    buf[9] = type & 0xFF;
    buf[10] = (type >> 8) & 0xFF;
    buf[11] = GndThreeDebug[0];
    buf[12] = GndThreeDebug[1];
    buf[13] = GndThreeDebug[2];
    buf[14] = GndThreeDebug[3];
    buf[15] = GndThreeDebug[4];
    buf[16] = GndThreeDebug[5];

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


void carthreesend_message_to_sever(void)
{
    u16 msg_type;
    u8 j = 0;

    if (tcp_client_list[3].tcp_send_en == 0)//tcp发送成功或没在发送
    {
        msg_type = carthreeGetSendMsgFromQueue();
    }
    else//tcp正在发送
    {
        return;
    }

    switch (msg_type)
    {

    case CAR_MSG_GNDCTRL2WCS_CMD_INTERVAL_TYPE:
        carthreesend_msg_interval_data(&(tcp_client_list[3].tcp_send_buf[0]), &(tcp_client_list[3].tcp_send_len), msg_type);
        tcp_client_list[3].tcp_send_en = 1;
        break;
    case SEND_MSG_GNDCTRL2WCS_CMD_TYPE:
        send_msg_gndthree2wcs_cmd(&(tcp_client_list[3].tcp_send_buf[0]), &(tcp_client_list[3].tcp_send_len), msg_type);
        tcp_client_list[3].tcp_send_en = 1;
        break;
    case SEND_MSG_GNDCTRL2WCS_CMD_INTERVAL_TYPE:
        send_msg_gndthree2wcs_data(&(tcp_client_list[3].tcp_send_buf[0]), &(tcp_client_list[3].tcp_send_len), msg_type);
        tcp_client_list[3].tcp_send_en = 1;
        break;
    default:
        break;
    }
}

