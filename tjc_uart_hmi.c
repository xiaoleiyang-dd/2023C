/*
 * tjc_uart_hmi.c
 *
 *  Created on: 2023��8��2��
 *      Author: Lenovo
 */


/**
ʹ��ע������:
    1.��tjc_usart_hmi.c��tjc_usart_hmi.h �ֱ��빤��
    2.����Ҫʹ�õĺ������ڵ�ͷ�ļ������ #include "tjc_usart_hmi.h"
    3.ʹ��ǰ�뽫 HAL_UART_Transmit_IT() ���������Ϊ��ĵ�Ƭ���Ĵ��ڷ��͵��ֽں���
    3.TJCPrintf��printf�÷�һ��

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

RingBuff_t ringBuff;    //����һ��ringBuff�Ļ�����
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
��������    TJCPrintf
���ߣ�     wwd
���ڣ�     2022.10.08
���ܣ�     �򴮿ڴ�ӡ����,ʹ��ǰ�뽫USART_SCREEN_write���������Ϊ��ĵ�Ƭ���Ĵ��ڷ��͵��ֽں���
���������       �ο�printf
����ֵ��        ��ӡ�����ڵ�����
�޸ļ�¼��
**********************************************************/

void TJCPrintf(const char *str, ...)
{

    int end = 0xff;
    char buffer[STR_LENGTH+1];  // ���ݳ���
    int in = 0;
    va_list arg_ptr;
    va_start(arg_ptr, str);
    vsnprintf(buffer, STR_LENGTH+1, str, arg_ptr);
    va_end(arg_ptr);

   // while (in < strlen(buffer))
   // {
        //���͵��ֽ�����
    transmitPacketBlocking((uint8_t *)(buffer),strlen(buffer));
        //DL_UART_fillTXFIFO(UART_0_INST,(uint8_t *)(buffer),strlen(buffer));
        //while(!DL_UART_isTXFIFOFull(UART_0_INST));//�ȴ��������
   // }
    DL_UART_transmitDataBlocking(UART_0_INST, end);
    //while(!DL_UART_isTXFIFOFull(UART_0_INST));
    DL_UART_transmitDataBlocking(UART_0_INST, end);
    //while(!DL_UART_isTXFIFOFull(UART_0_INST));
    DL_UART_transmitDataBlocking(UART_0_INST, end);
    //while(!DL_UART_isTXFIFOFull(UART_0_INST));
}

/********************************************************
��������    initRingBuff
���ߣ�
���ڣ�     2022.10.08
���ܣ�     ��ʼ�����λ�����
���������
����ֵ��        void
�޸ļ�¼��
**********************************************************/
void initRingBuff(void)
{
  //��ʼ�������Ϣ
  ringBuff.Head = 0;
  ringBuff.Tail = 0;
  ringBuff.Lenght = 0;
}


/********************************************************
��������    writeRingBuff
���ߣ�
���ڣ�     2022.10.08
���ܣ�     �����λ�����д������
���������
����ֵ��        void
�޸ļ�¼��
**********************************************************/
void writeRingBuff(uint8_t data)
{
  if(ringBuff.Lenght >= RINGBUFF_LEN) //�жϻ������Ƿ�����
  {
    return ;
  }
  ringBuff.Ring_data[ringBuff.Tail]=data;
  ringBuff.Tail = (ringBuff.Tail+1)%RINGBUFF_LEN;//��ֹԽ��Ƿ�����
  ringBuff.Lenght++;
}




/********************************************************
��������    deleteRingBuff
���ߣ�
���ڣ�     2022.10.08
���ܣ�     ɾ�����ڻ���������Ӧ���ȵ�����
���������       Ҫɾ���ĳ���
����ֵ��        void
�޸ļ�¼��
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

        if(ringBuff.Lenght == 0)//�жϷǿ�
        {
        initRingBuff();
        return;
        }
        ringBuff.Head = (ringBuff.Head+1)%RINGBUFF_LEN;//��ֹԽ��Ƿ�����
        ringBuff.Lenght--;

    }

}


/********************************************************
��������    read1BFromRingBuff
���ߣ�
���ڣ�     2022.10.08
���ܣ�     �Ӵ��ڻ�������ȡ1�ֽ�����
���������       position:��ȡ��λ��
����ֵ��        ����λ�õ�����(1�ֽ�)
�޸ļ�¼��
**********************************************************/
uint8_t read1BFromRingBuff(uint16_t position)
{
    uint16_t realPosition = (ringBuff.Head + position) % RINGBUFF_LEN;

    return ringBuff.Ring_data[realPosition];
}




/********************************************************
��������    getRingBuffLenght
���ߣ�
���ڣ�     2022.10.08
���ܣ�     ��ȡ���ڻ���������������
���������
����ֵ��        ���ڻ���������������
�޸ļ�¼��
**********************************************************/
uint16_t getRingBuffLenght()
{
    return ringBuff.Lenght;
}


/********************************************************
��������    isRingBuffOverflow
���ߣ�
���ڣ�     2022.10.08
���ܣ�     �жϻ��λ������Ƿ�����
���������
����ֵ��        1:���λ��������� , 2:���λ�����δ��
�޸ļ�¼��
**********************************************************/
uint8_t isRingBuffOverflow()
{
    return ringBuff.Lenght == RINGBUFF_LEN;
}




