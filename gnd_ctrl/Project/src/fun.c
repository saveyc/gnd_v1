#include "main.h"
#include "TCPclient.h"

sGndCtrl2WCS_CMD_Data   gnd2WcsCmdData;
sWCS2GndCtrl_Para_Data  wcs2GndParaData;
sRepairCar_CMD_Data     repairCarCmdData;

#define msgSendQueueSize  100
u16 msgQueueBuff[msgSendQueueSize];
MSG_SEND_QUEUE msgSendQueue;

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
        fwInitHz = fwInitServo();
        servo_start = 0;
        break;
    case RECV_MSG_REPAIR_CAR_CMD_TYPE:
        repairCarCmdData = *((sRepairCar_CMD_Data *)point);
        repair_car_para_init();
        AddSendMsgToQueue(REPLY_RECV_MSG_REPAIR_CAR_CMD_TYPE);
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
    u16 usrCarNo = gnd2WcsCmdData.carNo;
    u8  sum;
    u16 sendlen;
    u16 i;
    
    if(usrCarNo >0)//WCS小车号从0开始
        usrCarNo--;
    //UartSendCarNo(usrCarNo);//debug
    sendlen = sizeof(sGndCtrl2WCS_CMD_Data);
    buf[9] = type & 0xFF;
    buf[10] = (type >> 8) & 0xFF;
    buf[11] = gnd2WcsCmdData.speed & 0xFF;
    buf[12] = (gnd2WcsCmdData.speed >>8) & 0xFF;
    buf[13] = (gnd2WcsCmdData.speed >>16) & 0xFF;
    buf[14] = (gnd2WcsCmdData.speed >>24) & 0xFF;
    buf[15] = usrCarNo & 0xFF;
    buf[16] = (usrCarNo >>8) & 0xFF;
    buf[17] = 0;
    buf[18] = 0;
    buf[19] = servo_hz & 0xFF; //170810
    buf[20] = (servo_hz >>8) & 0xFF;
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
void send_heart_msg(u8 *buf, u16 *len, u16 type)
{
    u8 sum = (type & 0xFF) ^ (type >> 8);

    buf[0] = 0xAA;
    buf[1] = 0xAA;
    
    buf[2] = 0x05;//版本号
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
        send_msg_gndctrl2wcs_cmd(&(tcp_client_list[0].tcp_send_buf[0]), &(tcp_client_list[0].tcp_send_len), msg_type);
        tcp_client_list[0].tcp_send_en = 1;
        break;
//    case HEART_MSG_TYPE:
//        send_heart_msg(&(tcp_client_list[0].tcp_send_buf[0]), &(tcp_client_list[0].tcp_send_len), msg_type);
//        tcp_client_list[0].tcp_send_en = 1;
//        break;
    case REPLY_RECV_MSG_REPAIR_CAR_CMD_TYPE:
        reply_recv_msg(&(tcp_client_list[0].tcp_send_buf[0]), &(tcp_client_list[0].tcp_send_len), msg_type);
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
