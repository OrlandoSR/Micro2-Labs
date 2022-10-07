#include <msp430.h> 


/**
 * main.c
 */

//Cable de 5V rojo Vcc y negro es GND
// RS and RW P5.1 y P5.0
// DB0 Violeta DB1 Gris DB2 Blanco DB3 Brown DB4 Rojo DB5 Amarillo DB6 Verde DB7 Orange
// DB0 - DB3 P4.0 ... P4.3
// DB4 - P3.4, DB5 P3.5, DB6 P3.6, DB7 P3.7
// Plugin enable and give negative edge for enable
// Enable P5.5

// S/C 0 for moving cursor only 1 for whole display
// R/L 0 left 1 right
// If both are 1 move entire display to the right

//Wait 15ms
//Write RS RW D7 D6 D5 D4 D3 D2 D1 D0
//      0   0  0  0  1  1  x  x  x  x
//Wait 4.1ms
//      RS RW D7 D6 D5 D4 D3 D2 D1 D0
//      0   0  0  0  1  1  x  x  x  x
//Wait 100us
//      RS RW D7 D6 D5 D4 D3 D2 D1 D0
//      0   0  0  0  1  1  x  x  x  x
//Function Set
//      RS RW D7 D6 D5 D4 D3 D2 D1 D0
//      0   0  0  0  1  1  N  F  x  x
//Display Off
//      RS RW D7 D6 D5 D4 D3 D2 D1 D0
//      0   0  0  0  0  0  1  0  0  0
//Display Clear
//      RS RW D7 D6 D5 D4 D3 D2 D1 D0
//      0   0  0  0  0  0  0  0  0  1
//Entry mode set
//      RS RW D7 D6 D5 D4 D3 D2  D1  D0
//      0   0  0  0  0  0  0  1  I/D  S

int pos = 0;

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


int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;   // stop watchdog timer

    //Timing variable

    //Init pins
    P3DIR |= (BIT4 | BIT5 | BIT6 | BIT7);
    P4DIR |= (BIT0 | BIT1 | BIT2 | BIT3);
    P5DIR |= (BIT0 | BIT1 | BIT5);

    init();

    while(1){
        move_cursor(6);
        write_string("Todo");
        move_cursor(44);
        write_string("Funciona");
        __delay_cycles(1000000);
        init(); //Use init to clear screen
    }

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

