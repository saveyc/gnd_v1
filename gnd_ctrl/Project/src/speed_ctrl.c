#include "main.h"
#include "math.h"
#include "speed_ctrl.h"

/*
����ֵ��
28hz--2m/s--pe_time:7500
15hz--1m/s--pe_time:1500
*/

//ȫ�ֱ���--
#define D_CTRL_OPEN         //΢�ֻ�������

#define SPEED_TARGET_TEST 1   //�����ٶ�ֵ  1m/s
#define SPEED_FACTOR  2000    //�ٶ����Ƶ���ı任ϵ�� 
#define PE_DISTANCE   75000   //������翪�ؾ���:75um
#define fwErrIMax     0.25     //����������
#define fwOutHzMax    5000    //��Ƶ�����Ƶ�����(��Χ:0~5000) 4500--45Hz 


/*���²�����ҪWCSϵͳ������*/
float max_speed ;                //����ٶȣ���WCS�趨
float min_speed ;                //��С�ٶȣ���WCS�趨
float fwKp = 10, fwKi = 5, fwKd = 40; //2016.04.25  ͨ��WCS�趨
float speed_target;              //Ŀ���ٶ� *m/s  ������WCSϵͳ�趨 ����ʱ��SPEED_TARGET_TEST
float speed_target_last;
//int   pe_time_target;          //��Ŀ���ٶ��£���ʱ����Ŀ��ֵ�����㹫ʽΪpe_time_target=PE_DISTANCE/speed_target
int   speed_factor;              //ͨ������£�Ŀ���ٶ����Ƶ��Ƶ�ʵ�ϵ��,ĿǰԤ��Ϊ1000
int   pe_distance;               //������翪�ؾ��� ͨ��ΪPE_DISTANCE=75000um



/**************************/
float fwErr[2], fwErrD = 0,  fwErrI = 0; //err[0]: ��һ�ε���err[1]�� ��ǰ���
float fwbV, fwbVLast;                    //�ٶȷ���ֵ����Ҫͨ�����緢�͸�WCS ��λΪm/s
int   fwHz;                              //speed reference to RS485
int   ctrlT;                             //��������
//unsigned char init_flag;                 //������־λ 1�����豸�����������̣�0����������ϡ�
//unsigned int  ctrl_times;
//unsigned char speed_vote_times;


//*------------------------------------------------------------------
//��ʼ������
//initialize the servo, finish initialization of servo
//inupt:val  0~4000
int RS485_OUT(int val)
{
    //ͨ��RS485���val
    servo_hz = val;
    
    ServoFreqSet(val);

    return 0;
}

int fwInitServo(void)
{
    speed_factor = SPEED_FACTOR;
    fwbV = 0;
    fwbVLast = 0;
    fwErr[0] = 0;
    fwErr[1] = 0;
    fwErrI = 0;
    fwErrD = 0;
    fwHz = 0;
	//init_flag = 0;
	//ctrl_times = 0;
	//speed_vote_times = 0;
        
    adjust_stage_flag = 0;
    first_stage_cnt = 0;
    second_stage_cnt = 0;
    //stop_time_delay = 0;
    /*������*/
    //    max_speed = 2.2;     //����ٶ�2.2��/��
    //    min_speed = 0.0;     //����ٶ�0.0��/��   ��WCS�趨
    //    pe_distance = PE_DISTANCE;
    //    if(speed_target == 0 )
    //    {
    //       speed_target  = SPEED_TARGET_TEST;
    //    }
    //
    //    fwHz = (int)(speed_target*speed_factor); //�趨һ��������ʼĿ�ٶ�;
    //    RS485_OUT(fwHz);  //485�������Ƶ��
    //
    //    return 0;
    max_speed = wcs2GndParaData.max_speed;
    min_speed = wcs2GndParaData.min_speed;
    fwKp =(float) wcs2GndParaData.fwKp;
    fwKi =(float) wcs2GndParaData.fwKi;
    fwKd =(float) wcs2GndParaData.fwKd;
    speed_target = wcs2GndParaData.speed_target;
    speed_factor = wcs2GndParaData.speed_factor;
    pe_distance  = wcs2GndParaData.pe_distance;
    second_stage_hz = wcs2GndParaData.standard_hz;//170811

    first_stage_hz = (int)(speed_target * speed_factor); //�趨һ��������ʼĿ�ٶ�;
    if(first_stage_hz > fwOutHzMax)
	first_stage_hz = fwOutHzMax;
    
    if(second_stage_hz == 0)//PID����ʱ����ʼƵ��
        fwHz = first_stage_hz;
    else
        fwHz = second_stage_hz;
    
    return first_stage_hz;
}


