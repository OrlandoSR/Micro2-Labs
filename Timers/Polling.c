#include <msp430.h> 


/**
 * main.c
 */
int main(void)
{
	WDTCTL = WDTPW | WDTHOLD;	// stop watchdog timer
	
	//Setup ports
	        P1DIR |= BIT4;      //Set output pin for buzzer P1.4
	        P1OUT &=~ BIT4;     //Clear output

	//Timer setup
	        TB0CTL |= TBCLR;            //Reset timer in register TBxCTL |= TBCLR;
	        TB0CTL |= TBSSEL_1;         //Select clock source TBSSEL |= TBSSEL_ACLK;
	        TB0CTL |= MC__UP;           //Select Timer/Counter in up mode to CCR0
	        TB0CTL |= CNTL_0;           //Select counter length to 16-bit
	        TB0CCR0 |= 33;              //Compare register setup

	//Setup TBx overflow IRQ
	        //Local enable for overflow TB0 |= TBIE;
	        //Enable maskable IRQs
	        //Clear IRQ flags TB0CTL |= TBIFG;

	while(1){
	    if(TB0CCTL0 & CCIFG){
	        P1OUT ^= BIT4;
	        TB0CCTL0 &=~ CCIFG;
	    }
	}


	return 0;
}

//----------------------------ISRs-----------------------------------//
#pragma vector = TIMER0_B1_VECTOR
__interrupt void ISR_Timer_B0(void){

}


















