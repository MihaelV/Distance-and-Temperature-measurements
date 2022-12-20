/*
//////////////////////////
		LED
//////////////////////////
	all LEDs have resistors

	PLAYER 1
	PD1 = LED0
	PD3 = LED1
	PD4 = LED2
	PD6 = LED3
	PD7 = LED4
	PC0 = LED5
	PC1 = LED6
	PC6 = LED7
	PC7 = LED8
	
	PLAYER 2
	PA0-PA7 = LED0-LED7
	
//////////////////////////
		BUTTONS
//////////////////////////

	INTERRUPT 1 = BUTTON1
	INTERRUPT 2 = BUTTON2
*/


#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>


/*
//////////////////////////
		VARIABLES
//////////////////////////
*/

int counterPlayer1 = 0;
int counterPlayer2 = 0;


/*
//////////////////////////
		PROGRAM
//////////////////////////
*/

int main(void)
{
	sei();
	
	DDRA = 0b11111111;
	DDRB = 0b00000000;
	DDRC = 0b11000011;
	DDRD = 0b11011010;
	
	// INT1 button
	PORTD |= 1<<PIND3;
	EICRA |=1<<ISC11;
	EICRA &= ~(1<<ISC10);
	EIFR |= 1<<INTF1;
	EIMSK |= 1<<INT1;
	
	// INT2 button
	PORTB |= 1<<PINB2;
	EICRA |= 1<<ISC21;
	EICRA &= ~(1<<ISC20);
	EIFR |= 1<<INTF2;
	EIMSK |= 1<<INT2;
	
	
	
	turnOofAllLedPlayer1();
	turnOofAllLedPlayer2();
	
	
    while(1)
    {
		
    }
}


void turnOnAllLedPlayer1(){
	PORTD |= 1<<PIND1;
	PORTD |= 1<<PIND4;
	PORTD |= 1<<PIND6;
	PORTD |= 1<<PIND7;
	PORTC |= 1<<PINC0;
	PORTC |= 1<<PINC1;
	PORTC |= 1<<PINC6;
	PORTC |= 1<<PINC7;
}

void turnOofAllLedPlayer1(){
	PORTD &= ~(1<<PIND1);
	PORTD &= ~(1<<PIND4);
	PORTD &= ~(1<<PIND6);
	PORTD &= ~(1<<PIND7);
	PORTC &= ~(1<<PINC0);
	PORTC &= ~(1<<PINC1);
	PORTC &= ~(1<<PINC6);
	PORTC &= ~(1<<PINC7);
}

void turnOnAllLedPlayer2(){
	PORTA = 0b11111111;
}

void turnOofAllLedPlayer2(){
	PORTA = 0b00000000;
}


void blinkingPlayer1(){
	turnOofAllLedPlayer1();
	_delay_ms(200);
	turnOnAllLedPlayer1();
	_delay_ms(200);
	turnOofAllLedPlayer1();
	_delay_ms(200);
	turnOnAllLedPlayer1();
	_delay_ms(200);
	turnOofAllLedPlayer1();
	_delay_ms(200);
	turnOnAllLedPlayer1();
	_delay_ms(200);
	turnOofAllLedPlayer1();
	_delay_ms(200);
	turnOnAllLedPlayer1();
	_delay_ms(200);
	turnOofAllLedPlayer1();
}


void blinkingPlayer2(){
	turnOofAllLedPlayer2();
	_delay_ms(200);
	turnOnAllLedPlayer2();
	_delay_ms(200);
	turnOofAllLedPlayer2();
	_delay_ms(200);
	turnOnAllLedPlayer2();
	_delay_ms(200);
	turnOofAllLedPlayer2();
	_delay_ms(200);
	turnOnAllLedPlayer2();
	_delay_ms(200);
	turnOofAllLedPlayer2();
	_delay_ms(200);
	turnOnAllLedPlayer2();
	_delay_ms(200);
	turnOofAllLedPlayer2();
}




// Player 1 button
ISR(INT1_vect){
	counterPlayer1++;
	
	if (counterPlayer1 == 1)
	{
		PORTD |= 1<<PIND1;
	}else if (counterPlayer1 == 2)
	{
		PORTD |= 1<<PIND4;
	}else if (counterPlayer1 == 3)
	{
		PORTD |= 1<<PIND6;
	}else if (counterPlayer1 == 4)
	{
		PORTD |= 1<<PIND7;
	}else if (counterPlayer1 == 5)
	{
		PORTC |= 1<<PINC0;
	}else if (counterPlayer1 == 6)
	{
		PORTC |= 1<<PINC1;
	}else if (counterPlayer1 == 7)
	{
		PORTC |= 1<<PINC6;
	}else if (counterPlayer1 == 8)
	{
		PORTC |= 1<<PINC7;
		_delay_ms(300);
		
		blinkingPlayer1();
		
		counterPlayer1 = 0;
		counterPlayer2 = 0;
		
		turnOofAllLedPlayer1();
		turnOofAllLedPlayer2();
		
		_delay_ms(300);
	}
	
	_delay_ms(10);
	EIFR |= 1<<INTF2;
	_delay_ms(5);
	EIFR |= 1<<INTF1;
}


// Player 2 button
ISR(INT2_vect){
	counterPlayer2++;
	
	if (counterPlayer2 == 1)
	{
		PORTA |= 1<<PINA0;
	}else if (counterPlayer2 == 2)
	{
		PORTA |= 1<<PINA1;
	}else if (counterPlayer2 == 3)
	{
		PORTA |= 1<<PINA2;
	}else if (counterPlayer2 == 4)
	{
		PORTA |= 1<<PINA3;
	}else if (counterPlayer2 == 5)
	{
		PORTA |= 1<<PINA4;
	}else if (counterPlayer2 == 6)
	{
		PORTA |= 1<<PINA5;
	}else if (counterPlayer2 == 7)
	{
		PORTA |= 1<<PINA6;
	}else if (counterPlayer2 == 8)
	{
		PORTA |= 1<<PINA7;
		_delay_ms(300);
		
		blinkingPlayer2();
		
		counterPlayer1 = 0;
		counterPlayer2 = 0;
		
		turnOofAllLedPlayer1();
		turnOofAllLedPlayer2();
			
		_delay_ms(300);
	}
	
	_delay_ms(10);
	EIFR |= 1<<INTF1;
	_delay_ms(5);
	EIFR |= 1<<INTF2;
}




