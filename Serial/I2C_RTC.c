#include <msp430.h> 


/**
 * main.c
 */

//P10.1 UCB3SDA
//P10.2 UCB3SCL


/*
 * RTC Register Address and Functions
 *
 *          Address         Function
 *------------------------------------------
 *          00h             Seconds
 *          01h             Minutes
 *          02h             Hours
 *          03h             Day
 *          04h             Date
 *          05h             Month
 *          06h             Year
 *          07h             Control
 *
 */

#define REGISTER_SIZE   8


#define REG_SECONDS     0x00
#define REG_MINUTES     0x01
#define REG_HOURS       0x02
#define REG_DAY         0x03
#define REG_DATE        0x04
#define REG_MONTH       0x05
#define REG_YEAR        0x06

char data = 0;

int checkACK();

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

int main(void)
{
	WDTCTL = WDTPW | WDTHOLD;	// stop watchdog timer
	
//-------Init LCD pins-------------------//
    P3DIR |= (BIT4 | BIT5 | BIT6 | BIT7);
    P4DIR |= (BIT0 | BIT1 | BIT2 | BIT3);
    P5DIR |= (BIT0 | BIT1 | BIT5);
//---------------------------------------//


    UCB3CTL1 |= UCSWRST;       //Enter software reset

    //UCB3CTL0
    UCB3CTL0 |= UCMODE_3;      //Set I2C mode
    UCB3CTL0 |= UCMST;         //Set MCU as Master
    UCB3I2CSA = 0x68;          //Slave address for RTC


	UCB3CTL1 |= UCSSEL__SMCLK; //Choose SMCLK 1,048,576 Hz
	UCB3BRW = 10;              //Divide by 10 for at SCL 104.8576kbps



	UCB3CTL1 &=~ UCSWRST;      //Enable UCB3

	UCB3IE |= (UCTXIE | UCRXIE | UCNACKIE);//Local interrupt enable

	//Pin function select
	P10SEL |= (BIT1 | BIT2);    //Select SDA/SCL functionality

	__enable_interrupt();       //Global interrupt enable


	init();                     //LCD INIT


	while(1){

	    //Write to RTC

	    UCB3CTL1 |= UCTR;       //Set Tx Mode
	    UCB3CTL1 |= UCTXSTT;    //Send START bit

	    while ((UCB3CTL1 & UCTXSTT) && ((UCB3IFG & UCTXIFG) == 0)); //Message done transmitting
	    //Start -> slave addr -> write -> ACK (RTC) -> Tx interrupt -> Exit while

	    //Read from RTC
	    UCB3CTL1 &=~ UCTR;      //Set Rx Mode
        UCB3CTL1 |= UCTXSTT;    //Send START bit

        while((UCB3CTL1 & UCTXSTT) && ((UCB3IFG & UCRXIFG) == 0)); //Wait for START bit to be sent and message to be recieved

        //Start -> slave addr -> -> Read ->  NACK (Master) -> STOP

        UCB3CTL1 |= UCTR;       //Set Tx Mode
        UCB3CTL1 |= UCTXNACK;
        while(UCB3CTL1 & UCTXNACK);
        UCB3CTL1 |= UCTXSTP;






//        display_clear();
//        write_char(data);
	}
}

int checkACK(){
    if(UCB3IFG & UCNACKIFG){
        return 0;
    }
    return 1;
}


//-----------------------------------ISR--------------------------------//
#pragma vector = USCI_B3_VECTOR
__interrupt void USCI_B3_I2C_ISR(void){
    switch(UCB3IV){
        case 0x04:               //NACK when ACK was expected
            UCB3CTL1 |= UCTXSTP; //Send STOP signal if NACK recieved
            break;

        case 0x0A:               //RXIFG, there's data in the RxBuffer
            data = UCB3RXBUF;
            break;

        case 0x0C:               //TXIFG, ready to transmit data (register to be read from)
            UCB3TXBUF = REG_SECONDS;    //Send register for seconds
            break;

        default:
            break;
    }
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


