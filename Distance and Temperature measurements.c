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
   Sensor SEN0153
////////////////////
tx=rx
rx=tx


////////////////////
 Buttons-intterrupt
////////////////////
Button1 = INT0
Button2 = INT1

*/


/*
////////////////////
		MODE
////////////////////

mode1 = Options(distance and temperature)
mode2 = Distance measurement
mode3 =  Temperature measurement

*/

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

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



/*
	VARIABLES
*/

int counter;
int BufferCounter;
char reciveMessage;
char buffer[10];

char arrow_up = 1;
char mode = 0;


char firstNumber;
char secondNumber;
char thirdNumber;

int temperatureValue;


/*
	MESSAGES
*/
// Sensor
char sensorDistanceMessage[6] = {0x55,0xAA,0x11,0x00,0x02,0x12};
char sensorTemperatureMessage[6] = {0x55,0xAA,0x11,0x00,0x03,0x13};

// LCD
char HelloText[6] = {LCD_H,LCD_E,LCD_L,LCD_L,LCD_O,0};
char WaitingText[8] = {LCD_W,LCD_A,LCD_I,LCD_T,LCD_I,LCD_N,LCD_G,0};
char Options_distance_text[9] = {LCD_D,LCD_I,LCD_S,LCD_T,LCD_A,LCD_N,LCD_C,LCD_E,0};
char Options_temperature_text[12] = {LCD_T,LCD_E,LCD_M,LCD_P,LCD_E,LCD_R,LCD_A,LCD_T,LCD_U,LCD_R,LCD_E,0};
char Back_text[5] = {LCD_B,LCD_A,LCD_C,LCD_K,0};
char Cm_text[3] = {LCD_C,LCD_m,0};
char Celsius_text[3] = {LCD_celsius_sign,LCD_C,0};


/*
	PROGRAM
*/

int main(void)
{
	sei();
	
	DDRA = 0b11111111;
	DDRB = 0b00000111;
	DDRC = 0b00000000;
	DDRD = 0b00000000;
	
	// UART
	UCSRB = 0b10011000;
	UCSRC = 0b10000110;
	UBRRL = 25;
	
	// INT0
	PORTD |= 1<<PIND2;
	MCUCR |= 1<<ISC01;
	MCUCR &= ~(1<<ISC00);
	GIFR |= 1<<INTF1;
	GICR |= 1<<INT0;
	
	// INT1
	PORTD |= 1<<PIND3;
	MCUCR |= 1<<ISC11;
	MCUCR &= ~(1<<ISC10);
	GIFR |= 1<<INTF1;
	GICR |= 1<<INT1;
	
	// 16 bit Timer/counter1
	TCCR1B = 0b00000011;
	TCNT1 = 0;
	OCR1AH = 0b10000000;
	OCR1AL = 0b10000000;
	TIMSK &= ~(1<<OCIE1A);
	
	_delay_ms(1000);
	
	displayOnCursorOffBlinkOff();
	ClearDisplay();
	
	ProgramStart();
	
	options_distance_temperature();
	
    while(1)
    {
		
    }
}

/*
////////////////////
   MODES FUNCTION
////////////////////
*/


void ProgramStart(){
	TIMSK &= ~(1<<OCIE1A); // turn Off Timer/Counter1
	newClearDisplay();
	setAddress(6);
	writeString(HelloText);
	_delay_ms(15000);
	newClearDisplay();
	setAddress(6);
	writeString(WaitingText);
	waitingPoints(3);
}

void options_distance_temperature(){
	TIMSK &= ~(1<<OCIE1A); // turn Off Timer/Counter1
	newClearDisplay();
	setAddress(0);
	writeString(Options_distance_text);
	setAddress(20);
	writeString(Options_temperature_text);
	setAddress(9);
	writeData(LCD_LEFT_ARROW);
	setAddress(0);
	mode = 1;
	arrow_up = 1;
}

void DistancMode(){
	newClearDisplay();
	setAddress(0);
	writeString(Options_distance_text);
	writeData(LCD_DOUBLE_POINT);
	setAddress(36);
	writeString(Back_text);
	setAddress(34);
	writeData(LCD_RIGHT_ARROW);
	setAddress(26);
	writeString(Cm_text);
	setAddress(0);
	mode = 2;
	TIMSK |= 1<<OCIE1A; // turn On Timer/Counter1
}

void TemperatureMode(){
	newClearDisplay();
	setAddress(0);
	writeString(Options_temperature_text);
	writeData(LCD_DOUBLE_POINT);
	setAddress(36);
	writeString(Back_text);
	setAddress(34);
	writeData(LCD_RIGHT_ARROW);
	setAddress(26);
	writeString(Celsius_text);
	setAddress(0);
	mode = 3;
	TIMSK |= 1<<OCIE1A; // turn On Timer/Counter1
}



