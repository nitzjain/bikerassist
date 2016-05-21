/*
 * tts.cpp
 *
 *  Created on: Apr 4, 2016
 *      Author: Amit
 */

#include "tts.hpp"

SemaphoreHandle_t TTS_Semaphore;

//XXX:Semaphore can be used to trigger Uart send

#define buttonTesting 0

TextSelection_t SelectText;


TTSTask::TTSTask(uint8_t priority):scheduler_task("TTS", 4*512, priority),
tts_uart(Uart3::getInstance())
{
    TTS_Semaphore = NULL;
}

bool TTSTask::init()
{
    vSemaphoreCreateBinary(TTS_Semaphore);
    xSemaphoreTake(TTS_Semaphore,0);
    tts_uart.init(TTSBaudRate,1,100);

    SelectText    = NoDirection;

    return true;
}

bool TTSTask::run(void *p)
{

    if(xSemaphoreTake(TTS_Semaphore,portMAX_DELAY))
    {
        char rec;
        {
#if buttonTesting
            if(SW.getSwitch(1))
                SelectText = RightDirection;
            if(SW.getSwitch(2))
                SelectText = LeftDirection;
#endif
            {
                switch(SelectText)
                {
                    case RightDirection:
                        //LE.on(1);
                        tts_uart.putline("SCaution!Object on your Left! \n",portMAX_DELAY);//Please be careful before taking turn.
                        //LE.off(1);
                        tts_uart.getChar(&rec,0);
                        break;
                    case LeftDirection:
                        //LE.on(2);
                        tts_uart.putline("SCaution!Object on your Right! \n",portMAX_DELAY);
                        //LE.off(2);
                        tts_uart.getChar(&rec,0);
                        break;
                    case NoDirection:
                        break;
                }
                SelectText = NoDirection;
            }
        }
    }
    return true;
}

//Code written for Testing using terminal
CMD_HANDLER_FUNC(TTSDirHandler)
{

    if(cmdParams.containsIgnoreCase("right"))
    {
        LE.toggle(1);
        SelectText = RightDirection;
    }
    else if(cmdParams.containsIgnoreCase("left"))
    {
        LE.toggle(2);
        SelectText = LeftDirection;

    }
    else
    {
        output.putline("Error in command");
    }
       return true;
}
