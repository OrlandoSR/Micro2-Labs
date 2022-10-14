#include <msp430.h> 


/**
 * main.c
 */
int main(void)
{
	WDTCTL = WDTPW | WDTHOLD;	// stop watchdog timer
	
	//Setup ports
	        //Set output pin for buzzer

	//Timer setup
	        //Reset timer in register TBxCTL |= TBCLR;
	        //Select clock source TBSSEL |= TBSSEL_ACLK;
	        //Slect Timer/Counter Mode TBxCTL |= MC__CONTINOUS;

	//Setup TBx overflow IRQ
	        //Local enable for overflow TB0 |= TBIE;
	        //Enable maskable IRQs
	        //Clear IRQ flags TB0CTL |= TBIFG;


	return 0;
}

//----------------------------ISRs-----------------------------------//
#pragma vector = TIMER0_B1_VECTOR
__interrupt void ISR_Timer_B0(void){

}


















