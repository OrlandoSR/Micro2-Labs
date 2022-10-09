#include <msp430.h> 
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define REFRESH display_clear()
#define COL1 BIT5
#define COL2 BIT2
#define COL3 BIT4

#define ROW4 BIT7
#define ROW3 BIT6
#define ROW2 BIT5
#define ROW1 BIT4


volatile unsigned char i= '\0';
volatile unsigned char prev = '\0';
volatile unsigned int j = 0;

char buff[20];

// LCD

int pos = 0;
int update = 0;
int SCAN_CODE = 0;
int RETURN_CODE = 0;

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
int scan_code();
void decode();

// Pins to be used P1.4 for Schmidt trigger and P2.6 for onboard switch
// ROW4 7.7 , ROW3 7.6, ROW2 7.5, ROW1 7.4
// COL3 1.4, COL2 1.2, COL1 1.5


int main(void)
{
	WDTCTL = WDTPW | WDTHOLD;	// stop watchdog timer

	//Init LCD pins
	    P3DIR |= (BIT4 | BIT5 | BIT6 | BIT7);
	    P4DIR |= (BIT0 | BIT1 | BIT2 | BIT3);
	    P5DIR |= (BIT0 | BIT1 | BIT5);

	    init();
	
	// Port Configurations
	    P2DIR &=~ BIT6;     // Config pin as input    (PxDIR)
	    P2OUT &=~ BIT6;     // Clear pin value for output    (PxOUT)

	    P2REN |= BIT6;      // Enable resistor for for input (PxREN)
	    P2OUT |= BIT6;      // Enable direction of resistor pull up/down (PxOUT)
	    P2IES |= BIT6;      // Select interrupt edge sensitivity (PxIES)


	//Set the pins connected to the columns as inputs
	    P1DIR &=~ (BIT4 | BIT2 | BIT5);     // Config pin's as input
	    P1IES |= (BIT4 | BIT2 | BIT5);     // Select interrupt edge sensitivity (PxIES)

	//Set the pins connected to the rows as outputs
	    P7DIR |= (BIT4| BIT5 | BIT6 | BIT7);
	    P7OUT = BIT4;           // Config pin as input

	// Setup IRQ
	    P2IE |= BIT6;           // Local enable for interrupt for a given bit of the port (PxIE)
	    P1IE |= (BIT5 | BIT2 | BIT4);
	    __enable_interrupt();   // Enable maskable interrupt (__enable_interrupt();)
	    P2IFG &=~ BIT6;         // Clear the interrupt flag for the given bit in the port (PxIFG)
	    P1IFG &=~ (BIT5 | BIT2 | BIT4); // Local enable for interrupt for a given bit of the port (PxIE)

//	    sprintf(buff, "Pressed %c", i);
//	    write_string(buff);

	while(1){
	    SCAN_CODE = scan_code();

	    if(update){
	        decode();
//	        if(prev != i){
//	            REFRESH;
//	        }
	        write_char(i);
	        __delay_cycles(1000000);
	        update = 0;
	    }
	}

}

void decode(){
    switch(SCAN_CODE){
        case (ROW4):
            if(RETURN_CODE == COL1){
                i = '*';
            }else if(RETURN_CODE == COL2){
                i = '0';
            }
            else if(RETURN_CODE == COL3){
                i = '#';
            }
        break;
        case (ROW3):
            if(RETURN_CODE == COL1){
                i = '7';
            }else if(RETURN_CODE == COL2){
                i = '8';
            }
            else if(RETURN_CODE == COL3){
                i = '9';
            }
        break;
        case (ROW2):
             if(RETURN_CODE == COL1){
                 i = '4';
             }else if(RETURN_CODE == COL2){
                 i = '5';
             }else if(RETURN_CODE == COL3){
                 i = '6';
             }
        break;
        case (ROW1):
            if(RETURN_CODE == COL1){
                i = '1';
            }else if(RETURN_CODE == COL2){
                i = '2';
            }else if(RETURN_CODE == COL3){
                i = '3';
            }
        break;
    }
//    prev = i;

}

// KeyPad
int scan_code(){

    int scancode = P7OUT;
    if(j < 4){              //Increments of row variable
        P7OUT = P7OUT << 1;
        j++;
    } else {
        P7OUT = BIT4;   // Config pin as input
        j = 0;
    }
    return scancode;

}

// ISRs
#pragma vector = PORT1_VECTOR
__interrupt void ISR_P1(void){
    update = 1;
    if(COL1 & P1IFG){
        RETURN_CODE = COL1;
    } else if(COL2 & P1IFG) {
        RETURN_CODE = COL2;
    } else if(COL3 & P1IFG) {
        RETURN_CODE = COL3;
    }

    P1IFG &=~ 0xFF;         // clear IFG for the bit of the port
}













///////////////////////////////////////------------LCD-----------////////////////////////////////////////////////////////////////////
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


