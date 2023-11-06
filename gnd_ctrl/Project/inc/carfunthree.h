#ifndef _CARFUNTHREE_H
#define _CARFUNTHREE_H


extern MSG_SEND_QUEUE carthreeSendQueue;



void carthreeInitSendMsgQueue(void);
void carthreeAddSendMsgToQueue(u16 msg);
void carthreerecv_message_from_sever(u8* point, u16* len);


void carthreesend_message_to_sever(void);

#endif