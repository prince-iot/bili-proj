#ifndef __SR501_H
#define __SR501_H
#include "sys.h"

#define SR501  GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_9)//��ȡ����0

void SR501_Init(void);//IO��ʼ��
#endif
