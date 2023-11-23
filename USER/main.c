#include <math.h>
#include "delay.h"
#include "sys.h"
#include "oled.h"
#include "bmp.h"
#include "key.h"
#include "Serial.h"
#include "LED.h"
#include "dht11.h"
#include "adc.h"
// mock
uint8_t currentTemp = 90;
uint8_t currentHumi = 50;
uint8_t isOpenPerson = 1;  // 1���� 0������
uint16_t currentSmoke = 500;
uint8_t hasPerson = 0;

// ��ֵ
uint8_t MaxTemp = 80;
uint8_t MinTemp = 20;
uint8_t MaxHumi = 80;
uint8_t MinHumi = 20;
uint16_t MaxSmoke = 600;

uint8_t t = 0;
uint8_t setn = 1;
uint8_t isFirstShow = 1;

// ����ҳ�� �ֿ��±�
typedef enum SetTypeFontIndex
{
    SENSOR_TEMP = 15,
    SENSOR_HUMI = 19,
    SENSOR_SMOKE = 23
} SetTypeFontIndex;

// �Ƿ�����ֵ��
typedef struct IsInRange
{
    u8 inMaxTempRange;
    u8 inMinTempRange;
    u8 inMaxHumiRange;
    u8 inMinHumiRange;
    u8 inMaxSmokeRange;
    u8 hasNoPerson;
} IsInRange;



void showMainPage(void);
void setSensorVal(void);
void handleKeyClick(void);
IsInRange inRange(void);
void handleAbnormal(void);
void getSensorVal(void);
int main(void)
{
    KEY_Init();
    Serial_Init();
    delay_init();
    OLED_Init();
    Adc_Init();
    LED_Init();
    while (DHT11_Init()) {}
    while (1)
    {
        OLED_Refresh();
        getSensorVal();
        if (isFirstShow)
        {
            showMainPage();
            isFirstShow = 0;
        }
        handleKeyClick();
        if (setn == 1)
        {
            handleAbnormal();
        }

    }

}

void showMainPage(void)
{
    OLED_Clear();
    uint8_t i;
    for (i = 0; i < 2; i++)  OLED_ShowChinese(i * 16 + 5, 0, i, 16, 1); // �¶�
    for (i = 0; i < 2; i++)  OLED_ShowChinese((i + 2) * 16 + 5 + 10, 0, i + 2, 16, 1); // ʪ��
    for (i = 0; i < 2; i++)  OLED_ShowChinese((i + 4) * 16 + 5 + 20, 0, i + 4, 16, 1); //����
    OLED_ShowChinese(5 + 16, 16, 10, 16, 1); // ���϶�
    for (i = 0; i < 2; i++)  OLED_ShowChinese(i * 16 + 5, 16 * 2, i + 8, 16, 1); // ����
    OLED_ShowString(5 + 16 * 2, 16 * 2, ": ", 16, 1);
    setSensorVal();
}
void getSensorVal()
{
    DHT11_Read_Data(&currentTemp, &currentHumi);
   // Serial_Printf("temp: %d", currentTemp);
   //Serial_Printf("humi: %d", currentHumi);
	uint16_t adcx=Get_Adc_Average(ADC_Channel_9,10);
	currentSmoke=adcx*((10000-300)/4096)+300;
	
}

void setSensorVal(void)
{
    if (setn == 1)
    {
        uint8_t i;
        char humiArr[4];
        char smokeArr[7];
        OLED_ShowNum(5, 16, currentTemp, 2, 16, 1); // �¶�ֵ
        sprintf(humiArr, "%d%%", currentHumi);
        sprintf(smokeArr, "%dppm", currentSmoke);
        //OLED_ShowNum(2*16+5+10,16,currentHumi,2,16,1);  // ʪ��ֵ
        OLED_ShowString(2 * 16 + 5 + 10, 16, (u8 *)humiArr, 16, 1);
        OLED_ShowChinese(4 * 16 + 5 + 20 + 8, 16, isOpenPerson == 1 ? 6 : 7, 16, 1); //  �����⿪��
        //OLED_ShowNum(5+16*3,16*2,currentSmoke,3,16,1); // ����ֵ
        OLED_ShowString(5 + 16 * 3, 16 * 2, (u8 *)smokeArr, 16, 1);
        if (hasPerson)
        {
            for (i = 0; i < 4; i++)  OLED_ShowChinese(i * 16 + 5, 16 * 3, i + 11, 16, 1); // ��⵽��
        }
        else
        {
            OLED_ShowString(5, 16 * 3, "        ", 16, 1);
        }
    }
    else if (setn == 2 || setn == 3)
    {

        OLED_ShowNum(16 * 3 + 5, 16 + 8,  MaxTemp, 2, 16, setn - 2); // �¶�������ʾ
        OLED_ShowNum(16 * 3 + 5, 16 * 2 + 8 + 8,  MinTemp, 2, 16,  setn - 3); // ������ʾ
    }
    else if (setn == 4 || setn == 5)
    {
        OLED_ShowNum(16 * 3 + 5, 16 + 8,  MaxHumi, 2, 16, setn - 4); // ʪ��������ʾ
        OLED_ShowNum(16 * 3 + 5, 16 * 2 + 8 + 8,  MinHumi, 2, 16, setn - 5); // ������ʾ
    }
    else
    {
        char smokeStr[7];
        sprintf(smokeStr, "%dppm", MaxSmoke); // ����+��λ
        OLED_ShowString(16 * 3 + 5, 16  + 8, (u8 *)smokeStr,  16, 0); // ����������ʾ
    }

}


