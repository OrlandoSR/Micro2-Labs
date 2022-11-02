#include <msp430.h> 

#define MAX_TIME  (1000)
#define THIRTY (117)
#define TWO_TWENTY_TWO (870)
#define TWO_SEVEN_TEEN (850)
#define TWO_FIFTHY_TWO (988)
#define TWO_FOURTY (941)
#define TWO_HUNDRED (784)
#define FOURTY (156)
#define ONE_TWENTY_TRHEE (482)
#define THIRTY_THREE (129)

#define RED_RGB TA0CCR4
#define GREEN_RGB TA0CCR3
#define BLUE_RGB TA0CCR1


volatile unsigned long int i = 1;


int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;   // stop watchdog timer

    // Button setup P2.6
    P2DIR &= ~BIT6;         // Enable S1 as input (PxDIR)
    P2REN |= BIT6;      // Enable resistor for for input (PxREN)
    P2OUT |= BIT6;      // Enable direction of resistor pull up/down (PxOUT)
    P2IES |= BIT6;      // Select interrupt edge sensitivity (PxIES)


    //Button Interrupt
    P2IE |= BIT6;           // Local enable for interrupt for a given bit of the port (PxIE)
    __enable_interrupt();   // Enable maskable interrupt
    P2IFG &=~ BIT6;         // Clear the interrupt flag for the given bit in the port (PxIFG)

    //Init pins
    P1DIR |= (BIT5|BIT2|BIT4);
    P1SEL |= (BIT5|BIT2|BIT4);

    //Set Initial frequency values
    TA0CCR0  = (MAX_TIME);

    //Set PWM output mode
    TA0CCTL1 = (OUTMOD_7);
    TA0CCTL4 = (OUTMOD_7);
    TA0CCTL3 = (OUTMOD_7);

    // Start timer 1.
    TA0CTL  =  (TASSEL_2 | MC_1);

    //LPM
    __low_power_mode_0(); //Set to turn on LPWM

}


//ISRs
#pragma vector = PORT2_VECTOR
__interrupt void ISR_P2_S1(void){
  _delay_cycles(150000);
  switch(i)
  {
    case 0:
        RED_RGB = 0;
        GREEN_RGB = 0;
        BLUE_RGB = 0;
        break;
    case 1:
        RED_RGB = 0;
        GREEN_RGB = 0;
        BLUE_RGB = MAX_TIME;
        break;
    case 2:
        RED_RGB = 0;
        GREEN_RGB = MAX_TIME;
        BLUE_RGB = 0;
        break;
    case 3:
        RED_RGB = MAX_TIME;
        GREEN_RGB = 0;
        BLUE_RGB = 0;
        break;
    case 4:
        RED_RGB = MAX_TIME;
        GREEN_RGB = THIRTY;
        BLUE_RGB = TWO_SEVEN_TEEN;
        break;
    case 5:
        RED_RGB = THIRTY;
        GREEN_RGB = TWO_TWENTY_TWO;
        BLUE_RGB = TWO_FIFTHY_TWO;
        break;
    case 6:
        RED_RGB = TWO_FOURTY;
        GREEN_RGB = TWO_HUNDRED;
        BLUE_RGB = FOURTY;
        break;
    case 7:
        RED_RGB = MAX_TIME;
        GREEN_RGB = ONE_TWENTY_TRHEE;
        BLUE_RGB = THIRTY_THREE;
        break;
    case 8:
        RED_RGB = MAX_TIME;
        GREEN_RGB = MAX_TIME;
        BLUE_RGB = MAX_TIME;
        break;
    default:
        RED_RGB = 0;
        GREEN_RGB = 0;
        BLUE_RGB = 0;
        i = 1;
  }

  _delay_cycles(150000);
  i++;
  P2IFG &=~ BIT6;         // clear IFG for the bit of the port
}
