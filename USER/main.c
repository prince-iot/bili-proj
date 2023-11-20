#include "delay.h"
#include "sys.h"
#include "oled.h"
#include "bmp.h"
#include "key.h"
#include "Serial.h"
// mock
uint8_t currentTemp = 20;
uint8_t currentHumi = 50;
uint8_t isOpenPerson = 1;  // 1���� 0������
uint16_t currentSmoke = 500;
uint8_t hasPerson = 1;

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

void showMainPage(void);
void setSensorVal(void);
void handleKeyClick(void);
int main(void)
{
    KEY_Init();
    Serial_Init();
    delay_init();
    OLED_Init();
    while (1)
    {
        OLED_Refresh();
        if (isFirstShow)
        {
            showMainPage();
            isFirstShow = 0;
        }
        handleKeyClick();
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
void setSensorVal(void)
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


// ��������ҳ�����Ĳ���
void setSensorPage(SetTypeFontIndex type)
{
    u8 i;
    OLED_Clear();
    for (i = 0; i < 4; i++)OLED_ShowChinese(i * 16 + 32, 0, i + type, 16, 1); //��ʾ���ģ��������¶ȡ�ʪ�ȡ�����
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
            setSensorPage(pageArr[setn-1]);
        }
    }
}


