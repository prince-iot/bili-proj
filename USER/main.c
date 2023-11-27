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
#include "SR501.h"
#include "Motor.h"
#include "esp8266.h"
#include "usart.h"
#include "esp8266.h"
#include "serve.h"
#include "protocol.h"
#include "Store.h"

typedef enum 
{
    SENSOR_TEMP = 15,
    SENSOR_HUMI = 19,
    SENSOR_SMOKE = 23
} SetTypeFontIndex;

typedef struct 
{
    u8 inMaxTempRange;
    u8 inMinTempRange;
    u8 inMaxHumiRange;
    u8 inMinHumiRange;
    u8 inMaxSmokeRange;
    u8 hasNoPerson;
} IsInRange;



const char *devSubTopic[] = {"/k0hfv4sYShN/PrinceIot_Device/user/get"}; //订阅主题
const char devPubTopic[] = "/k0hfv4sYShN/PrinceIot_Device/user/post";   //发布主题

unsigned short timeCount = 0;   //发送间隔变量
unsigned short timeCount_date = 0;  //发送间隔变量
unsigned char *dataPtr = NULL;  //esp8266正常运作检查指针
extern char PUB_BUF1[256];
uint8_t t = 0;
uint8_t setn = 1;
uint8_t isFirstShow = 1;

Range range;
CurrentVal currentVal;

void showMainPage(void);
void setSensorVal(void);
void handleKeyClick(void);
IsInRange inRange(void);
void handleAbnormal(void);
void getSensorVal(void);
void net_task(void) ;
void initVal(void);

int main(void)
{
    KEY_Init();
    // Serial_Init();
    initVal();
    delay_init();
    OLED_Init();
    Adc_Init();
    LED_Init();
    SR501_Init();
    Motor_Init();
	getFlashVal(&range,&currentVal.isOpenPerson);
    uart_init(115200);//串口1初始化
    uart3_init(115200);//串口3初始化
    ESP8266_Init();
    while (Ali_DevLink())           //接入Ali
        delay_ms(500);
    Ali_Subscribe(devSubTopic, 1);
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
        net_task();
    }
}
void initVal(void)
{
    range.MaxHumi = 60;
    range.MaxSmoke = 200;
    range.MaxTemp = 50;
    range.MinHumi = 10;
    range.MinTemp = 10;

    currentVal.currentHumi = 0;
    currentVal.currentSmoke = 0;
    currentVal.currentTemp = 0;
    currentVal.hasPerson = 0;
    currentVal.isOpenPerson = 0;

}
//物联网控制函数，向服务器发送数据、接收互联网数据控制外设
void net_task(void)
{
    timeCount++;
    timeCount_date++;
    if (timeCount_date % 25 == 0)
    {
        sprintf(PUB_BUF1, "{\"params\":{\"temp\":%d,\"humi\":%d,\"smoke\":%d, \"hasPerson\":%d},\"method\":\"thing.event.property.post\"}", currentVal.currentTemp, currentVal.currentHumi, currentVal.currentSmoke, currentVal.hasPerson);  //发布数据格式
        //向服务器发布缓冲区PUB_BUF1信息，即发布湿度、温度值
        Ali_Publish("/sys/k0hfv4sYShN/PrinceIot_Device/thing/event/property/post", PUB_BUF1);
        timeCount_date = 0;
    }
    if (++timeCount >= 5000)                                //发送间隔
    {
        AliNet_Ping();      //保活，否则可能会连接中断
        timeCount = 0;
    }
}

