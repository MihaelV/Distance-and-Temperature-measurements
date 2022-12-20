/*
////////////////////////////////
	LCD 12864 - Atmega32
////////////////////////////////
		D0 = PA0
		D1 = PA1
		D2 = PA2
		D3 = PA4
		D4 = PA5
		D5 = PA6
		D6 = PA7
		D7 = PD7

		RS = PB0
		RW = PB1
		E = PB2
		
////////////////////////////////
		  JOYSTIC
////////////////////////////////
		VRx = PA3
		+5V = 2 otpornika(1k,470)oma
		
////////////////////////////////
		   ADC
////////////////////////////////
		AREF = capacitor
		GND = GND
		
////////////////////////////////
	   INT0 BUTTON
////////////////////////////////
		PD2 = button

////////////////////////////////
	  UART USB CABLE
////////////////////////////////
		PD0 = TX
		PD1 = RX
*/

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#define LCD_R 0x52
#define LCD_A 0x41
#define LCD_N 0x4E
#define LCD_D 0x44
#define LCD_O 0x4F
#define LCD_M 0x4D
#define LCD_T 0x54
#define LCD_E 0x45
#define LCD_X 0x58
#define LCD_1 0x31
#define LCD_2 0x32
#define LCD_3 0x33
#define LCD_4 0x34
#define LCD_SPACE 0x20
#define	LCD_LEFT_ARROW 0x1B
#define	LCD_RIGHT_ARROW 0x1A




// VARIABLES
int counter = 0;
char buffer[100];
int counterBuffer = 0;
short int adcHighRegister;
short int adcLowRegister;

char hexNumber;
short int binaryNumberHalf[4];
short int binaryNumberFull[8];

int fieldAddress = 0x06;
int saveFiledAddress;

long int joysticDelay = 0;
int joysticAvailable = 0;
int row = 0;

int counterMessage;

int arrowDirection;

int randomTextChooser = 0;
int modeChooser = 0;
int optionsChooser = 1;

// MESSAGES
char randomText1[13] = {LCD_R,LCD_A,LCD_N,LCD_D,LCD_O,LCD_M,LCD_SPACE,LCD_T,LCD_E,LCD_X,LCD_T,LCD_1,0};
char randomText2[13] = {LCD_R,LCD_A,LCD_N,LCD_D,LCD_O,LCD_M,LCD_SPACE,LCD_T,LCD_E,LCD_X,LCD_T,LCD_2,0};
char randomText3[13] = {LCD_R,LCD_A,LCD_N,LCD_D,LCD_O,LCD_M,LCD_SPACE,LCD_T,LCD_E,LCD_X,LCD_T,LCD_3,0};
char randomText4[13] = {LCD_R,LCD_A,LCD_N,LCD_D,LCD_O,LCD_M,LCD_SPACE,LCD_T,LCD_E,LCD_X,LCD_T,LCD_4,0};

int main(void)
{
	sei();
	
	// PORTS I/O
	DDRA = 0b11111111;
	DDRB = 0b00000111;
	DDRC = 0b00000000;
	DDRD = 0b10000000;
	
	// INT0 button
	MCUCR |= 1 << ISC01;
	GICR |= 1 << INT0;
	PORTD |= 1 << PIND2;
	
	// UART
	UCSRB = 0b10011000;
	UCSRC = 0b10000110;
	UBRRL = 51;
	
	// ADC
	ADMUX = 0b11000011;
	ADCSRA = 0b11101111;
	
	_delay_ms(2000);
	
	
    while(1)
    {
		
		if (optionsChooser == 1){optionsPage();optionsChooser = 0;}
		if (randomTextChooser == 1){randomText1Function();}
		else if (randomTextChooser == 2){randomText2Function();}
		else if (randomTextChooser == 3){randomText3Function();}
		else if (randomTextChooser == 4){randomText4Function();}
    }
}

void randomText1Function(){
	newClear();
	while (randomTextChooser == 1)
	{
		counterMessage = 0;
		setDdramAddress(0x11);
		while (randomTextChooser == 1)
		{
			if (randomText1[counterMessage] == 0){break;}
			writeData(randomText1[counterMessage]);
			_delay_ms(700);
			counterMessage++;
		}
		if (randomTextChooser == 0){break;}
		_delay_ms(3000);
		if (randomTextChooser == 0){break;}
		newClear();
		_delay_ms(2500);
	}
	counterMessage = 0;
}

