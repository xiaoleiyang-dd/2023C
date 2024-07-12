/*
 * Copyright (c) 2021, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "ti_msp_dl_config.h"
#include "tjc_uart_hmi.h"

#define cs_9833_0() DL_GPIO_clearPins(GPIO_GRP_0_PORT, GPIO_GRP_0_LED_1_PIN);
#define cs_9833_1() DL_GPIO_setPins(GPIO_GRP_0_PORT, GPIO_GRP_0_LED_1_PIN);

/*
 * Variaries Definitionn
 */volatile bool gCheckADC;
volatile uint16_t gAdcResult0[100];
volatile uint16_t gAdcResult1[100];
volatile uint16_t gAdcResult2[100];
int flag;
/*
 * Function Declaration
 *
 */
void AD_Init(void);
void AD_Write(unsigned short TxData);
void AD_CtrlSet(unsigned char Reset,unsigned char SleepMode,unsigned char optionbit,unsigned char modebit);
void AD_FreqSet(double Freq);
void SPI_Controller_transmitData(uint8_t *data, uint8_t dataLength);
void get_adc_val(int num);

int main(void)
{
    /*
     * Init
     */
    SYSCFG_DL_init();
    NVIC_EnableIRQ(ADC12_0_INST_INT_IRQN);
    while(1)
    {
        while(flag==0)
        {
        //Wait for sw trigger
        }
        if(flag==1)
        {
            detect_C();
        }
        else
        {
            //Null
        }
        if(flag==2)
        {
            detect_L();
        }
        else
        {
            //Null
        }
        flag=0;
    }
   //DL_GPIO_clearPins(GPIO_GRP_0_PORT, GPIO_GRP_0_P0_PIN);
   // DL_GPIO_setPins(GPIO_GRP_0_PORT, GPIO_GRP_0_LED_1_PIN);

    //int cnt=15;
    //DL_GPIO_clearPins(GPIO_LEDS_PORT, GPIO_LEDS_USER_LED_2_PIN);
   // DL_GPIO_setPins(GPIO_LEDS_PORT, GPIO_LEDS_USER_LED_1_PIN | GPIO_LEDS_USER_LED_3_PIN);
    //while (cnt) {

        // GPIO Example
     //DL_GPIO_writePinsVal(GPIO_GRP_0_PORT, GPIO_GRP_0_P0_PIN,1);
       // while(1){}
    //DL_UART_Main_transmitData(UART_0_INST, "H");
    //TJCPrintf("t0.txt=\"111\"");




}
/*
 * ADC-Inturaption
 */
void ADC12_0_INST_IRQHandler(void)
{
    switch (DL_ADC12_getPendingInterrupt(ADC12_0_INST)) {
        case DL_ADC12_IIDX_MEM2_RESULT_LOADED:
            gCheckADC = true;
            break;
        default:
            break;
    }
}
void get_adc_val(int num)
{
    int i=0;
    gCheckADC  = false;
    while (num) {
           DL_ADC12_startConversion(ADC12_0_INST);
           /* Wait until all data channels have been loaded. */
           while (gCheckADC == false) {}
           /* Store ADC Results into their respective buffer */
           gAdcResult0[i]=DL_ADC12_getMemResult(ADC12_0_INST, DL_ADC12_MEM_IDX_0);
           gAdcResult1[i]=DL_ADC12_getMemResult(ADC12_0_INST, DL_ADC12_MEM_IDX_1);
           gAdcResult2[i]=DL_ADC12_getMemResult(ADC12_0_INST,DL_ADC12_MEM_IDX_2);
           gCheckADC = false;
           DL_ADC12_enableConversions(ADC12_0_INST);
           num--;
           i++;
   }
}
/*
 * AD9833 Definition
 */
void SPI_Controller_transmitData(uint8_t *data, uint8_t dataLength)
{
    int i = 0;
    for (i = 0; i < dataLength; i++) {
        while (DL_SPI_isBusy(SPI_0_INST))
            ;
        DL_SPI_transmitData8(SPI_0_INST, data[i]);
    }
}

void AD_Init(void)
{
  cs_9833_1();
}
void AD_Write(unsigned short TxData)
{
  unsigned char idata[2];
  idata[0] =(unsigned char)((TxData>>8)&0xff);
  idata[1] = (unsigned char) (TxData&0xff);
  cs_9833_0();
  SPI_Controller_transmitData(idata,2);
  cs_9833_1();
}
//Reset:0
//Sleepmode:3
//opt|modebit 00-sin 01 10 11
//

void AD_CtrlSet(unsigned char Reset,unsigned char SleepMode,unsigned char optionbit,unsigned char modebit)
{
  unsigned short regtmep =0;
  regtmep = regtmep|(((unsigned short)Reset&0x0l)<<8);
  regtmep = regtmep|((SleepMode&0x03)<<6);
  regtmep= regtmep|((optionbit&0x01)<<5);
  regtmep = regtmep|((modebit&0x0l)<<1);
  AD_Write(regtmep);
  //AD9833 Write(0x2000regtmep):
}
void AD_FreqSet(double Freq)
{
  int frequence_LSB,frequence_MSB;
  double frequence_mid,frequence_DATA;
  long int frequence_hex;
  frequence_mid=268435456/25;
  frequence_DATA=Freq;
  frequence_DATA=frequence_DATA/1000000;
  frequence_DATA=frequence_DATA*frequence_mid;
  frequence_hex=(long int)frequence_DATA;//32bit
  frequence_LSB=frequence_hex;
  frequence_LSB=frequence_LSB&0x3fff; //14bit
  frequence_MSB=frequence_hex>>14;
  frequence_MSB=frequence_MSB&0x3fff; //14bit
  frequence_LSB=frequence_LSB|0x4000;
  frequence_MSB=frequence_MSB|0x4000;
  AD_Write(0x2100);
  AD_Write(frequence_LSB);//L14
  AD_Write(frequence_MSB); //H14
}

