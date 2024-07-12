/*
 * tjc_uart_hmi.h
 *
 *  Created on: 2023年8月2日
 *      Author: Lenovo
 */

#ifndef TJC_UART_HMI_H_
#define TJC_UART_HMI_H_


#include "ti_msp_dl_config.h"

/**
    打印到屏幕串口
*/
void TJCPrintf(const char *cmd, ...);
void initRingBuff(void);
void writeRingBuff(uint8_t data);
void deleteRingBuff(uint16_t size);
uint16_t getRingBuffLenght(void);
uint8_t read1BFromRingBuff(uint16_t position);



#define RINGBUFF_LEN    (500)     //定义最大接收字节数 500

#define usize getRingBuffLenght()
#define code_c() initRingBuff()
#define udelete(x) deleteRingBuff(x)
#define u(x) read1BFromRingBuff(x)


extern uint8_t RxBuff[1];

#endif /* TJC_UART_HMI_H_ */