void randomText2Function(){
	newClear();
	saveFiledAddress = fieldAddress;
	fieldAddress = 0;
	setDdramAddress(0x00);
	while (randomTextChooser == 2)
	{
		setDdramAddress(fieldAddress);
		fieldAddress++;
		if (fieldAddress > 31){fieldAddress = 0;}
		printString(randomText2);
		_delay_ms(1500);
		newClearFast();
	}
	
	fieldAddress = saveFiledAddress;
}

void randomText3Function(){
	newClear();
	counterMessage = 0;
	saveFiledAddress = fieldAddress;
	fieldAddress = 0x10;
	setDdramAddress(fieldAddress);
	while (randomTextChooser == 3)
	{
		
		_delay_ms(1000);
	}
	
	fieldAddress = saveFiledAddress;
	counterMessage = 0;
}

void randomText4Function(){
	newClear();
	
	while (randomTextChooser == 4)
	{
		setDdramAddress(0x01);
		printStringDelay(randomText4);
		_delay_ms(3000);
		newClearRow0Delay();
		if (randomTextChooser != 4){break;}
		setDdramAddress(0x11);
		printStringDelay(randomText4);
		_delay_ms(3000);
		newClearRow1Delay();
		if (randomTextChooser != 4){break;}
		setDdramAddress(0x09);
		printStringDelay(randomText4);
		_delay_ms(3000);
		newClearRow2Delay();
		if (randomTextChooser != 4){break;}
		setDdramAddress(0x19);
		printStringDelay(randomText4);
		_delay_ms(3000);
		newClearRow3Delay();
	}
}

void optionsPage(){
	newClear();
	setDdramAddress(0x00);
	printString(randomText1);
	setDdramAddress(0x12);
	printString(randomText2);
	setDdramAddress(0x08);
	printString(randomText3);
	setDdramAddress(0x1A);
	printString(randomText4);
	
	setDdramAddress(fieldAddress);
	if (fieldAddress == 0x06)
	{
		writeData(LCD_LEFT_ARROW);
	}else if (fieldAddress == 0x19)
	{
		writeData(LCD_RIGHT_ARROW);
	}else if (fieldAddress == 0x11)
	{
		writeData(LCD_RIGHT_ARROW);
	}else if (fieldAddress == 0x0E)
	{
		writeData(LCD_LEFT_ARROW);
	}
	joysticAvailable = 1;
}

void enable(){
	PORTB |= 1 << PINB2;
	_delay_ms(0.045); // 500 nano second
	PORTB &= ~(1<<PINB2);
	_delay_ms(10); // 100 micro second
}

void displayCursorBlinkingOn(){
	PORTB &= ~(1<<PINB0) & ~(1<<PINB1);
	PORTA &= ~(1<<PINA7) & ~(1<<PINA6)  & ~(1<<PINA5);
	PORTA |= 1<<PINA0 | 1<<PINA1 | 1<<PINA2 | 1<<PINA4;
	PORTD &= ~(1<<PIND7);
	enable();
}

void displayOnCursorOffBlinkingOff(){
	PORTB &= ~(1<<PINB0) & ~(1<<PINB1);
	PORTA &= ~(1<<PINA7) & ~(1<<PINA6)  & ~(1<<PINA5) & ~(1<<PINA1) & ~(1<<PINA0);
	PORTA |= 1<<PINA2 | 1<<PINA4;
	PORTD &= ~(1<<PIND7);
	enable();
}

void home(){
	PORTB &= ~(1<<PINB0) & ~(1<<PINB1);
	PORTA &= ~(1<<PINA7) & ~(1<<PINA6)  & ~(1<<PINA5) & ~(1<<PINA3) & ~(1<<PINA2) & ~(1<<PINA0);
	PORTA |= 1 << PINA1;
	PORTD &= ~(1<<PIND7);
	enable();
}

void clear(){
	displayCursorBlinkingOn();
	PORTA |= 1<<PINA0;
	PORTA &= ~(1<<PINA1) & ~(1<<PINA2) & ~(1<<PINA4) & ~(1<<PINA5) & ~(1<<PINA6) & ~(1<<PINA7);
	PORTD &= ~(1<<PIND7);
	enable();
	_delay_ms(600);
	displayOnCursorOffBlinkingOff();
	setDdramAddress(0x00);
}

void newClear(){
	counter	= 0;
	setDdramAddress(0);
	while (counter < 64)
	{
		writeData(LCD_SPACE);
		counter++;
	}
	counter = 0;
	setDdramAddress(0);
	_delay_ms(200);
}

void newClearRow0Delay(){
	counter	= 0;
	setDdramAddress(0x00);
	while (counter < 16)
	{
		if (!modeChooser){break;}
		writeData(LCD_SPACE);
		counter++;
		_delay_ms(400);
	}
	counter = 0;
	setDdramAddress(0);
}

