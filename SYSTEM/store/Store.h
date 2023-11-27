#ifndef __PARAM_LIST_H
#define __PARAM_LIST_H


typedef struct
{
    uint8_t currentTemp;
    uint8_t currentHumi;
    uint8_t isOpenPerson ;
    uint16_t currentSmoke ;
    uint8_t hasPerson ;
} CurrentVal;

typedef struct
{
    uint8_t MaxTemp ;
    uint8_t MinTemp;
    uint8_t MaxHumi ;
    uint8_t MinHumi ;
    uint16_t MaxSmoke ;
} Range;
extern uint16_t Store_Data[];

uint8_t Store_Init(void);
void Store_Save(void);
void Store_Clear(void);
void getFlashVal(Range* range, uint8_t* isOpenPerson);
void saveToFlash(Range* range, uint8_t isOpenPerson);
#endif
