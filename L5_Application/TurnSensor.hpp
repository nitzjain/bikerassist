/*
 * TurnSensor.hpp
 *
 *  Created on: May 18, 2016
 *      Author: Amit
 */

#ifndef L5_APPLICATION_TURNSENSOR_HPP_
#define L5_APPLICATION_TURNSENSOR_HPP_


#include "adc0.h"
#include "LPC17xx.h"
#include "utilities.h"
#include <stdio.h>
#include "uart3.hpp"
#include "io.hpp"
#include "tasks.hpp"
#include "scheduler_task.hpp"
#include "gpio.hpp"


class TurnSensTask:public scheduler_task
{
    public:
        TurnSensTask(uint8_t priority);
        bool init(void);
        bool run(void *p);
        void light_led(uint16_t reading);
    private:
        uint16_t ADC_reading;
        GPIO headlight;
        bool headlight_flag;
};

class LEDBlink:public scheduler_task
{
    public:
        LEDBlink(uint8_t priority);
        bool init(void);
        bool run(void *p);
    private:
        GPIO left_led;
        GPIO right_led;
        GPIO center_led;
};


#endif /* L5_APPLICATION_TURNSENSOR_HPP_ */
