#include "SR501.h"

//按键初始化函数
void SR501_Init(void)
{

    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE); //使能PORTA,PORTC时钟

    GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_9; //PA15
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
    GPIO_Init(GPIOB, &GPIO_InitStructure);//初始化GPIOA15

}