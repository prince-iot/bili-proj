#include "stm32f10x.h"                  // Device header
#include "MyFLASH.h"
#include "Store.h"
#define STORE_START_ADDRESS     0x0800FC00
#define STORE_COUNT             512

uint16_t Store_Data[STORE_COUNT];

uint8_t Store_Init(void)
{
	u8 flag = 1;
    if (MyFLASH_ReadHalfWord(STORE_START_ADDRESS) != 0xA5A5)
    {
        MyFLASH_ErasePage(STORE_START_ADDRESS);
        MyFLASH_ProgramHalfWord(STORE_START_ADDRESS, 0xA5A5);
        for (uint16_t i = 1; i < STORE_COUNT; i ++)
        {
            MyFLASH_ProgramHalfWord(STORE_START_ADDRESS + i * 2, 0x0000);
        }
		flag = 0;
    }

    for (uint16_t i = 0; i < STORE_COUNT; i ++)
    {
        Store_Data[i] = MyFLASH_ReadHalfWord(STORE_START_ADDRESS + i * 2);
    }
	return flag;
	
}

void Store_Save(void)
{
    MyFLASH_ErasePage(STORE_START_ADDRESS);
    for (uint16_t i = 0; i < STORE_COUNT; i ++)
    {
        MyFLASH_ProgramHalfWord(STORE_START_ADDRESS + i * 2, Store_Data[i]);
    }
}

void Store_Clear(void)
{
    for (uint16_t i = 1; i < STORE_COUNT; i ++)
    {
        Store_Data[i] = 0x0000;
    }
    Store_Save();
}

// ±£´æµ½flash
void saveToFlash(Range* range, uint8_t isOpenPerson)
{
    Store_Data[1] = range->MaxTemp;
    Store_Data[2] = range->MinTemp;
    Store_Data[3] = range->MaxHumi;
    Store_Data[4] = range->MinHumi;
    Store_Data[5] = range->MaxSmoke;
    Store_Data[6] = isOpenPerson;
    Store_Save();
}
void getFlashVal(Range* range, uint8_t* isOpenPerson)
{
	u8 flag = Store_Init();
	if(!flag) return;
    range->MaxTemp = Store_Data[1];
    range->MinTemp = Store_Data[2];
    range->MaxHumi = Store_Data[3] ;
    range->MinHumi = Store_Data[4] ;
    range->MaxSmoke =  Store_Data[5];
    *isOpenPerson =  Store_Data[6];

}