//�����㣬���仯��
//in:    pe_time    ������翪��֮���ʱ��ֵ ��λus
int fwErrCalculate(int pe_time)
{
    //���������ٶ�
    fwbV = (pe_distance / (float)pe_time);


//	if( fwbV < 0.5 )  //����ٶȵ���0.5��/�룬�豸��������ģʽ
//	{
//		init_flag = 1;
//		ctrl_times = 0;
//		speed_vote_times = 0;
//	}

    //�������
    fwErr[0] = fwErr[1];//err[0]:��һ�ε����ֵ��err[1]:��ǰ���ֵ
    fwErr[1] = speed_target - fwbV;

    /*
    if(speed_target_last == 0.5)
    {
      speed_target_last = speed_target;
      fwErr[1] = speed_target-fwbV;
      return 0;
    }
    */

#ifdef D_CTRL_OPEN
    //΢�����
    //fwErrD = (fwErr[1]-fwErr[0])/ctrlF;
    fwErrD = (fwErr[1] - fwErr[0]);
    if (fabs(fwErrD) > 0.1) //����ļ��ٶȲ����������
        fwErrD = 0;
    if (fwErr[0] == 0 || fwErr[0] == fwErr[1])//�����δ���������������ʱ���ٹ��� ,������΢�ֿ���
        fwErrD = 0;
#endif
    //�������
    if (fabs(fwErr[1]) <= fwErrIMax) //ֻ�н���PID�����ſ�ʼ����
    {
        if ((fabs(fwErrI) >= fabs(fwErrIMax)) && (fwErrI * fwErr[1] > 0)) //������������
        {
            return 0;
        }

        fwErrI += fwErr[1];

        if (fabs(fwErrI) >= fwErrIMax) // ���ܳ������ֵ
        {
            fwErrI = fwErrI > 0 ?  fwErrIMax : (-fwErrIMax);
        }
    }
    else
        fwErrI = 0;


    if(fwErr[0]*fwErr[1] < 0) //�ٶȴ�С���趨�ٶ������޷�����Ծ
    {
        fwErrI = 0;
    }


    return 0;

}

//PID���ƣ��ֶα����PID�����㷨���Ĵ�С�ɸ��ݾ������������ֻҪ���Ʊ�����������ı仯��ƽ�ȼ���
//���㷨����Ҫ˼�룺����ϵͳ���ļ�С���ʵ���С��������΢��ϵ�������Ӵ����ϵ����
//��С���ʱȡ��΢�֣�����΢��������š���ϵͳ��������΢�ֶ��ӽ���ʱ��
//����ջ�����������ּ������ô����ĸ���Ӱ�졣
//ע��������λ����������ʱ�����Ƶ�������뱣֤����������λ��
//in:    pe_time    ������翪��֮���ʱ��ֵ ��λus
int fwPIDctrl(int pe_time)
{
    fwErrCalculate(pe_time); 	

	/*
	if(fabs(fwErr[1]) < 0.10) //�ٶ��ȶ�����
	{
        speed_vote_times++;
		if(speed_vote_times == 60)
		{
			speed_vote_times = 0;
			init_flag = 0;  
			ctrl_times = 0;
		}
	}
	else
	{
        speed_vote_times = 0;
	}
		
	if( init_flag == 1 ) //��������
	{
		ctrl_times++;		
		if(ctrl_times<60)
		{
			fwHz = 2000;//20Hz
			RS485_OUT(fwHz);			
			return 0;
		}
		else
		{
			init_flag = 0;
			ctrl_times = 0;			
		}
	}
    */    
	
	//�ֶ�PID����

	if( (fwErr[1]) < -0.5 )//fwErr[1] = speed_target - fwbV;
	{	   
		fwHz = (int)( fwHz + fwKp * fwErr[1] * 20.0 + fwKi * fwErrI + fwKd * fwErrD ); 
	}
	else if( (fwErr[1]) < -0.30 )
	{
		fwHz = (int)( fwHz + fwKp * fwErr[1] * 15.0 + fwKi * fwErrI + fwKd * fwErrD ); 	  
	}
	else if( (fwErr[1]) < -0.20 )
	{
		fwHz = (int)( fwHz + fwKp * fwErr[1] * 2.0 + fwKi * fwErrI + fwKd * fwErrD );		 
	}
	else if( (fwErr[1]) < -0.15 )
	{
		fwHz = (int)( fwHz + fwKp * fwErr[1] * 1.5 + fwKi * fwErrI + fwKd * fwErrD );		 
	}
	else if( (fwErr[1]) < -0.10 )
	{
		fwHz = (int)( fwHz + fwKp * fwErr[1] * 1.0 + fwKi * fwErrI + fwKd * fwErrD );		 
	}	   
	else if( (fwErr[1]) < -0.05 )
	{
		fwHz = (int)( fwHz + fwKp * fwErr[1] * 0.9 + fwKi * fwErrI + fwKd * fwErrD );		 
	}  
	else if( (fwErr[1]) < -0.03 )
	{
		fwHz = (int)( fwHz + fwKp * fwErr[1] * 0.8 + fwKi * fwErrI + fwKd * fwErrD );		 
	} 
	else if( (fwErr[1]) < -0.01 )
	{
		fwHz = (int)( fwHz + fwKp * fwErr[1] * 0.7 + fwKi * fwErrI + fwKd * fwErrD );	   
	}
	else if( (fwErr[1]) < 0 )
	{
		fwHz = (int)( fwHz + fwKp * fwErr[1] * 0.6 + fwKi * fwErrI + fwKd * fwErrD );	   
	}
	if(fwErr[1] >= 0.05)
	{
		fwHz = (int)( fwHz + fwKp * fwErr[1] * 0.3 + fwKi * fwErrI + fwKd * fwErrD + 0.5);
	}		
    if( (fwErr[1] > 0) && (fwErr[1] < 0.05) )
    {
        fwHz = (int)( fwHz + fwKp * fwErr[1] * 0.6 + fwKi * fwErrI + fwKd * fwErrD + 1.0 );
    }	
	
	
    if(fabs(fwErr[1]) <= 0.001 && fabs(fwErrD) <= 1) //������ɣ���������� ǧ��֮һ����
    {
        fwErrI = 0;
    }

    if(fwHz > fwOutHzMax)
    {
        fwHz = fwOutHzMax;
    }

    if(fwHz < 0)
    {
        fwHz =  0;
    }
    
    if (keep_in_paire == REPAIRE_NONE) {
        RS485_OUT(fwHz);
    }                                          //485�������Ƶ��

    return 0;

}