void newClearRow1Delay(){
	counter	= 0;
	setDdramAddress(0x10);
	while (counter < 16)
	{
		if (!modeChooser){break;}
		writeData(LCD_SPACE);
		counter++;
		_delay_ms(400);
	}
	counter = 0;
	setDdramAddress(0);
}

void newClearRow2Delay(){
	counter	= 0;
	setDdramAddress(0x08);
	while (counter < 16)
	{
		if (!modeChooser){break;}
		writeData(LCD_SPACE);
		counter++;
		_delay_ms(400);
	}
	counter = 0;
	setDdramAddress(0);
}

void newClearRow3Delay(){
	counter	= 0;
	setDdramAddress(0x18);
	while (counter < 16)
	{
		if (!modeChooser){break;}
		writeData(LCD_SPACE);
		counter++;
		_delay_ms(400);
	}
	counter = 0;
	setDdramAddress(0);
}

void newClearRow0(){
	counter	= 0;
	setDdramAddress(0x00);
	while (counter < 16)
	{
		if (!modeChooser){break;}
		writeData(LCD_SPACE);
		counter++;
	}
	counter = 0;
	setDdramAddress(0);
}

void newClearRow1(){
	counter	= 0;
	setDdramAddress(0x10);
	while (counter < 16)
	{
		if (!modeChooser){break;}
		writeData(LCD_SPACE);
		counter++;
	}
	counter = 0;
	setDdramAddress(0);
}

void newClearRow2(){
	counter	= 0;
	setDdramAddress(0x08);
	while (counter < 16)
	{
		if (!modeChooser){break;}
		writeData(LCD_SPACE);
		counter++;
	}
	counter = 0;
	setDdramAddress(0);
}

void newClearRow3(){
	counter	= 0;
	setDdramAddress(0x18);
	while (counter < 16)
	{
		if (!modeChooser){break;}
		writeData(LCD_SPACE);
		counter++;
	}
	counter = 0;
	setDdramAddress(0);
}

void newClearFast(){
	counter	= 0;
	setDdramAddress(0x00);
	while (counter < 64)
	{
		writeData(LCD_SPACE);
		counter++;
	}
	counter = 0;
	setDdramAddress(0x00);
}

void setDdramAddress(int number){
	
	hexToBinary(convertHexToChar(number / 0x10));
	binaryNumberFull[0] = binaryNumberHalf[0];
	binaryNumberFull[1] = binaryNumberHalf[1];
	binaryNumberFull[2] = binaryNumberHalf[2];
	binaryNumberFull[3] = binaryNumberHalf[3];
	hexToBinary(convertHexToChar(number % 0x10));
	binaryNumberFull[4] = binaryNumberHalf[0];
	binaryNumberFull[5] = binaryNumberHalf[1];
	binaryNumberFull[6] = binaryNumberHalf[2];
	binaryNumberFull[7] = binaryNumberHalf[3];
	
	if (binaryNumberFull[2]){
		PORTA |= 1<<PINA6;
	}
	else{PORTA &= ~(1<<PINA6);}
	if (binaryNumberFull[3]){
		PORTA |= 1<<PINA5;
	}
	else{PORTA &= ~(1<<PINA5);}
	if (binaryNumberFull[4]){
		PORTA |= 1<<PINA4;
	} 
	else{PORTA &= ~(1<<PINA4);}
	if (binaryNumberFull[5]){
		PORTA |= 1<<PINA2;
	} 
	else{PORTA &= ~(1<<PINA2);}
	
	if (binaryNumberFull[6]){
		PORTA |= 1<<PINA1;
	} 
	else{PORTA &= ~(1<<PINA1);}
	if (binaryNumberFull[7]){
		PORTA |= 1<<PINA0;
	} 
	else{PORTA &= ~(1<<PINA0);}
	
	PORTB &= ~(1<<PINB1) & ~(1<<PINB0);
	PORTD |= 1<<PIND7;
	PORTA &= ~(1<<PINA7);
	enable();
}


