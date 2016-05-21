/*
 * TurnSensor.cpp
 *
 *  Created on: May 18, 2016
 *      Author: Amit
 */
#include "TurnSensor.hpp"
#include "file_logger.h"
#include "printf_lib.h"
SemaphoreHandle_t LED_Semaphore;
uint8_t flag = 0;

TurnSensTask::TurnSensTask(uint8_t priority):scheduler_task("TurnSensorTask", 4*512, priority)
/*,left_led(P2_2),right_led(P2_4)/*,headlight(P1_22)*/
{
    //nothing to init
    //headlight_flag=0;
    reading=0;
    LED_Semaphore=NULL;
}



bool TurnSensTask:: init()
{
    adc0_init();
    LPC_PINCON->PINSEL3 |=  (3 << 28); // ADC-4 is on P1.30, select this as ADC0.4

    //    left_led.setAsOutput();
    //    right_led.setAsOutput();
    //headlight.setAsOutput();

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

    reading = adc0_get_reading(4);
    // Read current value of ADC-4
    // printf("\n%d\n", reading);
    LOG_INFO("pot:%d",reading);
    u0_dbg_printf("A:%d\n",reading);
    if(reading>1950)
    {

        flag = 1;
        // vTaskResume(getTaskPtrByName("LED_BlTask"));
        xSemaphoreGive(LED_Semaphore);
        // vTask
        /*//left_led.setLow();
        delay_ms(500);
        //left_led.setHigh();
        delay_ms(500);*/
    }
    else if(reading<1800)
    {
        flag = 2;
        // vTaskResume(getTaskPtrByName("LED_BlTask"));
        xSemaphoreGive(LED_Semaphore);
        /*//right_led.setLow();
        delay_ms(500);
        //right_led.setHigh();
        delay_ms(500);*/
    }
    /*uint16_t light_percent=LS.getPercentValue();
   // printf("Light percent %i",light_percent);
    if(light_percent<15 && headlight_flag==0)
    {
        headlight.setHigh();
        delay_ms(25);
        headlight.setLow();
        //delay_ms(25);
        headlight_flag=1;
    }
    if(light_percent>15 && headlight_flag==1)
        {
            headlight.setHigh();
            delay_ms(25);
            headlight.setLow();
            //delay_ms(10);
            headlight_flag=0;
        }
    LOG_INFO("Light percent value :%d",light_percent);*/
    // light_led(reading);
    //XXX: Log ADC data in file ... Use LOG_INFO... see example in Distance_Sensor.hpp
    //vTaskDelay(500);
    return true;
}

LEDBlink::LEDBlink(uint8_t priority):scheduler_task("LED_BlTask", 4*512, priority),
        left_led(P2_2),right_led(P2_4),center_led(P2_3)/*,headlight(P1_22)*/
{
    //nothing to init
    //headlight_flag=0;
    //reading=0;
}



bool LEDBlink:: init()
{
    //    adc0_init();
    //    LPC_PINCON->PINSEL3 |=  (3 << 28); // ADC-4 is on P1.30, select this as ADC0.4

    vSemaphoreCreateBinary(LED_Semaphore);
    xSemaphoreTake(LED_Semaphore,0);

    left_led.setAsOutput();
    left_led.setHigh();
    right_led.setAsOutput();
    right_led.setHigh();
    center_led.setAsOutput();
    center_led.setHigh();
    //headlight.setAsOutput();

    return true;

}


bool LEDBlink::run(void *p)
{
    if(xSemaphoreTake(LED_Semaphore,portMAX_DELAY))
    {
        int i;

        //reading = adc0_get_reading(4);
        // Read current value of ADC-4
        // printf("\n%d\n", reading);
        //LOG_INFO("pot:%d",reading);
        switch(flag)
        {
            case 1:
                /*   LE.toggle(3);
        delay_ms(500);*/

               // for(i=0;i<5;i++)
                //{
                    LE.toggle(3);
                    left_led.setLow();
                    vTaskDelay(1000);
                    left_led.setHigh();
                    vTaskDelay(1000);

                    //  vTaskDelay(500);
               // }
                flag = 0;

                //vTaskSuspend(0);
                break;
            case 2:

               // for(i=0;i<5;i++)
               // {
                    LE.toggle(4);
                    right_led.setLow();
                    vTaskDelay(1000);
                    right_led.setHigh();
                    //delay_ms(500);
                    vTaskDelay(1000);
               // }
                flag = 0;
                break;
                //vTaskSuspend(0);

        }
    }

    return true;
}

