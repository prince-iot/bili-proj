#include "SR501.h"

//������ʼ������
void SR501_Init(void)
{

    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE); //ʹ��PORTA,PORTCʱ��

    GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_9; //PA15
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
    GPIO_Init(GPIOB, &GPIO_InitStructure);//��ʼ��GPIOA15

}