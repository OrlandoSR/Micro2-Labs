#include <msp430.h> 


/**
 * main.c
 */

#define SEGA BIT0;
#define SEGB BIT1;
#define SEGC BIT2;
#define SEGD BIT3;
#define SEGE BIT5;
#define SEGF BIT6;
#define SEGG BIT7;

//DISPLAY VALUE'S ARE SET TO TURN OFF THESE SPECIFIC BITS
#define DISPLAY_0 (SEGG)
#define DISPLAY_1 (SEGG | SEGF | SEGE | SEGD | SEGA)
#define DISPLAY_2 (SEGF | SEGC)
#define DISPLAY_3 (SEGF | SEGE)
#define DISPLAY_4 (SEGE | SEGD | SEGA)
#define DISPLAY_5 (SEGE | SEGB)
#define DISPLAY_6 (SEGB)
#define DISPLAY_7 (SEGG | SEGF | SEGE | SEGD)
#define DISPLAY_8 ()
#define DISPLAY_9 (SEGE)
#define DISPLAY_A (SEGD)
#define DISPLAY_B (SEGB | SEGA)
#define DISPLAY_C (SEGF | SEGC | SEGB | SEGA)
#define DISPLAY_D (SEGF | SEGA)
#define DISPLAY_E (SEGC | SEGB)
#define DISPLAY_F (SEGD | SEGC | SEGB)

/*
 * SEG A 4.0
 *     B 4.1
 *     C   .2
 *     D   .3
 *     E   .5
 *     F   .6
 *     G   .7
 */
int main(void)
{
	WDTCTL = WDTPW | WDTHOLD;	// stop watchdog timer
	
	//Setup Ports
	P4DIR |= 0xFF;


	//Timer setup
        TB0CTL |= TBCLR;            //Reset timer in register TBxCTL |= TBCLR;
        TB0CTL |= TBSSEL_1;         //Select clock source TBSSEL |= TBSSEL_ACLK;
        TB0CTL |= MC__UP;           //Select Timer/Counter in up mode to CCR0
        TB0CTL |= CNTL_0;           //Select counter length to 16-bit
        TB0CCR0 |= 33;              //Compare register setup // 33 , 16 , 11 , 8 , 6



	return 0;
}
