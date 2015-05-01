/***
samples input and outputs a PWM comparator at 40 khz based on the input
output pin: 26 (PWM output)
input pin: 20 (samples a 0 - 3 V signal)
***/

#include "mbed.h"
#include <InterruptIn.h>

//unused data if sampling input
const int len = 8;
int sig[len] = {250, 500, 750, 1000, 1250, 1500, 1750, 2000};
//const int len = 4;
//int sig[len] = {1200, 2200, 200, 1200};
unsigned int i = 0;

//test leds
DigitalOut led1(LED1);
DigitalOut led2(LED2);
DigitalOut led3(LED3);
DigitalOut led4(LED4);

//output pins
DigitalOut botL(p10);
DigitalOut botM(p9);
DigitalOut botR(p8);
DigitalOut midL(p7);
DigitalOut midR(p6);
DigitalOut top(p5);

void setupADC() {
    LPC_SC->PCONP |= 1 << 12;//power ADC
    LPC_SC->PCLKSEL0 |= 3 << 24;//ADCclk = cpuclk/8 = 12 MHz
    LPC_PINCON->PINSEL3 |= 3 << 30;//select AD mode fr pin20 (audio input) 
       
    LPC_PINCON->PINSEL1 |= 1 << 14; //pin15 is AD mode (pos input);
     
    LPC_ADC->ADCR |= (1 << 5) //sample on AD0.5 (pin 20)
                | (1 << 0) //sample on AD0.0 (pin 0)
                | (1 << 16) //BURST mode (repeated conversion)
                | (1 << 21);//operate ADC            
} 

void setupPWM() {    
    LPC_SC->PCONP |= 1 << 6;//power PWM
    LPC_SC->PCLKSEL0 |= 1 << 12; //PWMclk = cpu clk (96MHz)
    LPC_PINCON->PINSEL4 = 1 << 2;//pin25 mode is pwm

//configure PWM
    LPC_SC->PCLKSEL0 |= 1<<12;//set PWM clk = MCU clk
    LPC_PWM1->MCR = (1 << 0); // PWM1 interrupt/reset on match w MR0
    LPC_PWM1->MR0 = 2400; //PWM1 reset every 2400 ticks (40 khz)
    LPC_PWM1->MR2 = 1200; //PWM1 output toggles when TCNT = MR2, init to 50% d.c.

    LPC_PWM1->TCR = 9;//enable pwm
    LPC_PWM1->PCR = 1 << 10;//(1<<0) | (1 << 3);//enable output on pin26       
}
    
void selectSpeaker(int pos) {
    if (pos < 2230) {
            botL = 1;
            botM = 0;
            botR = 0;
            midL = 0;
            midR = 0;
            top = 0;
            
    }
    else if (pos < 2604) {
            botL = 0;
            botM = 1;
            botR = 0;
            midL = 0;
            midR = 0;
            top = 0; 
            
    }  
    else if (pos < 2976) {
        botL = 0;
        botM = 0;
        botR = 1;
        midL = 0;
        midR = 0;
        top = 0; 
    
    }
    else if (pos < 3348) {
        botL = 0;
        botM = 0;
        botR = 0;
        midL = 1;
        midR = 0;
        top = 0; 

    }
    else if (pos < 3720) {
        botL = 0;
        botM = 0;
        botR = 0;
        midL = 0;
        midR = 1;
        top = 0; 
    }
    else {
        botL = 0;
        botM = 0;
        botR = 0;
        midL = 0;
        midR = 0;
        top = 1; 
    }

}    
    
     
int main(){
    printf("hello\r\n");
    setupADC();
    led3 = 1;
    //printf("adc set up\r\n");
    setupPWM();
    printf("pwm set up\r\n");  
        
    while(1){
        int pos = (LPC_ADC->ADDR0 >> 4) & 0xFFF;
        //printf("pos: %d\r\n", pos);
        selectSpeaker(pos);   
        //manual isr
        if ((LPC_PWM1->IR >> 0) & 1) {//check interrupt bit for MR0
            //sampled input
            int val = (LPC_ADC->ADDR5 >> 4) & 0xFFF; //sampled value on pin20
            LPC_PWM1->MR2 = val >> 1; //div by 2 to fit into 0 - 2048 (max counter is 2400)
            
            led2 = 1;
            //input from memory
            //LPC_PWM1->MR2 = sig[i];
            //i = (i+1)% len;
            LPC_PWM1->LER = 1<<2;            
            LPC_PWM1->IR |= 1<<0;
            
        }
    }   
}
    