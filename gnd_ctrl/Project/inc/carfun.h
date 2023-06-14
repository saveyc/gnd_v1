#ifndef _CARFUN_H
#define _CARFUN_H


extern MSG_SEND_QUEUE carSendQueue;


#define   CAR_MSG_GNDCTRL2WCS_CMD_INTERVAL_TYPE  0xA1A1

void carInitSendMsgQueue(void);
void carAddSendMsgToQueue(u16 msg);
void carrecv_message_from_sever(u8* point, u16* len);


void carsend_message_to_sever(void);

#endif