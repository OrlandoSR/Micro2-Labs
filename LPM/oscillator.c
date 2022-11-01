#include <msp430.h> 


int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;   // stop watchdog timer

    //Init pins
    P8DIR |= BIT6; //Set pin 8.6 to the output direction.
    P8SEL |= BIT6; //Select pin 8.6 as our PWM output.
    TA1CCR0 = 1000; //Set the period in the Timer A1 Capture/Compare 0 register to 1000 us. //En otras palabras la frequencia de 2100 es 500Hz
    TA1CCTL1 = OUTMOD_7;
    TA1CCR1 = 500; //The period in microseconds that the power is ON. It's half the time, which translates to a 50% duty cycle.
    TA1CTL = TASSEL_2 + MC_1; //TASSEL_2 selects SMCLK as the clock source, and MC_1 tells it to count up to the value in TA0CCR0.

    //LPM
//    __low_power_mode_0(); //Set to turn on LPWM

    while(1){

    }

}


