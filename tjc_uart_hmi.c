/*
 * tjc_uart_hmi.c
 *
 *  Created on: 2023年8月2日
 *      Author: Lenovo
 */


/**
使用注意事项:
    1.将tjc_usart_hmi.c和tjc_usart_hmi.h 分别导入工程
    2.在需要使用的函数所在的头文件中添加 #include "tjc_usart_hmi.h"
    3.使用前请将 HAL_UART_Transmit_IT() 这个函数改为你的单片机的串口发送单字节函数
    3.TJCPrintf和printf用法一样

*/

#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include "tjc_uart_hmi.h"
#include <stddef.h>
#include "ti_msp_dl_config.h"

#define STR_LENGTH 100

typedef struct
{
    uint16_t Head;
    uint16_t Tail;
    uint16_t Lenght;
    uint8_t  Ring_data[RINGBUFF_LEN];
}RingBuff_t;

RingBuff_t ringBuff;    //创建一个ringBuff的缓冲区
uint8_t RxBuff[1];


void transmitPacketBlocking(uint8_t *buffer, uint32_t size)
{
    uint32_t i;
    for (i = 0; i < size; i++) {
        /*
         * The transmitter will automatically check that the CTS signal is
         * asserted (low) before transmitting the next byte. If CTS is
         * de-asserted (high), the byte will not be transmitted out of the TX
         * FIFO. Therefore block if the TX FIFO is full before attempting
         * to fill it.
         */
        DL_UART_Main_transmitDataBlocking(UART_0_INST, buffer[i]);
    }
}

/********************************************************
函数名：    TJCPrintf
作者：     wwd
日期：     2022.10.08
功能：     向串口打印数据,使用前请将USART_SCREEN_write这个函数改为你的单片机的串口发送单字节函数
输入参数：       参考printf
返回值：        打印到串口的数量
修改记录：
**********************************************************/

void TJCPrintf(const char *str, ...)
{

    int end = 0xff;
    char buffer[STR_LENGTH+1];  // 数据长度
    int in = 0;
    va_list arg_ptr;
    va_start(arg_ptr, str);
    vsnprintf(buffer, STR_LENGTH+1, str, arg_ptr);
    va_end(arg_ptr);

   // while (in < strlen(buffer))
   // {
        //发送单字节数据
    transmitPacketBlocking((uint8_t *)(buffer),strlen(buffer));
        //DL_UART_fillTXFIFO(UART_0_INST,(uint8_t *)(buffer),strlen(buffer));
        //while(!DL_UART_isTXFIFOFull(UART_0_INST));//等待发送完毕
   // }
    DL_UART_transmitDataBlocking(UART_0_INST, end);
    //while(!DL_UART_isTXFIFOFull(UART_0_INST));
    DL_UART_transmitDataBlocking(UART_0_INST, end);
    //while(!DL_UART_isTXFIFOFull(UART_0_INST));
    DL_UART_transmitDataBlocking(UART_0_INST, end);
    //while(!DL_UART_isTXFIFOFull(UART_0_INST));
}

/********************************************************
函数名：    initRingBuff
作者：
日期：     2022.10.08
功能：     初始化环形缓冲区
输入参数：
返回值：        void
修改记录：
**********************************************************/
void initRingBuff(void)
{
  //初始化相关信息
  ringBuff.Head = 0;
  ringBuff.Tail = 0;
  ringBuff.Lenght = 0;
}


/********************************************************
函数名：    writeRingBuff
作者：
日期：     2022.10.08
功能：     往环形缓冲区写入数据
输入参数：
返回值：        void
修改记录：
**********************************************************/
void writeRingBuff(uint8_t data)
{
  if(ringBuff.Lenght >= RINGBUFF_LEN) //判断缓冲区是否已满
  {
    return ;
  }
  ringBuff.Ring_data[ringBuff.Tail]=data;
  ringBuff.Tail = (ringBuff.Tail+1)%RINGBUFF_LEN;//防止越界非法访问
  ringBuff.Lenght++;
}




/********************************************************
函数名：    deleteRingBuff
作者：
日期：     2022.10.08
功能：     删除串口缓冲区中相应长度的数据
输入参数：       要删除的长度
返回值：        void
修改记录：
**********************************************************/
void deleteRingBuff(uint16_t size)
{
    if(size >= ringBuff.Lenght)
    {
        initRingBuff();
        return;
    }
    for(int i = 0; i < size; i++)
    {

        if(ringBuff.Lenght == 0)//判断非空
        {
        initRingBuff();
        return;
        }
        ringBuff.Head = (ringBuff.Head+1)%RINGBUFF_LEN;//防止越界非法访问
        ringBuff.Lenght--;

    }

}


/********************************************************
函数名：    read1BFromRingBuff
作者：
日期：     2022.10.08
功能：     从串口缓冲区读取1字节数据
输入参数：       position:读取的位置
返回值：        所在位置的数据(1字节)
修改记录：
**********************************************************/
uint8_t read1BFromRingBuff(uint16_t position)
{
    uint16_t realPosition = (ringBuff.Head + position) % RINGBUFF_LEN;

    return ringBuff.Ring_data[realPosition];
}




/********************************************************
函数名：    getRingBuffLenght
作者：
日期：     2022.10.08
功能：     获取串口缓冲区的数据数量
输入参数：
返回值：        串口缓冲区的数据数量
修改记录：
**********************************************************/
uint16_t getRingBuffLenght()
{
    return ringBuff.Lenght;
}


/********************************************************
函数名：    isRingBuffOverflow
作者：
日期：     2022.10.08
功能：     判断环形缓冲区是否已满
输入参数：
返回值：        1:环形缓冲区已满 , 2:环形缓冲区未满
修改记录：
**********************************************************/
uint8_t isRingBuffOverflow()
{
    return ringBuff.Lenght == RINGBUFF_LEN;
}