/*
////////////////////
  OTHER FUNCTION
////////////////////
*/

int DistanceValue;

void TranslateSensorMessgeDistanceToDisplay(int highDistance,int lowDistance){
	highDistance = highDistance << 8;
	DistanceValue = highDistance + lowDistance;
	
	thirdNumber = DistanceValue % 10;
	DistanceValue /=10;
	secondNumber = DistanceValue % 10;
	DistanceValue /=10;
	firstNumber = DistanceValue % 10;
	DistanceValue /=10;
	
	PrintSensorMessageToDisplay(firstNumber);
	PrintSensorMessageToDisplay(secondNumber);
	PrintSensorMessageToDisplay(thirdNumber);
}

void TranslateSensorMessgeTemperatureToDisplay(int highTemp,int lowTemp){
	highTemp = highTemp << 8;
	
	temperatureValue = highTemp + lowTemp;
	
	thirdNumber = temperatureValue % 10;
	temperatureValue /=10;
	secondNumber = temperatureValue % 10;
	temperatureValue /=10;
	firstNumber = temperatureValue % 10;
	temperatureValue /=10;
	
	PrintSensorMessageToDisplay(firstNumber);
	PrintSensorMessageToDisplay(secondNumber);
	writeData(LCD_POINT);
	PrintSensorMessageToDisplay(thirdNumber);
	
}


void PrintSensorMessageToDisplay(char number){
	if (number == 0){writeData(LCD_NULL);}
	else if (number == 1){writeData(LCD_ONE);}
	else if (number == 2){writeData(LCD_TWO);}
	else if (number == 3){writeData(LCD_THREE);}
	else if (number == 4){writeData(LCD_FOUR);}
	else if (number == 5){writeData(LCD_FIVE);}
	else if (number == 6){writeData(LCD_SIX);}
	else if (number == 7){writeData(LCD_SEVEN);}
	else if (number == 8){writeData(LCD_EIGHT);}
	else if (number == 9){writeData(LCD_NINE);}
}

void waitingPoints(char numberOfRepetition){
	for (int i=0;i<numberOfRepetition;i++)
	{
		setAddress(28);
		_delay_ms(3000);
		writeData(LCD_POINT);
		_delay_ms(3000);
		writeData(LCD_POINT);
		_delay_ms(3000);
		writeData(LCD_POINT);
		_delay_ms(3000);
		setAddress(28);
		writeData(LCD_SPACE);
		writeData(LCD_SPACE);
		writeData(LCD_SPACE);
	}
	
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
	
	if (mode == 1)
	{
		if(arrow_up){
			setAddress(9);
			writeData(LCD_SPACE);
			setAddress(32);
			writeData(LCD_LEFT_ARROW);
			arrow_up = 0;
		}else
		{
			setAddress(32);
			writeData(LCD_SPACE);
			setAddress(9);
			writeData(LCD_LEFT_ARROW);
			arrow_up = 1;
		}
	}
	
	_delay_ms(2000);
	GIFR |= 1<<INTF0;
}

ISR(INT1_vect){
	if (mode == 1)
	{
		if (arrow_up){DistancMode();}
		else{TemperatureMode();}
	}
	else if (mode == 2){options_distance_temperature();}
	else if (mode == 3){options_distance_temperature();}
	
	_delay_ms(2000);
	GIFR |= 1<<INTF1;
}

ISR(USART_RXC_vect){
	while(!(UCSRA & (1<<RXC)));
	
	reciveMessage = UDR;
	buffer[BufferCounter] = reciveMessage;
	BufferCounter++;
	
	if (BufferCounter > 9){BufferCounter=0;}
}

ISR(TIMER1_COMPA_vect){
	if (mode == 2)
	{
		counter = 0;
		BufferCounter = 0;
		for (counter=0;counter<sizeof(sensorDistanceMessage);counter++)
		{
			while(!(UCSRA & (1<<UDRE)));
			UDR = sensorDistanceMessage[counter];
		}
		counter=0;
		setAddress(22);
		TranslateSensorMessgeDistanceToDisplay(buffer[5],buffer[6]);
		setAddress(0);
		TCNT1 = 0;
				
	}else if (mode == 3)
	{
		counter = 0;
		BufferCounter = 0;
		for (counter=0;counter<sizeof(sensorTemperatureMessage);counter++)
		{
			while(!(UCSRA & (1<<UDRE)));
			UDR = sensorTemperatureMessage[counter];
		}
		counter=0;
		setAddress(22);
		TranslateSensorMessgeTemperatureToDisplay(buffer[5],buffer[6]);
		setAddress(0);
		TCNT1 = 0;
	}
	
}



