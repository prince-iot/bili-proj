#ifndef __USART_H
#define __USART_H
#include "stdio.h"	
#include "sys.h" 

#define USART_DEBUG		USART1		//调试打印所使用的串口组

void usart1_init(u32 bound);
void usart3_init(u32 bound);
void USART1_IRQHandler(void);
void Usart_SendString(USART_TypeDef *USARTx, unsigned char *str, unsigned short len);
//void UsartPrintf(USART_TypeDef *USARTx, char *fmt,...);

#endif


