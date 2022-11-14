#include <msp430.h> 
#include <stdio.h>
#include <stdint.h>
#include <math.h>


/**
 * main.c
 */

//Analog input P7.6

volatile unsigned long int ADC_value = 0;
volatile unsigned long int j, k = 0;

void word_to_string(volatile unsigned long int  number);
void tostring(char [], int);
void decToHexa(volatile unsigned long int n);

//----------------LCD----------------------------//
unsigned char str[4];
int pos = 0;
volatile unsigned int refresh = 0;
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
void init_LCD();
void move_cursor(int target);
void write_char (unsigned char data);
void write_string(char *s);
//---------------------------------------------//


int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;   // stop watchdog timer

//-------Init LCD pins-------------------//
    P3DIR |= (BIT4 | BIT5 | BIT6 | BIT7);
    P4DIR |= (BIT0 | BIT1 | BIT2 | BIT3);
    P5DIR |= (BIT0 | BIT1 | BIT5);
//---------------------------------------//

//----------Timer setup A0---------------------------------------------------//
    TA0CTL |= TACLR;            //Reset timer in register TAxCTL |= TACLR;
    TA0CTL |= TASSEL__ACLK;     //Select clock source TASSEL |= TASSEL_ACLK;
    TA0CTL |= MC__UP;           //Select Timer/Counter in up mode to CCR0
    TA0CCR0 |= 4095;            //Compare register setup
    TA0CTL |= ID__8;            //Set up prescaler div-8
//---------------------------------------------------------------------------//

//--------------------Setup TA0 compare IRQ----------------------------------//
    TA0CCTL0 |= CCIE;           //Local enable for CCR0
    __enable_interrupt();       //Enable maskable IRQs
    TA0CCTL0 &=~ CCIFG;         //Clear IRQ flags TA0CTL |= CCIFG;
//---------------------------------------------------------------------------//

//-------------------------Setup ports---------------------------------------//
    P7DIR &=~ BIT4; //Analog input P7.6 (A14)
    P7SEL |= BIT4;
    P7OUT &=~ BIT4;
//---------------------------------------------------------------------------//

//---------------------------ADC10 Setup-------------------------------------//
    ADC12CTL0 |= ADC12SHT10;    //Sample hold time = 16 cycles
    ADC12CTL0 |= ADC12ON;       //ADC12 ON

    ADC12CTL1 |= ADC12SSEL_1;   //Clock Source Select ACLK 32.768 kHz
    ADC12CTL1 |= ADC12SHP;      //Sampling singal from sample timer

    ADC12CTL2 |= ADC12RES_2;    //Resolution set to 12 bits

    ADC12MCTL0 |= ADC12INCH_14; //A14 as source
//---------------------------------------------------------------------------//


    init_LCD();



    while(1){

        ADC12CTL0 |= (ADC12ENC_L | ADC12SC_L);

        if(refresh){
            decToHexa(ADC_value);
            for (k = 1; k <= 4; k++){
                write_char(str[4-k]);
            }
            move_cursor(0);
            refresh = 0;
        }
    }
}

// //Decimal
// void word_to_string(volatile unsigned long int number){
//     for(j = 0; j < 16; j++){
//       tostring(str,(number));
//     }
// }

// //Change Value tostring
// void tostring(char str[], int num)
// {
//     int i, rem, len = 0, n;

//     n = num;
//     while (n != 0)
//     {
//         len++;
//         n /= 10;
//     }
//     for (i = 0; i < len; i++)
//     {
//         rem = num % 10;
//         num = num / 10;
//         str[len - (i + 1)] = rem + '0';
//     }
//     str[len] = '\0';
// }

//Decimal to hexadecimal
void decToHexa(volatile unsigned long int n)
{
        while (n != 0) {
            // temporary variable to
            // store remainder
            int temp = 0;

            // storing remainder in temp
            // variable.
            temp = n % 16;

            // check if temp < 10
            if (temp < 10) {
                str[j] = (char)(temp + 48);
            }
            else {
                str[j] = (char)(temp + 55);
            }
                j++;
            n = n / 16;
    }
        j=0;

}


//----------------------------ISRs-----------------------------------//
#pragma vector = TIMER0_A0_VECTOR
__interrupt void ISR_Timer_A0_CCR0(void){
    ADC_value = ADC12MEM0;
    refresh = 1;
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

    if(data == 0){
      data = '0';
    }

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

void init_LCD(){
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

