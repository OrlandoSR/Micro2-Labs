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
#define DISPLAY_0 SEGG
#define DISPLAY_1 (SEGG | SEGF | SEGE | SEGD | SEGA)
#define DISPLAY_2 (SEGF | SEGC)
#define DISPLAY_3 (SEGF | SEGE)
#define DISPLAY_4 (SEGE | SEGD | SEGA)
#define DISPLAY_5 (SEGE | SEGB)
#define DISPLAY_6 (SEGB)
#define DISPLAY_7 (SEGG | SEGF | SEGE | SEGD)
#define DISPLAY_8 ()
#define DISPLAY_9 (SEGE | SEGD)
#define DISPLAY_A (SEGD)
#define DISPLAY_B (SEGB | SEGA)
#define DISPLAY_C (SEGF | SEGC | SEGB | SEGA)
#define DISPLAY_D (SEGF | SEGA)
#define DISPLAY_E (SEGC | SEGB)
#define DISPLAY_F (SEGD | SEGC | SEGB)

volatile unsigned int j = 0;
int update = 0;
void segment_v(int value);

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
    WDTCTL = WDTPW | WDTHOLD;   // stop watchdog timer

    //Setup Ports
    P4DIR |= 0xFF;
    P5DIR |= (BIT0 | BIT1);    //Enable 7s_1 and 7s_2 Port's (Control Signal)

    //Timer setup
    TB0CTL |= TBCLR;            //Reset timer in register TBxCTL |= TBCLR;
    TB0CTL |= TBSSEL_1;         //Select clock source TBSSEL |= TBSSEL_ACLK;
    TB0CTL |= MC__UP;           //Select Timer/Counter in up mode to CCR0
    TB0CTL |= CNTL_0;           //Select counter length to 16-bit
    TB0CCR0 |= 2048;            //Compare register setup // 33 , 16 , 11 , 8 , 6
    TB0CTL |= ID__8;            //Set up prescaler div-8

    //Setup TB0 compare IRQ
    TB0CCTL0 |= CCIE;           //Local enable for CCR0
    __enable_interrupt();       //Enable maskable IRQs
    TB0CCTL0 &=~ CCIFG;         //Clear IRQ flags TB0CTL |= CCIFG;

    while(1){
        if(update){

            if(j < 16){

                //Turns off control signals
                P5OUT &=~ (BIT0 | BIT1);

                //Enables Iteration
                P4OUT &=~ 0xFF;
                segment_v(j);  //Iterate's through 0-9 & A-F segment values
                j++;

                //Turns on control signal 7s_1
                P5OUT |= (BIT0);

                //Delay Loop
                __delay_cycles(1000000);

                //Turns off control signals
                P5OUT &=~ (BIT0 | BIT1);

                //Turns on control signal 7s_2
                P5OUT |= (BIT1);

                //Delay Loop
                __delay_cycles(1000000);

            }
            else {
                j = 0;
            }

            update = 0;
        }
    }

//    return 0;
}

void segment_v(int value){
    switch(value){
        case (0):
               P4OUT |= (BIT7);
               break;
        case (1):
               P4OUT |= (BIT7 | BIT6 | BIT5 | BIT3 | BIT0);
               break;
        case (2):
               P4OUT |= (BIT6 | BIT2);
               break;
        case (3):
               P4OUT |= (BIT6 | BIT5);
               break;
        case (4):
               P4OUT |= (BIT5 | BIT3 | BIT0);
               break;
        case (5):
               P4OUT |= (BIT5 | BIT1);
               break;
        case (6):
               P4OUT |= (BIT1);
               break;
        case (7):
               P4OUT |= (BIT7 | BIT6 | BIT5 | BIT3);
               break;
        case (8):
               P4OUT &=~ 0xFF;
               break;
        case (9):
               P4OUT |= (BIT5 | BIT3);
               break;
        case (10): //A
               P4OUT |= (BIT3);
               break;
        case (11): //B
               P4OUT |= (BIT1 | BIT0);
               break;
        case (12): //C
               P4OUT |= (BIT6 | BIT2 | BIT1 | BIT0);
               break;
        case (13): //D
               P4OUT |= (BIT6 | BIT0);
               break;
        case (14): //E
               P4OUT |= (BIT2 | BIT1);
               break;
        case (15): //F
               P4OUT |= (BIT3 | BIT2 | BIT1);
               break;
    }
}

//----------------------------ISRs-----------------------------------//
#pragma vector = TIMER0_B0_VECTOR
__interrupt void ISR_Timer_B0_CCR0(void){
    update = 1;
    P1OUT ^= BIT4;
    TB0CCTL0 &=~ CCIFG;
}
