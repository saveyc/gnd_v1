#ifndef SPEED_CTRL_H
#define SPEED_CTRL_H
extern int RS485_OUT(int val);
extern int fwInitServo(void);
extern int fwErrCalculate(int pe_time);
extern int fwPIDctrl(int pe_time);
#endif
