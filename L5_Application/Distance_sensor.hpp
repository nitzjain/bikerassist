/*
 * Distance_sensor.hpp
 *
 *  Created on: May 7, 2016
 *      Author: Amit
 */

#ifndef L5_APPLICATION_DISTANCE_SENSOR_HPP_
#define L5_APPLICATION_DISTANCE_SENSOR_HPP_

#include <stdio.h>
#include "gpio.hpp"
#include "printf_lib.h"
#include "lpc_timers.h"
#include "eint.h"
#include "io.hpp"
#include "utilities.h"
#include "tts.hpp"
#include "file_logger.h"

#define DISTANCE_THRESHOLD      100

extern TextSelection_t SelectText;
extern SemaphoreHandle_t TTS_Semaphore;

int left_start_time = 0, left_end_time=0,right_start_time = 0, end_time_2=0;
bool echo_received = 0;
uint8_t next_trigger = 1;
int count;

struct distance_reading
{
        int left;
        int right;
}sensor_value;

void Left_Sens_Rising_Int()
{
    left_start_time = lpc_timer_get_value(lpc_timer0);
    //next_trigger = 0;
    LE.on(1);
}
void Left_Sens_Falling_Int()
{
    sensor_value.left = (lpc_timer_get_value(lpc_timer0) - left_start_time)/58;
    next_trigger = 2;
    echo_received = 1;
    LE.off(1);
}
void Right_Sens_Rising_Int()
{
    right_start_time = lpc_timer_get_value(lpc_timer0);

    LE.on(2);
}
void Right_Sens_Falling_Int()
{
    //end_time = lpc_timer_get_value(lpc_timer0);
    sensor_value.right = (lpc_timer_get_value(lpc_timer0) - right_start_time)/58;
    next_trigger = 1;
    echo_received = 1;
    LE.off(2);
}

class DistSensTask : public scheduler_task
{
        bool headlight_flag;
    public:
        GPIO headlight;
        GPIO Left_Tigger, Right_Trigger;
        GPIO Left_PWM, Right_PWM;
        DistSensTask(uint8_t priority) :
            scheduler_task("Maxbotix", 512*4, priority),
            Left_Tigger(P1_29),Left_PWM(P2_0),Right_PWM(P2_1),Right_Trigger(P1_28),headlight(P1_22)
        {
            headlight_flag = 0;
            setRunDuration(50);
            /* Nothing to init */
        }

        bool init()
        {

            logger_init(PRIORITY_MEDIUM);

            Left_Tigger.setAsOutput();
            Left_PWM.setAsInput();
            Right_Trigger.setAsOutput();
            Right_PWM.setAsInput();

            headlight.setAsOutput();

            const uint8_t port2_0 = 0,port2_1=1;
            eint3_enable_port2(port2_0,eint_rising_edge,Left_Sens_Rising_Int);
            eint3_enable_port2(port2_0,eint_falling_edge,Left_Sens_Falling_Int);
            eint3_enable_port2(port2_1,eint_rising_edge,Right_Sens_Rising_Int);
            eint3_enable_port2(port2_1,eint_falling_edge,Right_Sens_Falling_Int);
            lpc_timer_enable(lpc_timer0, 1);

            Left_Tigger.setLow();
            Right_Trigger.setLow();

            return true;
        }

        bool run(void *p)
        {

            switch(next_trigger)
            {
                case 1:
                    Left_Tigger.setHigh();
                    delay_us(25);
                    Left_Tigger.setLow();
                    break;
                case 2:
                    Right_Trigger.setHigh();
                    delay_us(25);
                    Right_Trigger.setLow();
                    break;
            }

            if(echo_received)
            {
                LOG_INFO("L:%d,R:%d",sensor_value.left,sensor_value.right);

                u0_dbg_printf("L:%d,R:%d",sensor_value.left,sensor_value.right);

                count=0;
                echo_received = 0;
                if(sensor_value.right<DISTANCE_THRESHOLD)
                {
                    SelectText = RightDirection;
                    //LE.toggle(1);
                    xSemaphoreGive(TTS_Semaphore);
                }
                else if(sensor_value.left<DISTANCE_THRESHOLD)
                {
                    SelectText = LeftDirection;
                    //LE.toggle(1);
                    xSemaphoreGive(TTS_Semaphore);
                }

            }
            uint16_t light_percent=LS.getPercentValue();
            // printf("Light percent %i",light_percent);
            if(light_percent<15 && headlight_flag==0)
            {
                headlight.setHigh();
                delay_ms(25);
                headlight.setLow();
                delay_ms(25);
                headlight_flag=1;
            }
            if(light_percent>15 && headlight_flag==1)
            {
                headlight.setHigh();
                delay_ms(25);
                headlight.setLow();
                delay_ms(25);
                headlight_flag=0;
            }
            LOG_INFO("Light:%d",light_percent);
            return true;
        }
};

#endif /* L5_APPLICATION_DISTANCE_SENSOR_HPP_ */