void writeData(int number){
	hexToBinary(convertHexToChar(number / 0x10));
	binaryNumberFull[0] = binaryNumberHalf[0];
	binaryNumberFull[1] = binaryNumberHalf[1];
	binaryNumberFull[2] = binaryNumberHalf[2];
	binaryNumberFull[3] = binaryNumberHalf[3];
	hexToBinary(convertHexToChar(number % 0x10));
	binaryNumberFull[4] = binaryNumberHalf[0];
	binaryNumberFull[5] = binaryNumberHalf[1];
	binaryNumberFull[6] = binaryNumberHalf[2];
	binaryNumberFull[7] = binaryNumberHalf[3];
	
	if (binaryNumberFull[0])
	{
		PORTD |= 1<<PIND7;
	}else{PORTD &= ~(1<<PIND7);}
	if (binaryNumberFull[1])
	{
		PORTA |= 1<<PINA7;
	}else{PORTA &= ~(1<<PINA7);}
	if (binaryNumberFull[2])
	{
		PORTA |= 1<<PINA6;
	}else{PORTA &= ~(1<<PINA6);}
	if (binaryNumberFull[3])
	{
		PORTA |= 1<<PINA5;
	}else{PORTA &= ~(1<<PINA5);}
	if (binaryNumberFull[4])
	{
		PORTA |= 1<<PINA4;
	}else{PORTA &= ~(1<<PINA4);}
	if (binaryNumberFull[5])
	{
		PORTA |= 1<<PINA2;
	}else{PORTA &= ~(1<<PINA2);}
	if (binaryNumberFull[6])
	{
		PORTA |= 1<<PINA1;
	}else{PORTA &= ~(1<<PINA1);}
	if (binaryNumberFull[7])
	{
		PORTA |= 1<<PINA0;
	}else{PORTA &= ~(1<<PINA0);}
	
	PORTB |= 1<<PINB0;
	PORTB &= ~(1<<PINB1);
	enable();
}

void printString(char message[]){
	counterMessage = 0;
	while(message[counterMessage] != 0){
		writeData(message[counterMessage]);
		counterMessage++;
	}
	counterMessage = 0;
}

void printStringDelay(char message[]){
	counterMessage = 0;
	while(message[counterMessage] != 0){
		if (!modeChooser){break;}
		writeData(message[counterMessage]);
		counterMessage++;
		_delay_ms(400);
	}
	counterMessage = 0;
}


void hexToBinary(char hexNumber){
		if(hexNumber == "0"){
			binaryNumberHalf[0] = 0;
			binaryNumberHalf[1] = 0;
			binaryNumberHalf[2] = 0;
			binaryNumberHalf[3] = 0;
		}else if(hexNumber == "1"){
			binaryNumberHalf[0] = 0;
			binaryNumberHalf[1] = 0;
			binaryNumberHalf[2] = 0;
			binaryNumberHalf[3] = 1;
		}else if(hexNumber == "2"){
			binaryNumberHalf[0] = 0;
			binaryNumberHalf[1] = 0;
			binaryNumberHalf[2] = 1;
			binaryNumberHalf[3] = 0;
		}else if(hexNumber == "3"){
			binaryNumberHalf[0] = 0;
			binaryNumberHalf[1] = 0;
			binaryNumberHalf[2] = 1;
			binaryNumberHalf[3] = 1;
		}else if(hexNumber == "4"){
			binaryNumberHalf[0] = 0;
			binaryNumberHalf[1] = 1;
			binaryNumberHalf[2] = 0;
			binaryNumberHalf[3] = 0;
		}else if(hexNumber == "5"){
			binaryNumberHalf[0] = 0;
			binaryNumberHalf[1] = 1;
			binaryNumberHalf[2] = 0;
			binaryNumberHalf[3] = 1;
		}else if(hexNumber == "6"){
			binaryNumberHalf[0] = 0;
			binaryNumberHalf[1] = 1;
			binaryNumberHalf[2] = 1;
			binaryNumberHalf[3] = 0;
		}else if(hexNumber == "7"){
			binaryNumberHalf[0] = 0;
			binaryNumberHalf[1] = 1;
			binaryNumberHalf[2] = 1;
			binaryNumberHalf[3] = 1;
		}else if(hexNumber == "8"){
			binaryNumberHalf[0] = 1;
			binaryNumberHalf[1] = 0;
			binaryNumberHalf[2] = 0;
			binaryNumberHalf[3] = 0;
		}else if(hexNumber == "9"){
			binaryNumberHalf[0] = 1;
			binaryNumberHalf[1] = 0;
			binaryNumberHalf[2] = 0;
			binaryNumberHalf[3] = 1;
		}else if(hexNumber == "A"){
			binaryNumberHalf[0] = 1;
			binaryNumberHalf[1] = 0;
			binaryNumberHalf[2] = 1;
			binaryNumberHalf[3] = 0;
		}else if(hexNumber == "B"){
			binaryNumberHalf[0] = 1;
			binaryNumberHalf[1] = 0;
			binaryNumberHalf[2] = 1;
			binaryNumberHalf[3] = 1;
		}else if(hexNumber == "C"){
			binaryNumberHalf[0] = 1;
			binaryNumberHalf[1] = 1;
			binaryNumberHalf[2] = 0;
			binaryNumberHalf[3] = 0;
		}else if(hexNumber == "D"){
			binaryNumberHalf[0] = 1;
			binaryNumberHalf[1] = 1;
			binaryNumberHalf[2] = 0;
			binaryNumberHalf[3] = 1;
		}else if(hexNumber == "E"){
			binaryNumberHalf[0] = 1;
			binaryNumberHalf[1] = 1;
			binaryNumberHalf[2] = 1;
			binaryNumberHalf[3] = 0;
		}else if(hexNumber == "F"){
			binaryNumberHalf[0] = 1;
			binaryNumberHalf[1] = 1;
			binaryNumberHalf[2] = 1;
			binaryNumberHalf[3] = 1;
		}else{
			binaryNumberHalf[0] = 0;
			binaryNumberHalf[1] = 0;
			binaryNumberHalf[2] = 0;
			binaryNumberHalf[3] = 0;
		}
}

