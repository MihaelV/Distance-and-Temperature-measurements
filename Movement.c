#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>


#define LCD_M 0b01001101
#define LCD_O 0b01001111
#define LCD_V 0b01010110
#define LCD_E 0b01000101
#define LCD_N 0b01001110
#define LCD_T 0b01010100
#define LCD_D 0b01000100
#define LCD_C 0b01000011
#define LCD_SPACE 0b00100000

	
char message[9] = {LCD_M,LCD_O,LCD_V,LCD_E,LCD_M,LCD_E,LCD_N,LCD_T,0};


int FirstTimeSignal = 1;
int msgCounter;
int isTextOnLCD = 0;


int main(void)
{
	sei();
	
	// PORTS I/O
	DDRA = 0b11111111;
	DDRB = 0b00000111;
	DDRC = 0b00000000;
	DDRD = 0b10000000;
	
	
	// pull-up resistor
	PORTD |= 1 << PIND2;
	PORTD |= 1 << PIND3;
	// INT0(sensor) on falling edge
	MCUCR |= 1 << ISC01;
	// INT1(button) on rising edge
	MCUCR |= 1<< ISC11;
	MCUCR |= 1 << ISC10;
	// interrupt enable
	GICR |= 1 << INT0;
	GICR |= 1 << INT1;
	
	
	_delay_ms(1000);
	
	displayCursorBlinkOn();
	cursorReturn();
	
	
    while(1)
    {
		
    }
}	


ISR(INT0_vect){
	if(FirstTimeSignal){
		MCUCR |= 1 << ISC00;
		FirstTimeSignal = 0;
	}else{
		if (!isTextOnLCD)
		{
			printStringDelay(message);
		}
		
	}
}


ISR(INT1_vect){
	screenClear();
	isTextOnLCD = 0;
}


void printStringDelay(char msg[]){
	msgCounter = 0;
	
	while (1)
	{
		if (msg[msgCounter] == 0)
		{
			break;
		}
		
		DataWrite(msg[msgCounter]);
		_delay_ms(1200);
		msgCounter++;
	}
	isTextOnLCD = 1;
	displayCursorBlinkOff();
}


// COMMAND FOR DISPLAY
void enable(){
	PORTB |= 1<<PINB2;
	_delay_ms(0.045); // 500 nano sekundi za 8MHz
	PORTB &= ~(1<<PINB2);
	_delay_ms(0.2);
}

void displayCursorBlinkOn(){
	PORTB = 0b00000000;
	PORTA = 0b00001111;
	enable();
}

void displayCursorBlinkOff(){
	PORTB = 0b00000000;
	PORTA = 0b00001100;
	enable();
}

void moveCursorLeft(){
	PORTB = 0b00000000;
	PORTA = 0b00010000;
	enable();
}

void moveCursorRight(){
	PORTB = 0b00000000;
	PORTA = 0b00010100;
	enable();
}

void moveDisplayRight(){
	PORTB = 0b00000000;
	PORTA = 0b00011100;
	enable();
}

void moveDisplayLeft(){
	PORTB = 0b00000000;
	PORTA = 0b00011000;
	enable();
}

void cursorReturn(){
	PORTB = 0b00000000;
	PORTA = 0b00000010;
	enable();
}

void screenClear(){
	PORTB = 0b00000000;
	PORTA = 0b00000001;
	enable();
}

void DataWrite(int character){
	
	PORTB = 0b00000001;
	PORTA = character;
	enable();
}

void displayOff(){
	PORTB = 0b00000000;
	PORTA = 0b00001000;
	enable();
}

void displayOn(){
	PORTB = 0b00000000;
	PORTA = 0b00001100;
	enable();
}

void setCursorPosition(int position){
	PORTB = 0b00000000;
	PORTA = position;
	enable();
}
