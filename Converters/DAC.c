#include <msp430.h> 


/**
 * main.c
 */

/*
 * Data Converter Module
 * D0-D3 P7.4-P7.7
 * D4-D6 P4.5.P4.7
 * D7 P7.3
 *
 */

volatile unsigned long int i = 0;
void hex_to_ports(unsigned int num);

unsigned int DAC_values[12] = {0x00, 0x17, 0x2E, 0x45, 0x5C, 0x73, 0x8A, 0xA1, 0xB8, 0xCF, 0xE6, 0xFF};


int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;   // stop watchdog timer

  //Timer setup A0
    TA0CTL |= TBCLR;            //Reset timer in register TBxCTL |= TBCLR;
    TA0CTL |= TBSSEL_1;         //Select clock source TBSSEL |= TBSSEL_ACLK;
    TA0CTL |= MC__UP;           //Select Timer/Counter in up mode to CCR0
    TA0CTL |= CNTL_0;           //Select counter length to 16-bit
    TA0CCR0 |= 68;              //Compare register setup
    TA0CTL |= ID__8;            //Set up prescaler div-8



  //Init Ports
  P7DIR |= (BIT3 |BIT4 | BIT5 | BIT6 | BIT7);
  P4DIR |= (BIT4 | BIT7);


    while(1){
        for( i = 0; i < 12; i++){
            hex_to_ports(DAC_values[i]);
        }
    }
}



void hex_to_ports(unsigned int num){ //Takes in value and translates for each port

  //D0
  if(num & BIT0){
    P7OUT |= (BIT4);
  }else{
    P7OUT &=~ (BIT4);
  }

  //D1
  if(num & BIT1){
    P7OUT |= (BIT5);
  }else{
    P7OUT &=~ (BIT5);
  }

  //D2
  if(num & BIT2){
    P7OUT |= (BIT6);
  }else{
    P7OUT &=~ (BIT6);
  }

  //D3
  if(num & BIT3){
    P7OUT |= (BIT7);
  }else{
    P7OUT &=~ (BIT7);
  }

  //D4
  if(num & BIT4){
    P4OUT |= (BIT5);
  }else{
    P4OUT &=~ (BIT5);
  }

  //D5
  if(num & BIT5){
    P4OUT |= (BIT6);
  }else{
    P4OUT &=~ (BIT6);
  }

  //D6
  if(num & BIT6){
    P4OUT |= (BIT7);
  }else{
    P4OUT &=~ (BIT7);
  }

  //D7
  if(num & BIT7){
    P7OUT |= (BIT3);
  }else{
    P7OUT &=~ (BIT3);
  }

}