int convertHexToChar(int number){
	if (number == 0x00)
	{
		return "0";
	}else if(number == 0x01){
		return "1";
	}else if(number == 0x02){
		return "2";
	}else if(number == 0x03){
		return "3";
	}else if(number == 0x04){
		return "4";
	}else if(number == 0x05){
		return "5";
	}else if(number == 0x06){
		return "6";
	}else if(number == 0x07){
		return "7";
	}else if(number == 0x08){
		return "8";
	}else if(number == 0x09){
		return "9";
	}else if(number == 0x0A){
		return "A";
	}else if(number == 0x0B){
		return "B";
	}else if(number == 0x0C){
		return "C";
	}else if(number == 0x0D){
		return "D";
	}else if(number == 0x0E){
		return "E";
	}else if(number == 0x0F){
		return "F";
	}
}


ISR(SIG_UART_RECV){
	while (!(UCSRA & (1<<RXC)));
	
	buffer[counterBuffer] = UDR;
	counterBuffer++;
	
	if(counterBuffer > 99){counterBuffer = 0;}
}


ISR(INT0_vect){
	if (modeChooser)
	{
		optionsChooser = 1;
		modeChooser = 0;
		randomTextChooser = 0;
	}else{
		if (row == 0){randomTextChooser = 1;}
		else if (row == 1){randomTextChooser = 2;}
		else if (row == 2){randomTextChooser = 3;}
		else if (row == 3){randomTextChooser = 4;}
		modeChooser = 1;
		joysticAvailable = 0;
	}
	_delay_ms(1500);
	GIFR |= 1 << INTF0;
}


ISR(ADC_vect){
	adcLowRegister = ADCL;
	adcHighRegister = ADCH;
	
	if (!modeChooser && joysticAvailable)
	{
	// ARROW UP
	if (adcLowRegister < 2){
		setDdramAddress(fieldAddress);
		writeData(LCD_SPACE);
		if (row == 0)
		{
			fieldAddress = 0x19;
			arrowDirection = LCD_RIGHT_ARROW;
			row = 3;
			
		}else if (row == 1)
		{
			fieldAddress = 0x06;
			arrowDirection = LCD_LEFT_ARROW;
			row = 0;
		}else if (row == 2)
		{
			fieldAddress = 0x11;
			arrowDirection = LCD_RIGHT_ARROW;
			row = 1;
		}else if (row == 3)
		{
			fieldAddress = 0x0E;
			arrowDirection = LCD_LEFT_ARROW;
			row = 2;
		}
		
		setDdramAddress(fieldAddress);
		writeData(arrowDirection);
		_delay_ms(2000);
	}
	// ARROW DOWN
	else if (adcLowRegister > 0x0A){
		setDdramAddress(fieldAddress);
		writeData(LCD_SPACE);
		if (row == 0)
		{
			fieldAddress = 0x11;
			arrowDirection = LCD_RIGHT_ARROW;
			row = 1;
			
		}else if (row == 1)
		{
			fieldAddress = 0x0E;
			arrowDirection = LCD_LEFT_ARROW;
			row = 2;
		}else if (row == 2)
		{
			fieldAddress = 0x19;
			arrowDirection = LCD_RIGHT_ARROW;
			row = 3;
		}else if (row == 3)
		{
			fieldAddress = 0x06;
			arrowDirection = LCD_LEFT_ARROW;
			row = 0;
		}
		
		setDdramAddress(fieldAddress);
		writeData(arrowDirection);
		_delay_ms(2000);
	}
	
	}
	
}