// ��������ҳ�����Ĳ���
void setSensorPage(SetTypeFontIndex type)
{
    u8 i;
    OLED_Clear();
    for (i = 0; i < 4; i++)OLED_ShowChinese(i * 16 + 32, 0, i + type, 16, 1); //��ʾ���ģ��������¶ȡ�ʪ�ȡ�������
    for (i = 0; i < 2; i++)OLED_ShowChinese(i * 16 + 5, 16 + 8, i + 27, 16, 1); //��ʾ���ģ������ޡ�
    OLED_ShowString(5 + 32, 16 + 8, (u8 *)": ", 16, 1);
    if (type != SENSOR_SMOKE)
    {
        for (i = 0; i < 2; i++)OLED_ShowChinese(i * 16 + 5, 16 * 2 + 8 + 8, i + 29, 16, 1); //��ʾ���ģ������ޡ�
        OLED_ShowString(5 + 32, 16 * 2 + 8 + 8, (u8 *)": ", 16, 1);
    }
    if (type == SENSOR_TEMP)
    {
        OLED_ShowChinese(5 + 16 * 4, 16 + 8, 10, 16, 1); // ��
        OLED_ShowChinese(5 + 16 * 4,  16 * 2 + 8 + 8, 10, 16, 1); // ��
    }
    if (type == SENSOR_HUMI)
    {
        OLED_ShowString(5 + 16 * 4, 16 + 8, (u8 *)"%", 16, 1); //%
        OLED_ShowString(5 + 16 * 4,  16 * 2 + 8 + 8, (u8 *)"%", 16, 1); //%
    }
}


uint8_t pageArr[6] = {NULL, SENSOR_TEMP, SENSOR_TEMP, SENSOR_HUMI, SENSOR_HUMI, SENSOR_SMOKE};
void handleKeyClick()
{

    t = KEY_Scan(0);    //�õ���ֵ
    if (t == KEY0_PRES)
    {
        setn++;
        setn = setn > 6 ? 1 : setn;
        if (setn == 1)
        {
            showMainPage();
        }
        else
        {
            setSensorPage(pageArr[setn - 1]);
        }
    }
    if (t == KEY1_PRES || t == KEY2_PRES)
    {

        if (setn == 2 && MaxTemp > 0)  t == KEY1_PRES ? MaxTemp++ : MaxTemp--; // �����¶�����
        if (setn == 3 && MinTemp < MaxTemp)  t == KEY1_PRES ? MinTemp++ : MinTemp--; // �����¶�����
        if (setn == 4 && MaxHumi)  t == KEY1_PRES ? MaxHumi++ : MaxHumi--; // ����ʪ������
        if (setn == 5 && MinHumi < MaxHumi)  t == KEY1_PRES ? MinHumi++ : MinHumi--; // ����ʪ������
        if (setn == 6 && MaxSmoke > 0)  t == KEY1_PRES ? MaxSmoke++ : MaxSmoke--; // ������������
    }
    if (t == KEY3_PRES)
    {

        isOpenPerson = !isOpenPerson;

    }
    setSensorVal();
}
// ������ֵ��������
void handleAbnormal()
{
    u8 i;
    static u16 count = 0;
    static u8 isBlink = 1;
    isBlink = !isBlink;
    IsInRange judge = inRange();
    if (count++ < 20) return;  // ��˸�����ʱ
    count = 0;
    // OLED
    (!judge.hasNoPerson || !judge.inMaxHumiRange || !judge.inMaxSmokeRange || !judge.inMaxTempRange || !judge.inMaxTempRange || !judge.inMinHumiRange) ? LED1_ON() : LED1_OFF();
    // *��˸
    OLED_ShowString(5 + 16 + 16, 16, ((!judge.inMaxTempRange || !judge.inMinTempRange) && isBlink) ? (u8 *)"*" : (u8 *)" ", 16, 1); //*
    OLED_ShowString(47 + 32, 16, ((!judge.inMaxHumiRange || !judge.inMinHumiRange) && isBlink) ? (u8 *)"*" : (u8 *)" ", 16, 1); //*
    if (hasPerson && isBlink)
    {
        for (i = 0; i < 4; i++)OLED_ShowChinese(i * 16 + 5, 3 * 16, i + 11, 16, 1) ; // ��⵽��
    }
    else
    {
        OLED_ShowString(5, 3 * 16, (u8 *)"        ", 16, 1);  // ���
    }
}
// ��ֵ�ж�
IsInRange inRange(void)
{
    // c99
    IsInRange judge =
    {
        .inMaxTempRange = currentTemp <= MaxTemp,
        .inMinTempRange = currentTemp >= MinTemp,
        .inMaxHumiRange = currentHumi <= MaxHumi,
        .inMinHumiRange = currentHumi >= MinHumi,
        .inMaxSmokeRange = currentSmoke <= MaxSmoke,
        .hasNoPerson = !hasPerson
    };

    //  Serial_SendNumber(currentSmoke,3);
    //  Serial_SendNumber(MaxSmoke,3);
    return judge;
}

