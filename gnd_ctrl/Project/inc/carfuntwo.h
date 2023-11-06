#ifndef _CARFUNTWO_H
#define _CARFUNTWO_H


extern MSG_SEND_QUEUE cartwoSendQueue;



void cartwoInitSendMsgQueue(void);
void cartwoAddSendMsgToQueue(u16 msg);
void cartworecv_message_from_sever(u8* point, u16* len);


void cartwosend_message_to_sever(void);

#endif