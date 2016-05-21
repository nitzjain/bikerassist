/*
 * headlight.hpp
 *
 *  Created on: May 18, 2016
 *      Author: Amit
 */

#ifndef L5_APPLICATION_HEADLIGHT_HPP_
#define L5_APPLICATION_HEADLIGHT_HPP_
#include "LPC17xx.h"
#include "utilities.h"
#include <stdio.h>
#include "uart3.hpp"
#include "io.hpp"
#include "tasks.hpp"
#include "scheduler_task.hpp"
#include "gpio.hpp"

class headlight:public scheduler_task
{
    public:
        headlight(uint8_t priority):scheduler_task("Headlight", 512*4, priority)
    {
            //nothing to init
          //  setRunDuration(900000);//every 15 minutes
    }
        bool init(void)
        {


        }
        bool run(void *p)
        {

            return true;
        }


};




#endif /* L5_APPLICATION_HEADLIGHT_HPP_ */
