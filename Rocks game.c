/*
	Atmega32
////////////////////
	LCD 2004
////////////////////
PA0-PA7 = D0-D7
RS = PB0
RW = PB1
E = PB3

////////////////////
 Buttons-intterrupt
////////////////////
Button1 = INT0

////////////////////
	  BUZZER
////////////////////
PD7 = IO

*/


/*
////////////////////
		MODE
////////////////////
mode 0 = start
mode 1 = game

*/

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdlib.h>

/*
	DEFINE
*/

#define LCD_D 0b01000100
#define LCD_I 0b01001001
#define LCD_S 0b01010011
#define LCD_T 0b01010100
#define LCD_A 0b01000001
#define LCD_N 0b01001110
#define LCD_C 0b01000011
#define LCD_E 0b01000101
#define LCD_T 0b01010100
#define LCD_M 0b01001101
#define LCD_P 0b01010000
#define LCD_R 0b01010010
#define LCD_U 0b01010101
#define LCD_H 0b01001000
#define LCD_L 0b01001100
#define LCD_O 0b01001111
#define LCD_W 0b01010111
#define LCD_G 0b01000111
#define LCD_V 0b01010110
#define LCD_SPACE 0b11111110
#define LCD_LEFT_ARROW 0b01111111
#define LCD_DOUBLE_POINT 0b00111010
#define LCD_B 0b01000010
#define LCD_K 0b01001011
#define LCD_RIGHT_ARROW 0b01111110
#define LCD_NULL 0b00110000
#define LCD_ONE 0b00110001
#define LCD_TWO 0b00110010
#define LCD_THREE 0b00110011
#define LCD_FOUR 0b00110100
#define LCD_FIVE 0b00110101
#define LCD_SIX 0b00110110
#define LCD_SEVEN 0b00110111
#define LCD_EIGHT 0b00111000
#define LCD_NINE 0b00111001
#define LCD_m 0b01101101
#define LCD_celsius_sign 0b11011111
#define LCD_POINT 0b00101110
#define LCD_ROCK 0b11111111
#define LCD_PLAYER 0b11111100
#define LCD_x 0b01111000

/*
	VARIABLES
*/

int counter;

int BufferCounter;
char reciveMessage;
char buffer[10];

char mode = 0;

char playerUp = 1;

int pathWayUp[100];
int pathWayDown[100];
int savePositionOfRockValue;
int randomNumber1;
int randomNumber2;

char isGameOver = 0;


/*
	MESSAGES
*/

char GameOverText[10] = {LCD_G,LCD_A,LCD_M,LCD_E,LCD_SPACE,LCD_O,LCD_V,LCD_E,LCD_R,0};
char StartGame[11] = {LCD_S,LCD_T,LCD_A,LCD_R,LCD_T,LCD_SPACE,LCD_G,LCD_A,LCD_M,LCD_E,0};


/*
	PROGRAM
*/

int main(void)
{
	sei();
	
	DDRA = 0b11111111;
	DDRB = 0b00000111;
	DDRC = 0b00000000;
	DDRD = 0b10000000;
	
	
	// INT0
	PORTD |= 1<<PIND2;
	MCUCR |= 1<<ISC01;
	MCUCR &= ~(1<<ISC00);
	GIFR |= 1<<INTF0;
	GICR |= 1<<INT0;
	
	_delay_ms(1000);
	
	displayOnCursorOffBlinkOff();
	ClearDisplay();
	
	
	
	GameStart();
	
    while(1)
    {
		
		_delay_ms(10);
		if (mode == 1)
		{
			newClearDisplay();
			fillThePathWay();
			isGameOver = 0;
			
			
			setAddress(2);
			playerUp = 1;
			writeData(LCD_PLAYER);
			while(1)
			{
				
				MoveAllUpRocksByOne();
				if (mode != 1){break;}
				_delay_ms(3000);
			}
		}
		
		
	}
}

/*
////////////////////
   MODES FUNCTIONS
////////////////////
*/

void gameOver(){
	newClearDisplay();
	setAddress(5);
	writeString(GameOverText);
	_delay_ms(11000);
	GameStart();
}

void GameStart(){
	mode = 0;
	newClearDisplay();
	setAddress(5);
	writeString(StartGame);
	writeData(LCD_SPACE);
	writeData(LCD_LEFT_ARROW);
	GICR |= 1<<INT0;
}


/*
////////////////////
  GAME FUNCTIONS
////////////////////
*/

void fillThePathWay(){
	savePositionOfRockValue = 20;
	for (int i=0;i<100;i++)
	{
		randomNumber1 = (rand() % 10);
		randomNumber2 = (rand() % 10);
		while (randomNumber1 < 2){randomNumber1 = (rand() % 10);}
		while (randomNumber2 < 2){randomNumber2 = (rand() % 10);}
		savePositionOfRockValue += randomNumber1;
		
		pathWayUp[i] =  savePositionOfRockValue;
		pathWayDown[i] = savePositionOfRockValue + 20 + randomNumber2;
		savePositionOfRockValue += randomNumber2;
	}
	savePositionOfRockValue = 0;
}

