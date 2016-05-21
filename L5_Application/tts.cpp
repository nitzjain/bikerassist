/*
 * tts.cpp
 *
 *  Created on: Apr 4, 2016
 *      Author: Amit
 */

#include "tts.hpp"

SemaphoreHandle_t TTS_Semaphore;

//244: Semaphore can be used to trigger Uart send

#define buttonTesting 0

TextSelection_t SelectText;


TTSTask::TTSTask(uint8_t priority):scheduler_task("TTS", 4*512, priority),
tts_uart(Uart3::getInstance())
{
    //TextSelectQueue = NULL;
    TTS_Semaphore = NULL;
}

bool TTSTask::init()
{
    vSemaphoreCreateBinary(TTS_Semaphore);
    xSemaphoreTake(TTS_Semaphore,0);
    //addSharedObject("TTS_Sem",&TTS_Semaphore);
    //XXX: Add semaphore to shared object
    //XXX: Create Queue for deciding which text to send to TTS module and add it to shared object
    //TextSelectQueue = xQueueCreate(10,sizeof(TextSelection_t));
    //addSharedObject("TTS_Q",&TextSelectQueue);
    tts_uart.init(TTSBaudRate,1,100);

    SelectText    = NoDirection;

    //addSharedObject("Text_Sel",&SelectText);
    return true;
}

bool TTSTask::run(void *p)
{

    if(xSemaphoreTake(TTS_Semaphore,portMAX_DELAY))
    {
        //LE.toggle(3);
        char rec;
        //XXX: Check if ':' received.... ':' received means device is ready
        //XXX: If ':' received then only send text
        //XXX: Receive queue to select output text to speech
        /*if(startup)
        {
            while(!tts_uart.getChar(&rec,portMAX_DELAY));
            startup=0;
        }*/
        //scheduler_task::getSharedObject("TTS_Q");
        //printf("%c ",rec);
        //if(rec==':')
        {
#if buttonTesting
            if(SW.getSwitch(1))
                SelectText = RightDirection;
            if(SW.getSwitch(2))
                SelectText = LeftDirection;
#endif
            //if(xQueueReceive(TextSelectQueue,&SelectText,portMAX_DELAY))
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
    //TextSelection_t direction = NoDirection;
    //QueueHandle_t que = scheduler_task::getSharedObject("TTS_Q");
    //SemaphoreHandle_t TTSDir_sem = scheduler_task::getSharedObject("TTS_Sem");
    /*if(que == NULL)
        LE.toggle(4);*/
    if(cmdParams.containsIgnoreCase("right"))
    {
        LE.toggle(1);
        SelectText = RightDirection;
        //xSemaphoreGive(TTSDir_sem);
        /*direction = RightDirection;
        if(xQueueSend(que, &direction,0))
            output.putline("right sent");*/
    }
    else if(cmdParams.containsIgnoreCase("left"))
    {
        LE.toggle(2);
        SelectText = LeftDirection;
        //xSemaphoreGive(TTSDir_sem);
        /*direction = LeftDirection;
        if(xQueueSend(que, &direction, 0))
            output.putline("left sent");*/
    }
    else
    {
        //LE.toggle(3);
        /*xQueueSend(scheduler_task::getSharedObject("TTS_Q"), &direction, 0);*/
        output.putline("Error in command");
    }
       return true;
}
