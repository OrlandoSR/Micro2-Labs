#include <msp430.h> 


/**
 * main.c
 */
int main(void)
{
	WDTCTL = WDTPW | WDTHOLD;	// stop watchdog timer
	
	// Port Configurations
	    // Config pin as input/output    (PxDIR)
	    // Clear pin value for output    (PxOUT)

	    // Enable resistor for for input (PxREN)
	    // Enable direction of resistor pull up/down (PxOUT)
	    // Select interrupt edge sensitivity (PxIES)

	// Setup IRQ
	    // Local enable for interrupt for a given bit of the port (PxIE)
	    // Enable maskable interrupt (__enable_interrupt();)
	    // Clear the interrupt flag for the given bit in the port (PxIFG)

	while(1){

	}

	return 0;
}

// ISRs

#pragma vector = PORTx_VECTOR
__interrupt void ISR_Px_S1(void){
    // do whatever
    // clear IFG for the bit of the port
}


