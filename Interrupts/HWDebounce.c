#include <msp430.h> 
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define REFRESH display_clear()

volatile unsigned long int i = 0;
char buff[20];

// LCD

int pos = 0;
int update = 0;

void function_clear();
void send_enable();
void display_off();
void display_clear();
void entry_mode();
void function_set_cmd();
void display_on();
void init();
void move_cursor(int target);
void write_char (unsigned char data);
void write_string(char *s);

// Pins to be used P1.4 for Schmidt trigger and P2.6 for onboard switch

int main(void)
{
	WDTCTL = WDTPW | WDTHOLD;	// stop watchdog timer

	    //Init LCD pins
	    P3DIR |= (BIT4 | BIT5 | BIT6 | BIT7);
	    P4DIR |= (BIT0 | BIT1 | BIT2 | BIT3);
	    P5DIR |= (BIT0 | BIT1 | BIT5);

	    init();
	
	// Port Configurations
	    P1DIR &=~ BIT4;     // Config pin as input    (PxDIR)
	    P1OUT &=~ BIT4;     // Clear pin value for output    (PxOUT)

//	    P2REN |= BIT6;      // Enable resistor for for input (PxREN)
//	    P2OUT |= BIT6;      // Enable direction of resistor pull up/down (PxOUT)
	    P1IES |= BIT4;      // Select interrupt edge sensitivity (PxIES)

	// Setup IRQ
	    P1IE |= BIT4;           // Local enable for interrupt for a given bit of the port (PxIE)
	    __enable_interrupt();   // Enable maskable interrupt (__enable_interrupt();)
	    P1IFG &=~ BIT4;         // Clear the interrupt flag for the given bit in the port (PxIFG)

	    sprintf(buff, "Pressed %d times", i);
	    write_string(buff);

	while(1){
	    if(update){
	        REFRESH;
	        sprintf(buff, "Pressed %d times", i);
	        write_string(buff);
	        update = 0;
	    }

	}
}

// ISRs

#pragma vector = PORT1_VECTOR
__interrupt void ISR_P1_PB3(void){
    i++;                    // increase number of times pressed
    update = 1;
    P1IFG &=~ BIT4;         // clear IFG for the bit of the port
}

void write_string(char *s){
    while(*s){
        write_char(*s);
        s++;
    }
}

void write_char(unsigned char data){

    function_clear();

    P5OUT &=~BIT0;
    P5OUT |= BIT1;

    P3OUT |= data;
    P4OUT |= data;

    send_enable();

    pos++;
}

void move_cursor(int target){
    function_clear();

    P3OUT |= BIT4;

    while(target != pos){
        if(target > pos){
            P4OUT |= BIT2;
            send_enable();
            pos++;

        }else if(target < pos){
            P4OUT &=~ BIT2;
            send_enable();
            pos--;
        }
    }
}

void function_clear(){
    P3OUT &=~ 0xFF;
    P4OUT &=~ 0xFF;
    P5OUT &=~ 0xFF;
}

void send_enable(){
    P5OUT |= BIT5;
//  __delay_cycles(1000);
    P5OUT &=~ BIT5;
}

void display_off(){
    function_clear();
    P4OUT |= BIT3;
    send_enable();
}

void display_clear(){
    function_clear();
    P4OUT |= BIT0;
    send_enable();
    pos = 0;
}

void entry_mode(){
    function_clear();
    P4OUT |= (BIT1 | BIT2);
    send_enable();
}

void function_set_cmd(){
    function_clear();
    P3OUT |= (BIT4 | BIT5);
    P4OUT |= BIT3;
    send_enable();
}

void display_on(){
    function_clear();
    P4OUT |= (BIT0 | BIT1 | BIT2 | BIT3);
    send_enable();
}

void init(){
    P5OUT &=~ (BIT0 | BIT1);
    __delay_cycles(1000000);
    function_set_cmd();
    __delay_cycles(100000);
    function_set_cmd();
    __delay_cycles(100000);
    function_set_cmd();
    __delay_cycles(100000);
    function_set_cmd();
    __delay_cycles(100000);
    display_off();
    __delay_cycles(100000);
    display_clear();
    __delay_cycles(100000);
    entry_mode();
    __delay_cycles(100000);
    display_on();
    __delay_cycles(100000);
}


