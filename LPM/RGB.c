#include <msp430.h> 

#define MAX_TIME  (1000)
#define MAX_RGB 255

#define RED_RGB TA0CCR4
#define GREEN_RGB TA0CCR3
#define BLUE_RGB TA0CCR1

int red = 500;
int green = 500;
int blue = 500;

volatile unsigned long int i = 1;


int RGB_to_CCR(int value){
  int CCR_value = 0;

  if(value > 255){ return MAX_TIME; }
  if(value < 0){ return 0;}

  CCR_value = ((value/255) * 1000);

  return CCR_value;
}


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

    //Set frequency values
    TA0CCR0  = (MAX_TIME);
    TA0CCR1  = (blue);
    TA0CCR3  = (green);
    TA0CCR4  = (red);

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
  switch(i)
  {
    case 1:
        RED_RGB = RGB_to_CCR(0);
        GREEN_RGB = RGB_to_CCR(0);
        BLUE_RGB = RGB_to_CCR(255);
        break;
    case 2:
        RED_RGB = RGB_to_CCR(0);
        GREEN_RGB = RGB_to_CCR(255);
        BLUE_RGB = RGB_to_CCR(0);
        break;
    case 3:
        RED_RGB = RGB_to_CCR(255);
        GREEN_RGB = RGB_to_CCR(0);
        BLUE_RGB = RGB_to_CCR(0);
        break;
    case 4:
        RED_RGB = RGB_to_CCR(255);
        GREEN_RGB = RGB_to_CCR(30);
        BLUE_RGB = RGB_to_CCR(217);
        break;
    case 5:
        RED_RGB = RGB_to_CCR(30);
        GREEN_RGB = RGB_to_CCR(222);
        BLUE_RGB = RGB_to_CCR(252);
        break;
    case 6:
        RED_RGB = RGB_to_CCR(240);
        GREEN_RGB = RGB_to_CCR(200);
        BLUE_RGB = RGB_to_CCR(40);
        break;
    case 7:
        RED_RGB = RGB_to_CCR(255);
        GREEN_RGB = RGB_to_CCR(123);
        BLUE_RGB = RGB_to_CCR(33);
        break;
    case 8:
        RED_RGB = RGB_to_CCR(255);
        GREEN_RGB = RGB_to_CCR(255);
        BLUE_RGB = RGB_to_CCR(255);
        break;
    default:
        RED_RGB = RGB_to_CCR(0);
        GREEN_RGB = RGB_to_CCR(0);
        BLUE_RGB = RGB_to_CCR(0);
        i = 1;
  }
  i++;
  P2IFG &=~ BIT6;         // clear IFG for the bit of the port
}
