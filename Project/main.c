/*
 * Timers_C.c
 *
 * Created: 11/23/2022 4:26:38 PM
 * Author : Justin y Orlando
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
#include <string.h>


volatile long int overflow = 0;

int alarm = 0;
int alarmState = 0;

//----------------IR--------------------------------//
volatile int currS1 = 0;
volatile int currS2 = 0;
volatile int prevS1 = 0;
volatile int prevS2 = 0;
volatile int crowd = 0;
void readIR();
void readCrowd();


//----------------ADC-------------------------------//
void setupADC();
void startConversion();
int ADC_value = 0;
void readADC(int sensor);

//----------------DHT11-----------------------------//
void setupDHT11();
void DHT11_Data_loop();
void start_signal();
void responce_signal();
uint8_t read_DHT11();
uint8_t RH_I,RH_D,temp_I,temp_D,checksum;
char str3[16];

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

	
int state;

int main(void)
{
//-----------------------PORTS setup-----------------------//
	DDRB = (1 << PB7);			//PB7 as output (built-in LED)
	PORTB = (0 << PB7);			//PB7 starts off
	
	//Button
	DDRH &=~ (1 << PH0);
	
	//IR
	DDRE &=~ (1 << PE4) | (1 << PE5);
	
	//Buzzer
	DDRH |= (1 << PH6);			//OC2B
	
	
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
	
//---------------------Timer0 Init-------------------------//
	TCCR0A = (1 << WGM01);		//Set  in CTC mode
	OCR0A = 156;				//Compare register value (0.01s)
	TIMSK0 = (1 << OCIE0A);		//Local interrupt enable 
	
	setupADC();
	
	sei();						//Set external interrupt
	
	TCCR0B = (1 << CS02) | (0 << CS00); //Start at 256 prescaler
	
	
	init_LCD();
	
    while (1) 
    {

		if(PINH & (1 << PINH0)){state++;}
		if(state > 6){state = 0;}

			
		readIR();
		
		//state = 6;					//Set fix state
		
		switch (state)
		{
			case 0:
				DHT11_Data_loop(); //DHT11 Data
				break;
			case 1:
				readADC(0);
				break;
			case 2:
				readADC(1);
				break;
			case 3:
				readADC(2);
				break;
			case 4:
				readADC(3);
				break;
			case 5:
				readADC(4);
				break;
			case 6:
				readCrowd();
			break;
			default:
				//_delay_ms(1000);
				//display_clear();
				//write_string("Default State Reached");
				break;
		}
    }
}

ISR(TIMER0_COMPA_vect){
	overflow++;
	if(alarm){
		PORTH ^= (1 << PH6);
	}
	
	if(overflow > 400){
		
		PORTB ^= (1 << PB7);			//Toggle built-in LED
		if(!alarm){
			state++;
		}else{
			state = alarmState;
		}
		overflow = 0;		
	}
}


ISR(ADC_vect){
		ADC_value = ADC;						//Send char  to the terminal
		startConversion();
}

//--------------------------ADC setup----------------------------------//
void setupADC(){
	ADMUX |= (1 << REFS0);						//AVcc as reference, A0 as input (PF0)
	ADCSRA = (1 << ADEN) | (1 << ADIE) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0); //ADC Enabled with local interrupt enable and 1024 pre-scaler
	DIDR0 = (1 << ADC5D);
	
	startConversion();
}

void startConversion(){
	ADCSRA |= (1 << ADSC);
}

void readADC(int sensor){
	
	ADCSRA &=~ (1 << ADEN);				//Disable ADC
	
	int sState = (PORTB & (1 << PB7));
	
	while(sState == (PORTB & (1 << PB7)));
	
	switch(sensor){
		case 0:
			ADMUX = (1 << REFS0);				//Read A0
			DIDR0 = (1 << ADC0D);				//Disable digital buffer for A0 (optional)
			setupADC();
			_delay_ms(2);
		
			display_clear();
			_delay_ms(1);
			write_string("Light level is:");
			move_cursor(40);
			//toString(dec, ADC);
			//write_string(dec);
			if(ADC > 200){
				write_string("Very Low");
			}else if(ADC < 150){
				write_string("Very High");
				alarm = 1;
				alarmState = 1;
			}else{
				write_string("Normal");
				alarm = 0;
			}

			break;
		
		case 1:
			ADMUX = (1 << REFS0) | (1 << MUX0);	//Read A1
			DIDR0 = (1 << ADC1D);				//Disable digital buffer for A1 (optional)
			setupADC();
			_delay_ms(2);
		
			display_clear();
			_delay_ms(1);
			write_string("Sound level is:");
			move_cursor(40);
			//toString(dec, ADC);
			//write_string(dec);
			if(ADC < 200){
				write_string("Very High");
				alarm = 1;
				}else if(ADC > 1000){
				write_string("Very Low");
				}else{
				write_string("Normal");
				alarm = 0;
				
			}
			break;
		
		case 2:
			ADMUX = (1 << REFS0) | (1 << MUX1);	//Read A2
			DIDR0 = (1 << ADC2D);				//Disable digital buffer for A2 (optional)
			setupADC();
			_delay_ms(2);
				
			display_clear();
			_delay_ms(1);
			write_string("CO level is:");
			move_cursor(40);
			//toString(dec, ADC);
			//write_string(dec);
			if(ADC < 100){
				write_string("High");
				alarm = 1;
			}else{
				write_string("Normal");
				alarm = 0;
			}
			break;

		case 3:
			ADMUX = (1 << REFS0) | (1 << MUX0) | (1 << MUX1);	//Read A3
			DIDR0 = (1 << ADC3D);				//Disable digital buffer for A3 (optional)
			setupADC();
			_delay_ms(2);
		
			display_clear();
			_delay_ms(1);
			write_string("Smoke level is:");
			move_cursor(40);
			//toString(dec, ADC);
			//write_string(dec);
			if(ADC < 80){
				write_string("High");
				alarm = 1;
			}else{
				write_string("Normal");
				alarm = 0;
			}
			break;
			
		case 4:
			ADMUX = (1 << REFS0) | (1 << MUX0) | (1 << MUX1);	//Read A3
			DIDR0 = (1 << ADC4D);				//Disable digital buffer for A4 (optional)
			setupADC();
			_delay_ms(2);
		
			display_clear();
			_delay_ms(1);
			write_string("Gas level is:");
			move_cursor(40);
			//toString(dec, ADC);
			//write_string(dec);
			if(ADC < 80){
				write_string("High");
				alarm = 1;
				}else{
				write_string("Normal");
				alarm = 0;
			}
			break;	
			
		default:
			write_string("Default ADC case");
			alarm = 0;
			break;
	}	

}

//--------------------------IR functions----------------------------------//
void readIR(){
	currS1 = (PINE & (1 << PINE5));
	currS2 = (PINE & (1 << PINE4));
	
	if(currS1 != 0 && prevS1 != currS1){
		crowd++;
	}else if(currS2 != 0 && prevS2 != currS2){
		crowd--;
	}
	
	prevS1 = currS1;
	prevS2 = currS2;
}


void readCrowd(){
	int sState = (PORTB & (1 << PB7));
	
	while(sState == (PORTB & (1 << PB7)));
	
	display_clear();
	_delay_ms(1);
	write_string("Crowd is:");
	move_cursor(40);
	toString(dec, crowd);
	if(crowd == 0){
		write_string("None");
	}else if(crowd < 0){
		crowd = 0;
	}else{
		write_string(dec);
	}
}


//--------------------------DHT11 setup-------------------------------//


void DHT11_Data_loop(){
	int sState = (PORTB & (1 << PB7));
	
	while(sState == (PORTB & (1 << PB7)));
	
	start_signal();    //Send start signal from sensor
	responce_signal(); //Recieve responce signal from sensors
	
	RH_I = read_DHT11();  //Read humidity (int)
	RH_D = read_DHT11();  //Read humidity (fraction)
	temp_I = read_DHT11();  //Read humidity (int)
	temp_D = read_DHT11();  //Read humidity (fraction)
	checksum = read_DHT11(); //Read check sum
	
	if((RH_I + RH_D + temp_I + temp_D) == checksum){
		
		display_clear();
		_delay_ms(1);
		write_string("Humidity: ");
		
		toString(dec,RH_I);
		write_string(dec);
		
		toString(dec,RH_D);
		write_string(dec);
		write_string("%");
		
		move_cursor(40);
		write_string("Temperature: ");
		
		toString(dec,temp_I);
		write_string(dec);
		write_string("C");

	}
}

void start_signal(){
		//Send start signal
		DDRL |= (1<<PL0);
		PORTL &= ~(1<<PL0);	/* set to low pin */
		_delay_ms(20);		/* wait for 20ms */
		PORTL |= (1<<PL0);	/* set to high pin */
		DDRL &=~ (1<<PL0);	//Set as input
}

void responce_signal(){
		while(PINL & (1<<PINL0));
		while((PINL & (1<<PINL0))==0);
		while(PINL & (1<<PINL0));
}

uint8_t read_DHT11(){ //Should be uint8_t
	uint8_t dataByte = 0;
	for(k=0;k<8; k++){
		while((PINL & (1<<PL0))==0); //high pulse
		_delay_us(50);
		if(PINL & (1<<PL0)){ //Check Pulse to Store Value (1 or 0)
			dataByte = (dataByte << 1) | (0x01);
		}
		else{
			dataByte = (dataByte << 1);
		}
		while(PINL & (1<<PL0)); //low pulse
		
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
	_delay_us(800);
	PORTC &=~ (1 << PC0);
}

void display_off(){
	function_clear();
	PORTA |= (1 << PA3);
	send_enable();
}

void display_clear(){
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
