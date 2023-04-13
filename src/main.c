#include "stm8s.h"
#include "main.h"
#include "milis.h"
#include "tm1637.h"
#include <stdio.h>

// #include "uart1.h"


/* tlačítka */
#define BTN_RESET_PORT GPIOD
#define BTN_RESET_PIN GPIO_PIN_5
#define BTN_SET_PORT GPIOC
#define BTN_SET_PIN GPIO_PIN_6

/* displej */
#define DIO_PORT GPIOA
#define DIO_PIN GPIO_PIN_2
#define CLK_PORT GPIOA
#define CLK_PIN GPIO_PIN_3
#define EEPROM_START FLASH_DATA_START_PHYSICAL_ADDRESS


// 000 000 0100 0011
/* proměnné */
int8_t hours = 0, min = 0, secs = 0;
int32_t readTime = 0;
char szTemp[8];             // nastavení 

void eeprom_write(uint32_t address, int32_t number);
int32_t eeprom_read(uint32_t address);


void setup(void)
{
    CLK_HSIPrescalerConfig(CLK_PRESCALER_HSIDIV1);      // taktovani MCU na 16MHz
    GPIO_Init(GPIOD, GPIO_PIN_4, GPIO_MODE_OUT_PP_LOW_SLOW); // LED port PD4
    tm1637Init(CLK_PORT, CLK_PIN, DIO_PORT, DIO_PIN);                       // Inicializace TM1637
    tm1637SetBrightness(7);                                                 // Nastavení jasu displeje
    GPIO_Init(BTN_RESET_PORT, BTN_RESET_PIN, GPIO_MODE_IN_PU_IT);           // Incializace tlačítka RESET

    // odemčeme přístup k paměti dat ("EEPROM")
    FLASH_Unlock(FLASH_MEMTYPE_DATA);
    // nastavíme programovací čas
    FLASH_SetProgrammingTime(FLASH_PROGRAMTIME_STANDARD);

    // nastavení citlivosti externího přerušení přerušení
    EXTI_SetExtIntSensitivity(EXTI_PORT_GPIOD, EXTI_SENSITIVITY_RISE_FALL);
    // nastavení priority přerušení
    ITC_SetSoftwarePriority(ITC_IRQ_PORTD, ITC_PRIORITYLEVEL_1);
    // povolení přeruření
    enableInterrupts();

    // init_uart1();
    init_milis();
}


int main(void)
{
  
    uint32_t time = 0;

    readTime = eeprom_read(EEPROM_START);

    min = readTime;
    hours = (readTime & 0xf00) >> 8;

    setup();

    while (1) 
    {
        if(milis() - time > 1000) {
            time = milis();
            if (secs == 1)
            {
                secs = 0;
                min++;
                eeprom_write(EEPROM_START, (hours << 8) | min);
            }
            if (min == 60)
            {
                min = 0;
                eeprom_write(EEPROM_START, hours << 8);
                hours++;
            }
            if (hours == 99)
            {
                hours = 0;
            }
            if (secs & 1)
                sprintf(szTemp, "%02d:%02d", hours, min); // formátování čísel aby šlo vepsat na displej
            else
                sprintf(szTemp, "%02d %02d", hours, min); // formátování čísel aby šlo vepsat na displej
            tm1637ShowDigits(szTemp);                     // print na displej
            secs++;
        }

        if (GPIO_ReadInputPin(GPIOD, GPIO_PIN_4) != RESET)
        {
            hours = 0;
            min = 0;
            secs = 0;
        }
    }
    
}

/**
  * @brief  Write 32-b number to EEPROM 
  * @note   standard  way
  * @param  address: address to start write
  * @param  number: number for write
  * @retval none
  */
void eeprom_write(uint32_t address, int32_t number)
{
    uint8_t byte[4];
    
    byte[3] = number & 0x000000FFL;
    byte[2] = (number & 0x0000FF00L) >> 8;
    byte[1] = (number & 0x00FF0000L) >> 16;
    byte[0] = (number & 0xFF000000L) >> 24;
    
    for (short i = 0; i < 4; ++i) {
        FLASH_ProgramByte(address + i, byte[i]);
    }
}


/**
  * @brief  Read 32-b number from EEPROM 
  * @note   standard  way
  * @param  address: address to start write
  * @retval readed number
  */
int32_t eeprom_read(uint32_t address)
{
    uint8_t byte[4];
    
    for (short i = 0; i < 4; ++i) {
        byte[i] = FLASH_ReadByte(address + i);
    }

    return ((int32_t)byte[0] << 24) |
           ((int32_t)byte[1] << 16) |
           ((int32_t)byte[2] << 8 ) |
           ((int32_t)byte[3]) ;
}

//*-------------------------------  Assert -----------------------------------*/
#include "__assert__.h"