// 显示数据页面
void showMainPage(void)
{
    OLED_Clear();
    uint8_t i;
    for (i = 0; i < 2; i++)  OLED_ShowChinese(i * 16 + 5, 0, i, 16, 1);
    for (i = 0; i < 2; i++)  OLED_ShowChinese((i + 2) * 16 + 5 + 10, 0, i + 2, 16, 1);
    for (i = 0; i < 2; i++)  OLED_ShowChinese((i + 4) * 16 + 5 + 20, 0, i + 4, 16, 1);
    OLED_ShowChinese(5 + 16, 16, 10, 16, 1);
    for (i = 0; i < 2; i++)  OLED_ShowChinese(i * 16 + 5, 16 * 2, i + 8, 16, 1);
    OLED_ShowString(5 + 16 * 2, 16 * 2, ": ", 16, 1);
    setSensorVal();
}
// 获取传感器信息
void getSensorVal()
{
    DHT11_Read_Data(&currentVal.currentTemp, &currentVal.currentHumi);
    uint16_t adcx = Get_Adc_Average(ADC_Channel_9, 10);
    currentVal.currentSmoke = adcx * ((10000 - 300) / 4096) + 300;
    currentVal.hasPerson = SR501;
}

// 页面切换+数据选中
void setSensorVal(void)
{
    if (setn == 1)
    {
        uint8_t i;
        char humiArr[4];
        char smokeArr[7];
        OLED_ShowNum(5, 16, currentVal.currentTemp, 2, 16, 1);
        sprintf(humiArr, "%d%%", currentVal.currentHumi);
        sprintf(smokeArr, "%dppm", currentVal.currentSmoke);
        OLED_ShowString(2 * 16 + 5 + 10, 16, (u8 *)humiArr, 16, 1);
        OLED_ShowChinese(4 * 16 + 5 + 20 + 8, 16, currentVal.isOpenPerson == 1 ? 6 : 7, 16, 1);
        OLED_ShowString(5 + 16 * 3, 16 * 2, (u8 *)smokeArr, 16, 1);
        if (currentVal.hasPerson && currentVal.isOpenPerson)
        {
            for (i = 0; i < 4; i++)  OLED_ShowChinese(i * 16 + 5, 16 * 3, i + 11, 16, 1);
        }
        else
        {
            OLED_ShowString(5, 16 * 3, "        ", 16, 1);
        }
    }
    else if (setn == 2 || setn == 3)
    {
        OLED_ShowNum(16 * 3 + 5, 16 + 8,  range.MaxTemp, 2, 16, setn - 2);
        OLED_ShowNum(16 * 3 + 5, 16 * 2 + 8 + 8,  range.MinTemp, 2, 16,  setn - 3);
    }
    else if (setn == 4 || setn == 5)
    {
        OLED_ShowNum(16 * 3 + 5, 16 + 8,  range.MaxHumi, 2, 16, setn - 4);
        OLED_ShowNum(16 * 3 + 5, 16 * 2 + 8 + 8,  range.MinHumi, 2, 16, setn - 5);
    }
    else
    {
        char smokeStr[7];
        sprintf(smokeStr, "%dppm", range.MaxSmoke);
        OLED_ShowString(16 * 3 + 5, 16  + 8, (u8 *)smokeStr,  16, 0);
    }
}

