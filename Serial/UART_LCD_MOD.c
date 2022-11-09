#include <msp430.h> 
#include <ctype.h>


/**
 * main.c
 */

/* Serial Communication Settings
 *
 *  Baud Rate = 9600
 *  Data Bits = 8
 *  Parity Bits = None
 *  Flow Control = None
 *
 *
 * MCU Baud Rate Settings (Family Guide p.952)
 *  BRCLK Frequency | Baud Rate | UCBRx | UCBRSx | USBRFx
 *      32,768          9600        3       3       0
 *    1,048,576         9600        109     2       0
 *
 */

/*
 * To initialize or reconfigure USCI module
 *  Set UCSWRST (UCAxCTL1 |= UCSWRST)
 *  Initialize all USCI registers with UCSWRST = 1
 *  Configure all ports
 *  Clear UCSWRST via software (UCAxCTL1 &=~ UCSWRST)
 *  Enable interrupts if needed
 *
 */

/*
 * USCI Module used is USCI_A2
 *
 * Use Tx = UCA2TXD (P9.4) and Rx = UCA2RXD (P9.5)
 *
 */

//----------------LCD----------------------------//
int pos = 0;
volatile unsigned long int i = 0;
volatile int direction = 0;

void display_string(int row);
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
//---------------------------------------------//

volatile unsigned long int j = 0;
volatile unsigned long int out_on = 0;
void string_in(char c);     //Enter characters recieved in the buffer
void serial_write();        //Write characters in buffer


#define MAX_CHAR 16

unsigned char txbuff[MAX_CHAR];
unsigned char rxbuff[MAX_CHAR];


int main(void)
{
    //Init LCD pins
    P3DIR |= (BIT4 | BIT5 | BIT6 | BIT7);
    P4DIR |= (BIT0 | BIT1 | BIT2 | BIT3);
    P5DIR |= (BIT0 | BIT1 | BIT5);

    WDTCTL = WDTPW | WDTHOLD;   // stop watchdog timer

/*  UART Configuration
    *   Configure baud control registers
    *   Select clock source, specify prescaler and divider
    *   Specify Asynchronous mode in control register
    *   Enable transmitter and global USART module
    *   Configure Tx and Rx pins selected to work with UART module
*/

    //USCI control registers

    UCA2CTL1 |= UCSWRST;    //Enable modificattion to all USCI control registers

    //UCA2CTL0 (Control Register 0)
    UCA2CTL0 &=~ UCSYNC;    //Set asynchronous mode
    UCA2CTL0 |= UCMODE_0;   //Set UART mode
    UCA2CTL0 &=~ (UCPAR | UCMSB | UC7BIT | UCSPB); //Configure parity, endianness, data size and stop bits

    //UCA2CTL1 (Control Register 1)
    UCA2CTL1 |= UCSSEL__SMCLK;    //BRCLK selected is SMCLK

    //UCA2BRW   (Baud Rate Select [use table above])
    UCA2BRW = 109;            //Set Baud Rate to 9600 / This edits the 16-bit word insted of two 8-bit registers

    //UCA2MCTL
    UCA2MCTL |= (BIT2);        //Set Baud Rate

    UCA2CTL1 &=~ UCSWRST;   //Disable modificattion to all USCI control registers

    //UCA2IE
    UCA2IE |= (UCRXIE);   //Local interrupt enable for Tx and Rx

    __enable_interrupt();      //Global interrupt enable


    //Pin Configuration
    P9SEL |= (BIT4 | BIT5);

    init();


    while(1){
        serial_write();
    }
}

void string_in(char c){
    if(isdigit(c)){return;}
    if(c == '*'){
        display_clear();
        j = 0;
        return;
    }

    rxbuff[j] = c;
    write_char(c);
    j++;
    if(j > 15){
        j = 0;
        out_on = 1;
        //Call other function
    }
    //Checks if value is in a specific position to move cursor to the next line
    else if(j == 15){
        if(pos == 15){
            move_cursor(40);
        }else if(pos == 55){
            move_cursor(0);
        }
    }
}

void serial_write(){
    if((UCA2IFG & BIT1) && out_on){   //Check if Tx buffer is empty
             UCA2TXBUF = toupper(rxbuff[j]);  //Send data to tx buffer
             j++;
             if(j > 15) {
                 j = 0;
                 out_on = 0;
                 UCA2TXBUF = '\n';
             } //Reset index to zero when out of array bounds
         }
    }


//----------------------ISR--------------------//
#pragma vector = USCI_A2_VECTOR
__interrupt void ISR_Rx(void){
    // do whatever
    if(UCA2IFG & BIT0){ // A character has been recieved in buffer
        string_in(UCA2RXBUF);
    }

    UCA2IFG &=~ BIT0;   // Clear interrupt flag (optional for this)

}

//----------------------------LCD-----------------------------------//

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
    __delay_cycles(1000);
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
    _delay_cycles(1000000);
    function_set_cmd();
    _delay_cycles(100000);
    function_set_cmd();
    _delay_cycles(100000);
    function_set_cmd();
    _delay_cycles(100000);
    function_set_cmd();
    _delay_cycles(100000);
    display_off();
    _delay_cycles(100000);
    display_clear();
    _delay_cycles(100000);
    entry_mode();
    _delay_cycles(100000);
    display_on();
    _delay_cycles(100000);
}
