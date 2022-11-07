#include <msp430.h> 


/**
 * main.c
 */

//P10.1 UCB3SDA
//P10.2 UCB3SCL


int main(void)
{
	WDTCTL = WDTPW | WDTHOLD;	// stop watchdog timer
	
	UCB3CTLW0 |= UCSWRST;       //Enter software reset
	UCB3CTLW0 |= UCSSEL__SMCLK; //Choose SMCLK 1,048,576 Hz
	UCB3BRW = 10;               //Divide by 10 for at SCL 104.8576kbps

	UCB3CTLW0 |= UCMODE_3;      //Set I2C mode
	UCB3CTLW0 |= UCMST;         //Set MCU as Master
	UCB3I2CSA = 0b1101000;      //Slave address for RTC

	// UCB3TBCNT needed for automatic stop but not defined in header and memory address unkown

	UCB3CTLW0 |= UCTXSTP;       //Generate automatic STOP bit preceeded by NACK

	UCB3CTLW0 &=~ UCSWRST;      //Enable UCB3

	UCB3IE |= (UCTXIE | UCRXIE);  //Local interrupt enable

	//Pin function select
	P10SEL |= (BIT1 | BIT2);    //Select SDA/SCL functionality


	__enable_interrupt();       //Global interrupt enable


	while(){

	}


	return 0;
}


//-----------------------------------ISR--------------------------------//
#pragma vector = USCI_B3_VECTOR
___interrupt void USCI_B3_I2C_ISR(void){

}
