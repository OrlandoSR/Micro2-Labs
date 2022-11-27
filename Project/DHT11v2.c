/*
 * Timers_C.c
 *
 * Created: 11/23/2022 4:26:38 PM
 * Author : orlan
 */ 

//PB7 is built in LED
//Analog input 0 (ADC0) is PF0 in alternate function



//Baud Rate calculations for UART communication
#define F_CPU 16000000
#define BAUD 9600
#define BRC ((F_CPU/16/BAUD) - 1)

//LCD Command Codes
#define CLEAR_CMD 0x01
#define HOME_CMD 0x02
#define ENTRY_CMD 0x06
#define ON_CMD 0x0F
#define FUNCTION_SET 0x38


#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdint-gcc.h>


volatile long int overflow = 0;

//----------------ADC----------------------------//
void setupADC();
void startConversion();
int ADC_value = 0;

//----------------DHT11----------------------------//
void setupDHT11();
void DHT11_Data_loop();
void start_signal();
void responce_signal();
uint8_t read_DHT11();
uint8_t RH_I,RH_D,temp_I,temp_D,checksum;

//----------------LCD----------------------------//
unsigned char str[4];
char dec[16];
int pos = 0;
volatile unsigned long int j, k, r = 0;
volatile unsigned int refresh = 0;
volatile unsigned long int i = 0;
volatile int direction = 0;

void LCD_cmd(char cmd);
void send_enable();
void init_LCD();
void toString(char [], int);
void display_string(int row);
void function_clear();
void display_off();
void display_clear();
void entry_mode();
void function_set_cmd();
void display_on();
void move_cursor(volatile unsigned long int target);
void write_char (unsigned char data);
void write_string(char *s);
//---------------------------------------------//

int main(void)
{
//-----------------------PORTS setup-----------------------//
	DDRB = (1 << PB7);			//PB7 as output (built-in LED)
	PORTB = (0 << PB7);			//PB7 starts off
	
	
	
//-----------------------LCD setup-----------------------//
	DDRA = (1 << PA0) | (1 << PA1) | (1 << PA2) | (1 << PA3) | (1 << PA4) | (1 << PA5) | (1 << PA6) | (1 << PA7);	//Entire port for sending 8bit data
	DDRC = (1 << PC0) | (1 << PC1);			//PC0 is enable, PC1 is register select (RS), R/w is always low
	
		
//-----------------------UART setup-----------------------//
	UBRR0H = (BRC >> 8);
	UBRR0L = BRC;
	
	UCSR0B = (1 << TXEN0);	//Transmit pin enable
	UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);	//8-bit mode
	
	
//-----------------------DHT11 setup----------------------//
	/*DDRB = 0xFF;*/
	
//---------------------Timer Init-------------------------//
	TCCR0A = (1 << WGM01);		//Set  in CTC mode
	OCR0A = 195;				//Compare register value (0.01s)
	TIMSK0 = (1 << OCIE0A);		//Local interrupt enable 
	
	setupADC();
	
	//sei();						//Set external interrupt
	
	TCCR0B = (1 << CS02) | (1 << CS00); //Start at 1024 prescaler
	
	init_LCD();
	
	//move_cursor(6);
	//write_string("Todo");
	//move_cursor(45);
	//write_string("Funciona");
	
	DHT11_Data_loop();
	
    while (1) 
    {
		//_delay_ms(1000);
		//display_clear();
// 		toString(dec, ADC_value);
// 		write_string(dec);

    }
	return 0;
}

ISR(TIMER0_COMPA_vect){
	overflow++;
	
	if(overflow > 100){
		PORTB ^= (1 << PB7);			//Toggle built-in LED
		overflow = 0;		
	}
}

ISR(ADC_vect){
		ADC_value = ADC;						//Send char  to the terminal
		startConversion();
}

//--------------------------ADC setup----------------------------------//
void setupADC(){
	ADMUX = (1 << REFS0);						//AVcc as reference, A0 as input (PF0)
	ADCSRA = (1 << ADEN) | (1 << ADIE) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0); //ADC Enabled with local interrupt enable and 1024 pre-scaler
	DIDR0 = (1 << ADC5D);
	
	startConversion();
}

void startConversion(){
	ADCSRA |= (1 << ADSC);
}
//--------------------------DHT11 setup-------------------------------//


void DHT11_Data_loop(){
	_delay_ms(2000);   //Read DHT11 every 2s
	start_signal();    //Send start signal from sensor
	responce_signal(); //Recieve responce signal from sensor
	
	RH_I = read_DHT11();  //Read humidity (int)
	RH_D = read_DHT11();  //Read humidity (fraction)
	temp_I = read_DHT11();  //Read humidity (int)
	temp_D = read_DHT11();  //Read humidity (fraction)
	checksum = read_DHT11(); //Read check sum
	
	if((RH_I + RH_D + temp_I + temp_D) == checksum){
		
		move_cursor(0);
		toString(dec,((int) RH_I));
		write_string(dec);
		
		move_cursor(40);
		toString(dec,((int) temp_I));
		write_string(dec);
	/*	write_string();*/
	}
}

void start_signal(){
	DDRL |= (1<<PL0);    //PL0 as DHT11 data line
	PORTL &= ~(1<<PL0);  //send low pulse
	_delay_ms(30);
	PORTL |= (1<<PL0);  //send high pulse
}

void responce_signal(){
	DDRL &=~ (1 << PINL0);		 //Set as input
	_delay_us(40);
	write_string("PINL = ");
	write_char((PINL + '0'));
	while(PINL & (1<<PL0));	     //low pulse
	display_clear();
	write_string("LOW PULSE");
	while((PINL & (1<<PL0))==0); //high pulse
	while(PINL & (1<<PL0));		 //low pulse
}

uint8_t read_DHT11(){ //Should be uint8_t
	uint8_t dataByte = 0;
	for(k=0;k<8; k++){
		while((PINB & (1<<PL0))==0); //high pulse
		_delay_us(50);
		if(PINB & (1<<PL0)){ //Check Pulse to Store Value (1 or 0)
			dataByte = (dataByte << 1) | (0x01);
		}
		else{
			dataByte = (dataByte << 1);
		}
		while(PINB & (1<<PL0)); //low pulse	
		
	}
	return dataByte;
}

//--------------------------LCD Functions-----------------------------//

void LCD_cmd(char cmd){
	PORTC &=~ (1 << PC1);
	PORTA = cmd;
	send_enable();
}

void init_LCD(){
	_delay_ms(15);
	LCD_cmd(FUNCTION_SET);
	_delay_us(410);
	LCD_cmd(FUNCTION_SET);
	_delay_us(100);
	LCD_cmd(FUNCTION_SET);
	LCD_cmd(FUNCTION_SET);
	LCD_cmd(ON_CMD);
	LCD_cmd(CLEAR_CMD);
	LCD_cmd(ENTRY_CMD);
}

//Decimal array population
void word_to_string(volatile unsigned long int number){
	for(r = 0; r < 4; r++){
		toString(dec,(number));
	}
}

//Change Value tostring
void toString(char str[], int num)
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
	//function_clear();
	//PORTA |= (1 << PA0);
	//send_enable();
	LCD_cmd(CLEAR_CMD);
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
