#ifndef _FUN_H
#define _FUN_H

#pragma pack (1)
/* ����ϵͳ�������ٸ�WCS,֡���0x1201*/
typedef struct{
    u8  cmd[11];
    u32 speed;//�����ٶ�
    u16 carNo;//����ͷ��С����
    //u16 carState;//С��״̬(���ް���)
    u8  photoAlarm;//��籨���� 18/11/23
    u8  backup;//���� 18/11/23
    u16 fwHz;//���͸���Ƶ����Ƶ��ֵ
}sGndCtrl2WCS_CMD_Data;

/*****************************************************/
/* WCS���͵������ϵͳ����������ϵͳ,֡���0x1220*/
typedef struct{
    u8  cmd[11];
    float max_speed;
    float min_speed;
    u32 fwKp;
    u32 fwKi;
    u32 fwKd;
    float speed_target;
    u32 speed_factor;
    u32 pe_distance;
    u16 standard_hz;//170811
}sWCS2GndCtrl_Para_Data;

/* �������ϵͳ���ͷ�����Ϣ��WCS,֡���0x9220*/
typedef struct{
    u8 cmd[11];
}sWCS2GndCtrl_Para_Data_ACK;

/* WCS����ά����λ����Ϣ������ϵͳ,֡����: 0x1221 */
typedef struct{
    u8  cmd[11];
    u16 car_no;
    u16 repair_locate;
    u16 car_tot_num;
    u16 standard_hz;
}sRepairCar_CMD_Data;

#pragma pack ()

/********�����**********/
#define		MSG_NULL_TYPE			      0x0000
/**********��������***********/
#define         HEART_MSG_TYPE                        0x1150
/**********������������*********/
#define		SEND_MSG_GNDCTRL2WCS_CMD_TYPE	      0x1201
/**********�������ͻظ�����*********/

/**********��������*********/
#define		RECV_MSG_WCS2GNDCTRL_PARA_TYPE	      0x1220
#define         RECV_MSG_REPAIR_CAR_CMD_TYPE          0x1221
/**********�ظ���������*********/
#define		REPLY_RECV_MSG_WCS2GNDCTRL_PARA_TYPE  0x9220
#define     REPLY_RECV_MSG_REPAIR_CAR_CMD_TYPE    0x9221
#define         RECV_MSG_BOOT_CMD_TYPE                0x1F01

#pragma pack (1) 
typedef struct {
	u8  MSG_TAG[2];
        u32 MSG_ID;
        u16 MSG_LENGTH;
	u8  MSG_CRC;
	u16 MSG_TYPE;
}MSG_HEAD_DATA;
#pragma pack () 

typedef struct {
        u16 *queue; /* ָ��洢���е�����ռ� */
        u16 front, rear, len; /* ����ָ�루�±꣩����βָ�루�±꣩�����г��ȱ��� */
        u16 maxSize; /* queue���鳤�� */
}MSG_SEND_QUEUE;

void InitSendMsgQueue(void);
void AddSendMsgToQueue(u16 msg);
u16 GetSendMsgFromQueue(void);
void recv_message_from_sever(u8 *point,u16 *len);
void send_message_to_sever(void);
void InitCarStateData(void);
u8 recv_msg_check(u8 *point,u16 len);

extern sGndCtrl2WCS_CMD_Data  gnd2WcsCmdData;
extern sWCS2GndCtrl_Para_Data wcs2GndParaData;
extern sRepairCar_CMD_Data    repairCarCmdData;

#endif