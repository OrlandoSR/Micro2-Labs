#include <msp430.h> 
#include <stdint.h>

/**
 * main.c
 */

/*
 * Data Converter Module
 * D0-D3 P3.0 - P3.3
 * D4-D7 P1.2 - P1.5
 *
 */

#define a (0)
#define b (128)
#define c (0)
#define d (255)

volatile unsigned long int i = 0;
void input_to_ports(unsigned int num);
void mapping(unsigned int num);
volatile unsigned long int  IPT_value = 0;

unsigned int DAC_values[12] = {0x00, 0x17, 0x2E, 0x45, 0x5C, 0x73, 0x8A, 0xA1, 0xB8, 0xCF, 0xE6, 0xFF};
//uint16_t values[4] = {0, 128, 255, 128};


int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;   // stop watchdog timer

  //Timer setup A0
    TA0CTL |= TBCLR;            //Reset timer in register TBxCTL |= TBCLR;
    TA0CTL |= TBSSEL__ACLK;    //Select clock source TBSSEL |= TBSSEL_ACLK;
    TA0CTL |= MC__UP;           //Select Timer/Counter in up mode to CCR0
    TA0CCR0 |= 1;              //Compare register setup
    TA0CTL |= ID__1;            //Set up prescaler div-8

    //Setup TA0 compare IRQ
    TA0CCTL0 |= CCIE;           //Local enable for CCR0
    __enable_interrupt();       //Enable maskable IRQs
    TA0CCTL0 &=~ CCIFG;         //Clear IRQ flags TA0CTL |= CCIFG;




  //Init Ports
  P3DIR |= (BIT0 | BIT1 | BIT2 | BIT3);
  P1DIR |= (BIT2 | BIT3 | BIT4 | BIT5);
  P3OUT &=~ 0xFF;
  P1OUT &=~ (BIT2 | BIT3 | BIT4 | BIT5);



    while(1){
//        for( i = 0; i < 12; i++){
//            hex_to_ports(DAC_values[i]);
//        }
//        mapping(i);
        input_to_ports(i);
    }
}

void mapping(unsigned int num){

    IPT_value = ((num-a)/(b-a)) * ((d-c) + c);

}


void input_to_ports(unsigned int num){ //Takes in value and translates for each port

  //D0
  if(num & BIT0){
    P3OUT |= (BIT0);
  }else{
    P3OUT &=~ (BIT0);
  }

  //D1
  if(num & BIT1){
    P3OUT |= (BIT1);
  }else{
    P3OUT &=~ (BIT1);
  }

  //D2
  if(num & BIT2){
    P3OUT |= (BIT2);
  }else{
    P3OUT &=~ (BIT2);
  }

  //D3
  if(num & BIT3){
    P3OUT |= (BIT3);
  }else{
    P3OUT &=~ (BIT3);
  }

  //D4
  if(num & BIT4){
    P1OUT |= (BIT2);
  }else{
      P1OUT &=~ (BIT2);
  }

  //D5
  if(num & BIT5){
     P1OUT |= (BIT3);
  }else{
     P1OUT &=~ (BIT3);
  }

  //D6
  if(num & BIT6){
     P1OUT |= (BIT4);
  }else{
     P1OUT &=~ (BIT4);
  }

  //D7
  if(num & BIT7){
     P1OUT |= (BIT5);
  }else{
     P1OUT &=~ (BIT5);
  }
}

//----------------------------ISRs-----------------------------------//
#pragma vector = TIMER0_A0_VECTOR
__interrupt void ISR_Timer_A0_CCR0(void){
    if(i > 255){i = 0;}
    i++;

}
