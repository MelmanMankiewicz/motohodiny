/**
 * @author [Petr Oblouk]
 * @github [https://github.com/peoblouk]
 * @create date 31-05-2022 - 17:05:15
 * @modify date 31-05-2022 - 17:05:15
 * @desc [TM1637 Library]
 */
#ifndef __STM8S_TM1637_H
#define __STM8S_TM1637_H

#include "stm8s.h"
#include "delay.h"

/*
typedef struct
{
    int (*init)(GPIO_TypeDef *, GPIO_Pin_TypeDef, GPIO_TypeDef *, GPIO_Pin_TypeDef);
    void (*brightness)(uint8_t);
    void (*show)(char *);
} TM1637_module;
 */

int tm1637Init(GPIO_TypeDef *bClock, GPIO_Pin_TypeDef bClockP, GPIO_TypeDef *bData, GPIO_Pin_TypeDef bDataP);
void tm1637Start(void);
void tm1637Stop(void);
uint8_t tm1637GetAck(void);
void tm1637WriteByte(uint8_t b);
void tm1637Write(uint8_t *pData, uint8_t bLen);
void tm1637SetBrightness(uint8_t b);
void tm1637ShowDigits(char *pString);

// extern const TM1637_module tm1637;

#endif /* __STM8S_TM1637_H */