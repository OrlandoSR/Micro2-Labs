#include <msp430.h> 
#include <stdint.h>
#include <math.h>

/**
 * main.c
 */

/*
 * Data Converter Module
 * D0-D3 P3.0 - P3.3
 * D4-D7 P1.2 - P1.5
 *
 */

const double a = 0;
const double b = 128;
const double c = 0;
const double d = 255;

const double a1 = -1;
const double b1 = 1;
const double c1 = 0;
const double d1 = 255;

const double a2 = 0;
const double b2 = 255;
const double c2 = 0;
const double d2 = (2*M_PI);

#define SAMPLES 256

volatile unsigned long int k = 0;
volatile unsigned long int j = 0;
void input_to_ports(uint8_t num);
double mapping(double num);
void populate_array_sine();
volatile unsigned long int  IPT_value = 0;
double _8bit_to_radians(uint8_t num_in);
uint8_t sin_to_8bit(double num_in);

unsigned int DAC_values[12] = {0x00, 0x17, 0x2E, 0x45, 0x5C, 0x73, 0x8A, 0xA1, 0xB8, 0xCF, 0xE6, 0xFF};
uint16_t values[SAMPLES];


int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;   // stop watchdog timer
  //Timer setup A0
    TA0CTL |= TBCLR;            //Reset timer in register TBxCTL |= TBCLR;
    TA0CTL |= TBSSEL__ACLK;    //Select clock source TBSSEL |= TBSSEL_ACLK;
    TA0CTL |= MC__UP;           //Select Timer/Counter in up mode to CCR0
    TA0CCR0 |= 16;              //Compare register setup
    TA0CTL |= ID__1;            //Set up prescaler div-8

    //Setup TA0 compare IRQ
    TA0CCTL0 |= CCIE;           //Local enable for CCR0
    TA0CCTL0 &=~ CCIFG;         //Clear IRQ flags TA0CTL |= CCIFG;




  //Init Ports
  P3DIR |= (BIT0 | BIT1 | BIT2 | BIT3);
  P1DIR |= (BIT2 | BIT3 | BIT4 | BIT5);
  P3OUT &=~ 0xFF;
  P1OUT &=~ (BIT2 | BIT3 | BIT4 | BIT5);

  populate_array_sine();

  __enable_interrupt();       //Enable maskable IRQs


    while(1){
//        for( i = 0; i < 12; i++){
//            hex_to_ports(DAC_values[i]);
//        }
//        mapping(i);
        input_to_ports(values[k]);
    }
}

uint8_t sin_to_8bit(double num_in){
    double res = 0;

    res = ((num_in-a1)/(b1-a1)) * ((d1-c1) + c1);

    return res;
}

double _8bit_to_radians(uint8_t num_in){

    double res = 0;

    res = ((num_in-a2)/(b2-a2)) * ((d2-c2) + c2);

    return res;
}



void populate_array_sine(){

    for(k = 0; k < 256; k++){
       values[k] = sin_to_8bit(sin(_8bit_to_radians(k)));
    }
}


double mapping(double num){

    double res = 0;

    res = ((num-a)/(b-a)) * ((d-c) + c);

    return res;
}


void input_to_ports(uint8_t num){ //Takes in value and translates for each port

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
    if(k > 255){k = 0;}
    k++;

}