// 显示数值
void setSensorPage(SetTypeFontIndex type)
{
    u8 i;
    OLED_Clear();
    for (i = 0; i < 4; i++)OLED_ShowChinese(i * 16 + 32, 0, i + type, 16, 1);
    for (i = 0; i < 2; i++)OLED_ShowChinese(i * 16 + 5, 16 + 8, i + 27, 16, 1);
    OLED_ShowString(5 + 32, 16 + 8, (u8 *)": ", 16, 1);
    if (type != SENSOR_SMOKE)
    {
        for (i = 0; i < 2; i++)OLED_ShowChinese(i * 16 + 5, 16 * 2 + 8 + 8, i + 29, 16, 1);
        OLED_ShowString(5 + 32, 16 * 2 + 8 + 8, (u8 *)": ", 16, 1);
    }
    if (type == SENSOR_TEMP)
    {
        OLED_ShowChinese(5 + 16 * 4, 16 + 8, 10, 16, 1);
        OLED_ShowChinese(5 + 16 * 4,  16 * 2 + 8 + 8, 10, 16, 1);
    }
    if (type == SENSOR_HUMI)
    {
        OLED_ShowString(5 + 16 * 4, 16 + 8, (u8 *)"%", 16, 1); //%
        OLED_ShowString(5 + 16 * 4,  16 * 2 + 8 + 8, (u8 *)"%", 16, 1); //%
    }
}
// 加减设置阈值
uint8_t pageArr[6] = {NULL, SENSOR_TEMP, SENSOR_TEMP, SENSOR_HUMI, SENSOR_HUMI, SENSOR_SMOKE};
void handleKeyClick()
{
    t = KEY_Scan(0);
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

        if (setn == 2 && range.MaxTemp > 0)  t == KEY1_PRES ? range.MaxTemp++ : range.MaxTemp--; //
        if (setn == 3 && range.MinTemp < range.MaxTemp)  t == KEY1_PRES ? range.MinTemp++ : range.MinTemp--; //
        if (setn == 4 && range.MaxHumi)  t == KEY1_PRES ? range.MaxHumi++ : range.MaxHumi--;
        if (setn == 5 && range.MinHumi < range.MaxHumi)  t == KEY1_PRES ? range.MinHumi++ : range.MinHumi--;
        if (setn == 6 && range.MaxSmoke > 0)  t == KEY1_PRES ? range.MaxSmoke++ : range.MaxSmoke--;
		saveToFlash(&range, currentVal.isOpenPerson); // 设置值保存到flash
    }
    if (t == KEY3_PRES)
    {
        currentVal.isOpenPerson = !currentVal.isOpenPerson;
		saveToFlash(&range, currentVal.isOpenPerson);
    }
    setSensorVal();
}
// 超过阈值处理函数
void handleAbnormal()
{
    u8 i;
    static u16 count = 0;
    static u8 isBlink = 1;
    isBlink = !isBlink;
    IsInRange judge = inRange();
    if (count++ < 20) return;
    count = 0;
    // OLED
    (!judge.hasNoPerson || !judge.inMaxHumiRange || !judge.inMaxSmokeRange || !judge.inMaxTempRange || !judge.inMaxTempRange || !judge.inMinHumiRange) ? LED1_ON() : LED1_OFF();
    if (judge.inMaxTempRange)
    {

        Motor_SetSpeed(0);
    }
    else
    {
        Motor_SetSpeed(50);
    }
    OLED_ShowString(5 + 16 + 16, 16, ((!judge.inMaxTempRange || !judge.inMinTempRange) && isBlink) ? (u8 *)"*" : (u8 *)" ", 16, 1); //*
    OLED_ShowString(47 + 32, 16, ((!judge.inMaxHumiRange || !judge.inMinHumiRange) && isBlink) ? (u8 *)"*" : (u8 *)" ", 16, 1); //*
	OLED_ShowString(47 + 64, 16*2, ((!judge.inMaxSmokeRange) && isBlink) ? (u8 *)"*" : (u8 *)" ", 16, 1); //*
    if (currentVal.hasPerson && isBlink)
    {
        for (i = 0; i < 4; i++)OLED_ShowChinese(i * 16 + 5, 3 * 16, i + 11, 16, 1) ;
    }
    else
    {
        OLED_ShowString(5, 3 * 16, (u8 *)"        ", 16, 1);
    }
}

// 是否在阈值范围内
IsInRange inRange(void)
{
    // c99
    IsInRange judge =
    {
        .inMaxTempRange = currentVal.currentTemp <= range.MaxTemp,
        .inMinTempRange = currentVal.currentTemp >= range.MinTemp,
        .inMaxHumiRange = currentVal.currentHumi <= range.MaxHumi,
        .inMinHumiRange = currentVal.currentHumi >= range.MinHumi,
        .inMaxSmokeRange = currentVal.currentSmoke <= range.MaxSmoke,
        .hasNoPerson = !currentVal.hasPerson
    };
    return judge;
}
