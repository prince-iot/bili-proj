#include "delay.h"
#include "sys.h"
#include "oled.h"
#include "bmp.h"

// mock
uint8_t currentTemp = 20;
uint8_t currentHumi = 50;
uint8_t isOpenPerson = 1;  // 1开启 0不开启
uint16_t currentSmoke = 500;
uint8_t hasPerson = 1;
void showMainPage(void);
void setSensorVal(void);

int main(void)
{

	delay_init();
	OLED_Init();
	OLED_ColorTurn(0);//0正常显示，1 反色显示
  OLED_DisplayTurn(0);//0正常显示 1 屏幕翻转显示
	while(1)
	{
	
		OLED_Refresh();
		showMainPage();
		setSensorVal();
	}
}

void showMainPage(void)
{
    uint8_t i;
    for (i = 0; i < 2; i++)  OLED_ShowChinese(i * 16 + 5, 0, i, 16, 1); // 温度
    for (i = 0; i < 2; i++)  OLED_ShowChinese((i + 2) * 16 + 5 + 10, 0, i + 2, 16, 1); // 湿度
    for (i = 0; i < 2; i++)  OLED_ShowChinese((i + 4) * 16 + 5 + 20, 0, i + 4, 16, 1); //人体
    OLED_ShowChinese(5 + 16, 16, 10, 16, 1); // 摄氏度
    for (i = 0; i < 2; i++)  OLED_ShowChinese(i * 16 + 5, 16 * 2, i + 8, 16, 1); // 烟雾
    OLED_ShowString(5 + 16 * 2, 16 * 2, ": ", 16, 1);
}
void setSensorVal(void)
{
    uint8_t i;
    char humiArr[4];
    char smokeArr[7];
    OLED_ShowNum(5, 16, currentTemp, 2, 16, 1); // 温度值
    sprintf(humiArr, "%d%%", currentHumi);
    sprintf(smokeArr, "%dppm", currentSmoke);
		//OLED_ShowNum(2*16+5+10,16,currentHumi,2,16,1);  // 湿度值
		OLED_ShowString(2*16+5+10,16,(u8 *)humiArr,16,1);
    OLED_ShowChinese(4 * 16 + 5 + 20 + 8, 16, isOpenPerson == 1 ? 6 : 7, 16, 1); //  人体检测开关
    //OLED_ShowNum(5+16*3,16*2,currentSmoke,3,16,1); // 烟雾值
		OLED_ShowString(5+16*3,16*2,(u8 *)smokeArr,16,1);
    if (hasPerson)
    {
        for (i = 0; i < 4; i++)  OLED_ShowChinese(i * 16 + 5, 16 * 3, i + 11, 16, 1); // 检测到人
    }
    else
    {
        OLED_ShowString(5, 16 * 3, "        ", 16, 1);
    }
}

