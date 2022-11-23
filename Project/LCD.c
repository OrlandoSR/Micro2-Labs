/*
 *
 * Created: 11/23/2022 12:24:25 PM
 * Author : Justin
 *
 */ 

#include <avr/io.h>
#include <stdio.h>
#include <stdint.h>
#include <math.h>
#ifndef F_CPU
#define F_CPU 16000000UL // 16 MHz clock speed
#endif
#include <util/delay.h>

//Analog input P7.6

volatile unsigned long int ADC_value = 0;
volatile unsigned long int j, k, r = 0;

void tostring(char [], int);

//----------------LCD----------------------------//
unsigned char str[4];
char dec[16];
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
void move_cursor(volatile unsigned long int target);
void write_char (unsigned char data);
void write_string(char *s);
//---------------------------------------------//

int main(void)
{
	
	//-------Init LCD pins-------------------//
	DDRA |= 0xFF;  //D0-D7 //
	DDRC |= ((1 << PC0) | (1 << PC1));              //Enable | RS | RW (ground)    
	//---------------------------------------//
	
	init_LCD();

    while(1){
		
		move_cursor(6);
		write_string("Todo");
		move_cursor(45);
		write_string("Funciona");

 		init_LCD();

	}
}


//Decimal array population
void word_to_string(volatile unsigned long int number){
    for(r = 0; r < 4; r++){
      tostring(dec,(number));
    }
}

//Change Value tostring
void tostring(char str[], int num)
{
    int i, rem, len = 0, n;

    n = num;
    while (n != 0)
    {
        len++;
        n /= 10;
    }
    for (i = 0; i < len; i++)
    {
        rem = num % 10;
        num = num / 10;
        str[len - (i + 1)] = rem + '0';
    }
    str[len] = '\0';
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

	PORTC |= (1 << PC1); //RS

    if(data == 0){
      data = '0';
    }
	
	//HMM
	PORTA = data;
	
    send_enable();

    pos++;
}

void move_cursor(volatile unsigned long int target){
    function_clear();

	PORTA |= (1 << PA4); 
	
    while(target != pos){
        if(target > pos){
			PORTA |= (1 << PA2);
            send_enable();
            pos++;

        }else if(target < pos){
			PORTA = (0 << PA2);
            send_enable();
            pos--;
        }
    }
}

void function_clear(){
	PORTA &=~ 0xFF;
	PORTC &=~ 0xFF;
}

void send_enable(){
	
	PORTC |= (1 << PC0); //Enable
	_delay_ms(1);
	PORTC &=~ (1 << PC0);
}

void display_off(){
    function_clear();
	PORTA |= (1 << PA3);
    send_enable();
}

void display_clear(){
    function_clear();
	PORTA |= (1 << PA0);
    send_enable();
    pos = 0;
}

void entry_mode(){
    function_clear();
	PORTA |= ((1 << PA1) | (1 << PA2));
    send_enable();
}

void function_set_cmd(){
    function_clear();
	PORTA |= ((1 << PA3) | (1 << PA4) | (1 << PA5));
    send_enable();
}

void display_on(){
    function_clear();
	PORTA |= ((1 << PA0) | (1 << PA1) | (1 << PA2) | (1 << PA3));
    send_enable();
}

void init_LCD(){
	PORTC &=~ (1 << PC1); //RS
    _delay_ms(100);
    function_set_cmd();
     _delay_ms(100);
    function_set_cmd();
     _delay_ms(100);
    function_set_cmd();
    _delay_ms(100);
    function_set_cmd();
     _delay_ms(100);
    display_off();
     _delay_ms(100);
    display_clear();
     _delay_ms(100);
    entry_mode();
     _delay_ms(100);
    display_on();
     _delay_ms(100);
}

