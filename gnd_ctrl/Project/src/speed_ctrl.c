#include "main.h"
#include "math.h"
#include "speed_ctrl.h"

/*
经验值：
28hz--2m/s--pe_time:7500
15hz--1m/s--pe_time:1500
*/

//全局变量--
#define D_CTRL_OPEN         //微分环节启动

#define SPEED_TARGET_TEST 1   //测试速度值  1m/s
#define SPEED_FACTOR  2000    //速度与变频器的变换系数 
#define PE_DISTANCE   75000   //两个光电开关距离:75um
#define fwErrIMax     0.25     //积分限制项
#define fwOutHzMax    5000    //变频器最大频率输出(范围:0~5000) 4500--45Hz 


/*以下参数需要WCS系统来配置*/
float max_speed ;                //最大速度，由WCS设定
float min_speed ;                //最小速度，由WCS设定
float fwKp = 10, fwKi = 5, fwKd = 40; //2016.04.25  通过WCS设定
float speed_target;              //目标速度 *m/s  来自于WCS系统设定 测试时用SPEED_TARGET_TEST
float speed_target_last;
//int   pe_time_target;          //在目标速度下，计时器的目标值，计算公式为pe_time_target=PE_DISTANCE/speed_target
int   speed_factor;              //通常情况下，目标速度与变频器频率的系数,目前预测为1000
int   pe_distance;               //两个光电开关距离 通常为PE_DISTANCE=75000um



/**************************/
float fwErr[2], fwErrD = 0,  fwErrI = 0; //err[0]: 上一次的误差，err[1]： 当前误差
float fwbV, fwbVLast;                    //速度反馈值，需要通过网络发送给WCS 单位为m/s
int   fwHz;                              //speed reference to RS485
int   ctrlT;                             //控制周期
//unsigned char init_flag;                 //启动标志位 1代表设备处于启动过程，0代表启动完毕。
//unsigned int  ctrl_times;
//unsigned char speed_vote_times;


//*------------------------------------------------------------------
//初始化程序
//initialize the servo, finish initialization of servo
//inupt:val  0~4000
int RS485_OUT(int val)
{
    //通过RS485输出val
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
    /*测试用*/
    //    max_speed = 2.2;     //最高速度2.2米/秒
    //    min_speed = 0.0;     //最低速度0.0米/秒   由WCS设定
    //    pe_distance = PE_DISTANCE;
    //    if(speed_target == 0 )
    //    {
    //       speed_target  = SPEED_TARGET_TEST;
    //    }
    //
    //    fwHz = (int)(speed_target*speed_factor); //设定一个启动初始目速度;
    //    RS485_OUT(fwHz);  //485输出到变频器
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

    first_stage_hz = (int)(speed_target * speed_factor); //设定一个启动初始目速度;
    if(first_stage_hz > fwOutHzMax)
	first_stage_hz = fwOutHzMax;
    
    if(second_stage_hz == 0)//PID调节时的起始频率
        fwHz = first_stage_hz;
    else
        fwHz = second_stage_hz;
    
    return first_stage_hz;
}


//误差计算，误差变化率
//in:    pe_time    两个光电开关之间的时间值 单位us
int fwErrCalculate(int pe_time)
{
    //计算主线速度
    fwbV = (pe_distance / (float)pe_time);


//	if( fwbV < 0.5 )  //如果速度低于0.5米/秒，设备进入启动模式
//	{
//		init_flag = 1;
//		ctrl_times = 0;
//		speed_vote_times = 0;
//	}

    //比例误差
    fwErr[0] = fwErr[1];//err[0]:上一次的误差值，err[1]:当前误差值
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
    //微分误差
    //fwErrD = (fwErr[1]-fwErr[0])/ctrlF;
    fwErrD = (fwErr[1] - fwErr[0]);
    if (fabs(fwErrD) > 0.1) //过大的加速度不能引入控制
        fwErrD = 0;
    if (fwErr[0] == 0 || fwErr[0] == fwErr[1])//电机还未启动或刚启动，此时加速过大 ,不加入微分控制
        fwErrD = 0;
#endif
    //积分误差
    if (fabs(fwErr[1]) <= fwErrIMax) //只有进入PID区域后才开始积分
    {
        if ((fabs(fwErrI) >= fabs(fwErrIMax)) && (fwErrI * fwErr[1] > 0)) //积分遇限削弱
        {
            return 0;
        }

        fwErrI += fwErr[1];

        if (fabs(fwErrI) >= fwErrIMax) // 不能超过最大值
        {
            fwErrI = fwErrI > 0 ?  fwErrIMax : (-fwErrIMax);
        }
    }
    else
        fwErrI = 0;


    if(fwErr[0]*fwErr[1] < 0) //速度大小与设定速度上下限发生跳跃
    {
        fwErrI = 0;
    }


    return 0;

}

//PID控制：分段变参数PID控制算法区的大小可根据具体情况来定，只要控制变量在这个区的变化较平稳即可
//该算法的主要思想：随着系统误差的减小，适当减小比例数和微分系数，并加大积分系数。
//在小误差时取消微分，避免微分引入干扰。在系统误差和误差的微分都接近零时，
//则清空积分项，消除积分记忆作用带来的负面影响。
//注：在软限位附近作调节时，控制的输出必须保证不超过软限位。
//in:    pe_time    两个光电开关之间的时间值 单位us
int fwPIDctrl(int pe_time)
{
    fwErrCalculate(pe_time); 	

	/*
	if(fabs(fwErr[1]) < 0.10) //速度稳定评定
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
		
	if( init_flag == 1 ) //启动过程
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
	
	//分段PID调节

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
	
	
    if(fabs(fwErr[1]) <= 0.001 && fabs(fwErrD) <= 1) //调节完成，清除积分项 千分之一精度
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
    }                                          //485输出到变频器

    return 0;

}
