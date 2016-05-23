/*
 * TurnSensor.cpp
 *
 *  Created on: May 18, 2016
 *      Author: Amit
 */
#include "TurnSensor.hpp"
#include "file_logger.h"
#include "printf_lib.h"

#define LEFT_DIR_THRESH     1950
#define RIGHT_DIR_THRESH    1800
#define LIGHT_THRESHOLD     12

SemaphoreHandle_t LED_Semaphore;
uint8_t Direction_flag = 0;

TurnSensTask::TurnSensTask(uint8_t priority):scheduler_task("TurnSensorTask", 4*512, priority),
        headlight(P1_22)
{
    //nothing to init
    /*headlight_flag = 0;*/
    ADC_reading=0;
    LED_Semaphore=NULL;
}



bool TurnSensTask:: init()
{
    adc0_init();
    LPC_PINCON->PINSEL3 |=  (3 << 28); // ADC-4 is on P1.30, select this as ADC0.4

    headlight.setAsOutput();
    headlight.setLow();
    return true;
}


/*void TurnSensTask::light_led(uint16_t reading)
{
    if(reading>2048)
    {
        left_led.setLow();
        delay_ms(500);
        left_led.setHigh();
        delay_ms(500);
    }
    else if(reading<2048)
    {
        right_led.setLow();
        delay_ms(500);
        right_led.setHigh();
        delay_ms(500);
    }

}*/
bool TurnSensTask::run(void *p)
{

    ADC_reading = adc0_get_reading(4);
    LOG_INFO("pot:%d",ADC_reading);
    u0_dbg_printf("A:%d\n",ADC_reading);
    if(ADC_reading>LEFT_DIR_THRESH)
    {
        Direction_flag = 1;
        xSemaphoreGive(LED_Semaphore);
    }
    else if(ADC_reading<RIGHT_DIR_THRESH)
    {
        Direction_flag = 2;
        xSemaphoreGive(LED_Semaphore);//Code added for semaphore use
    }

    uint8_t light_percent=LS.getPercentValue();
    // printf("Light percent %i",light_percent);
    //If headlight OFF and value less than threshold...turn off trigger
    if(light_percent<LIGHT_THRESHOLD /*&& headlight_flag==0*/)
    {
        headlight.setHigh();
        /*vTaskDelay(25);
        headlight.setLow();
        vTaskDelay(25);
        headlight_flag=1;*/
    }
    //If headlight ON and value greater than threshold...turn off trigger
    else if(light_percent>LIGHT_THRESHOLD /*&& headlight_flag==1*/)
    {
        headlight.setLow();
        /*headlight.setHigh();
        vTaskDelay(25);
        vTaskDelay(25);
        headlight_flag=0;*/
    }
    LOG_INFO("Light:%d",light_percent);

        return true;
}

LEDBlink::LEDBlink(uint8_t priority):scheduler_task("LED_BlTask", 4*512, priority),
        left_led(P2_2),right_led(P2_4),center_led(P2_3)
{
    //nothing to init
}

bool LEDBlink:: init()
{
    vSemaphoreCreateBinary(LED_Semaphore);
    xSemaphoreTake(LED_Semaphore,0);

    left_led.setAsOutput();
    left_led.setHigh();
    right_led.setAsOutput();
    right_led.setHigh();
    center_led.setAsOutput();
    center_led.setHigh();

    return true;
}

bool LEDBlink::run(void *p)
{
    if(xSemaphoreTake(LED_Semaphore,portMAX_DELAY))
    {
        switch(Direction_flag)
        {
            case 1:
                LE.toggle(3);
                left_led.setLow();
                vTaskDelay(1000);
                left_led.setHigh();
                vTaskDelay(1000);
                Direction_flag = 0;

                break;
            case 2:
                LE.toggle(4);
                right_led.setLow();
                vTaskDelay(1000);
                right_led.setHigh();
                vTaskDelay(1000);
                Direction_flag = 0;
                break;
        }
    }

    return true;
}