void MoveAllUpRocksByOne(){
	for (int i=0;i<100;i++)
	{
		pathWayUp[i] = pathWayUp[i] - 1;
		UpRockMove(pathWayUp[i]);
		if (isGameOver){break;}
		pathWayDown[i] = pathWayDown[i] - 1;
		DownRockMove(pathWayDown[i]);
		if (isGameOver){break;}
	}
}



void UpRockMove(int RockPosition){
	
	if (RockPosition == 2 && playerUp == 1)
	{
			GICR &= ~(1<<INT0);
			setAddress(3);
			writeData(LCD_SPACE);
			xSignBlink(2);
			isGameOver = 1;
			mode = 0;
			gameOver();
	}else{
		if (RockPosition <= 19 && RockPosition >= -1)
		{
			if (RockPosition == -1)
			{
				setAddress(RockPosition+1);
				writeData(LCD_SPACE);
			}else{
				setAddress(RockPosition);
				writeData(LCD_ROCK);
		
			if (RockPosition != 19)
			{
				setAddress(RockPosition+1);
				writeData(LCD_SPACE);
			}
			}
		
		}	
	}
	
}

void DownRockMove(int RockPosition){
	
	if (RockPosition == 22 && playerUp == 0)
	{
		GICR &= ~(1<<INT0);
		setAddress(23);
		writeData(LCD_SPACE);
		xSignBlink(22);
		isGameOver = 1;
		mode = 0;
		gameOver();
	}
	else{
		if (RockPosition <= 39 && RockPosition >= 19)
		{
			if (RockPosition == 19)
			{
				setAddress(RockPosition+1);
				writeData(LCD_SPACE);
			}else{
				setAddress(RockPosition);
				writeData(LCD_ROCK);
		
				if (RockPosition != 39)
				{
					setAddress(RockPosition+1);
					writeData(LCD_SPACE);
				}
			}
		
		}
	}
}

void xSignBlink(char xPosition){
	setAddress(xPosition);
	writeData(LCD_x);
	_delay_ms(4000);
	setAddress(xPosition);
	writeData(LCD_SPACE);
	_delay_ms(4000);
	setAddress(xPosition);
	writeData(LCD_x);
	_delay_ms(4000);
	setAddress(xPosition);
	writeData(LCD_SPACE);
	_delay_ms(4000);
	setAddress(xPosition);
	writeData(LCD_x);
	_delay_ms(4000);
}

/*
////////////////////
  LCD2004 Command
////////////////////
*/

void enable(){
	PORTB |= 1<<PINB2;
	_delay_ms(0.055);
	PORTB &= ~(1<<PINB2);
	_delay_ms(10);
}

void ClearDisplay(){
	PORTB &= ~(1<<PINB0);
	PORTB &= ~(1<<PINB1);
	PORTA = 0b00000001;
	enable();
	_delay_ms(400);
}

void newClearDisplay(){
	setAddress(0);
	counter = 0;
	for (counter=0;counter<40;counter++)
	{
		writeData(LCD_SPACE);
	}
	setAddress(0);
	counter = 0;
}

void returnHome(){
	PORTB &= ~(1<<PINB0);
	PORTB &= ~(1<<PINB1);
	PORTA = 0b00000010;
	enable();
	_delay_ms(300);
}

void writeData(char character){
	PORTB |= 1<<PINB0;
	PORTB &= ~(1<<PINB1);
	PORTA = character;
	enable();
}

void setAddress(char address){
	PORTB &= ~(1<<PINB0);
	PORTB &= ~(1<<PINB1);
	if (address > 127){address = 0;}
	PORTA = 0b10000000 + address;
	enable();
}

void displayCursorBlinkOn(){
	PORTB &= ~(1<<PINB0);
	PORTB &= ~(1<<PINB1);
	PORTA = 0b00001111;
	enable();
}


void displayOnCursorOffBlinkOff(){
	PORTB &= ~(1<<PINB0);
	PORTB &= ~(1<<PINB1);
	PORTA = 0b00001100;
	enable();
}

void writeString(char message[]){
	counter = 0;
	while (message[counter] != 0)
	{
		writeData(message[counter]);
		counter++;
	}
	counter = 0;
}

/*
////////////////////
   INTTERRUPTS
////////////////////
*/

ISR(INT0_vect){
	if (mode == 0)
	{
		mode = 1;
	}else if (mode == 1)
	{
		if (playerUp)
		{
			PORTD |= 1 << PIND7;
			setAddress(2);
			writeData(LCD_SPACE);
		
			setAddress(22);
			writeData(LCD_PLAYER);
			playerUp = 0;
		}else{
			PORTD |= 1 << PIND7;
			setAddress(22);
			writeData(LCD_SPACE);
		
			setAddress(2);
			writeData(LCD_PLAYER);
			playerUp = 1;
		}
		
	}
	
	_delay_ms(500);
	GIFR |= 1<<INTF0;
	PORTD &= ~(1 << PIND7);
}