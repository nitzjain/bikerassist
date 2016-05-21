/*
 * tts.h
 *
 *  Created on: Apr 4, 2016
 *      Author: Amit
 */

#ifndef TTS_HPP_
#define TTS_HPP_

#include<stdio.h>
#include "uart3.hpp"
#include "io.hpp"
#include "tasks.hpp"
#include "scheduler_task.hpp"
#include "command_handler.hpp"

#define TTSBaudRate 9600


enum TextSelection_t
{
    LeftDirection  = 1,
    RightDirection = 2,
    NoDirection    = 0

};


class TTSTask:public scheduler_task
{
    public:
        TTSTask(uint8_t priority);
        bool init(void);
        bool run(void *p);

        //QueueHandle_t TextSelectQueue;


    private:
        Uart3 &tts_uart;

        bool startup = 1;
};



#endif /* TTS_HPP_ */
