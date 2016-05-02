/*
 * MotorController.c
 *
 * Created: 4/25/2016 11:36:44 AM
 * Author : Sam
 */ 

#define F_CPU	16000000UL

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#define DRILL_ENDSTOP_PIN			PORTD2
#define DRILL_ENDSTOP_DIR_REG		DDRD
#define DRILL_ENDSTOP_PORT_REG		PORTD

#define LEFT_DRIVE_PWM_PIN			PORTD3
#define LEFT_DRIVE_PWM_DIR_REG		DDRD
#define LEFT_DRIVE_PWM_PORT_REG		PORTD
#define LEFT_DRIVE_PWM_TIMER_REG	OCR2B

#define LEFT_DRIVE_DIR_PIN			PORTD4
#define LEFT_DRIVE_DIR_DIR_REG		DDRD
#define LEFT_DRIVE_DIR_PORT_REG		PORTD

#define RIGHT_DRIVE_PWM_PIN			PORTB1
#define RIGHT_DRIVE_PWM_DIR_REG		DDRB
#define RIGHT_DRIVE_PWM_PORT_REG	PORTB
#define RIGHT_DRIVE_PWM_TIMER_REG	OCR1A

#define RIGHT_DRIVE_DIR_PIN			PORTB0
#define RIGHT_DRIVE_DIR_DIR_REG		DDRB
#define RIGHT_DRIVE_DIR_PORT_REG	PORTB

#define DRILL_DRIVE_PWM_PIN			PORTD5
#define DRILL_DRIVE_PWM_DIR_REG		DDRD
#define DRILL_DRIVE_PWM_PORT_REG	PORTD
#define DRILL_DRIVE_PWM_TIMER_REG	OCR0B

#define DRILL_DRIVE_DIR_PIN			PORTC1
#define DRILL_DRIVE_DIR_DIR_REG		DDRC
#define DRILL_DRIVE_DIR_PORT_REG	PORTC

#define PUMP_DRIVE_PWM_PIN			PORTD6
#define PUMP_DRIVE_PWM_DIR_REG		DDRD
#define PUMP_DRIVE_PWM_PORT_REG		PORTD
#define PUMP_DRIVE_PWM_TIMER_REG	OCR0A

#define PUMP_DRIVE_DIR_PIN			PORTD7
#define PUMP_DRIVE_DIR_DIR_REG		DDRD
#define PUMP_DRIVE_DIR_PORT_REG		PORTD

#define _BV(bit)		(1 << (bit))
#define sbi(sfr, bit)	sfr |= _BV(bit)
#define cbi(sfr, bit)	sfr &= ~(_BV(bit))

enum
{
	LEFT_DRIVE = 0,
	RIGHT_DRIVE,
	DRILL_DRIVE,
	PUMP_DRIVE,
	NUM_MOTORS	
};

enum
{
	NO_COMMAND = 0,
	SET_POWER,
	SET_DIR,
	GET_POWER,
	GET_DIR	
};

enum
{
	FORWARD = 0,
	REVERSE	
};

unsigned char state = 0;
unsigned char curCommand = NO_COMMAND;
unsigned char arg1, arg2;
unsigned char motorPower[NUM_MOTORS], motorDir[NUM_MOTORS];

void setupTimers(void)
{
	//Timer 0 PORTD5 (B)(Drill Drive) PORTD6 (A)(Pump Drive)
	TCCR0A = (1 << COM0A1) | (1 << COM0B1) | (1 << WGM01) | (1 << WGM00);
	OCR0A = 0;
	OCR0B = 0;
	TCCR0B = (1 << CS00) | (1 << CS01);
	
	//Timer 1 PORTB1 (A)(Right Drive)
	TCCR1A = (1 << COM1A1) | (1 << WGM10);
	OCR1A = 0;
	TCCR1B = (1 << WGM12) | (1 << CS10) | (1 << CS11);
	
	//Timer 2  PORTD3 (B)(Left Drive)
	TCCR2A = (1 << COM2B1) | (1 << WGM21) | (1 << WGM20);
	OCR2B = 0;
	TCCR2B = (1 << CS20) | (1 << CS21);
}

void SPI_SlaveInit(void)
{
	sbi(DDRB, PORTB3);
	
	cbi(DDRB, PORTB5);
	sbi(PORTB, PORTB5);
	
	cbi(DDRB, PORTB4);
	sbi(PORTB, PORTB4);
	
	cbi(DDRB, PORTB2);
	sbi(PORTB, PORTB2);
	
	SPCR = (1 << SPE);
}

void SPI_SlaveReceive(void)
{
	unsigned char c;
	while(!(SPSR & (1<<SPIF)))
	{
		if((PINB & (1 << PINB2)))
			state = 0;
	}
	
	c = SPDR;
	
	switch(state)
	{
		case 0:
		curCommand = c;
		state = 1;
		break;
		
		case 1:
		arg1 = c;
		
		switch(curCommand)
		{
			case GET_DIR:
			break;
			
			case GET_POWER:
			break;
		}
		
		state = 2;
		break;
		
		case 2:
		arg2 = c;
				
		switch(curCommand)
		{
			case SET_DIR:
			switch(arg1)
			{
				case LEFT_DRIVE:
				switch(arg2)
				{
					case FORWARD:
					cbi(LEFT_DRIVE_DIR_PORT_REG, LEFT_DRIVE_DIR_PIN);
					break;
					
					case REVERSE:
					sbi(LEFT_DRIVE_DIR_PORT_REG, LEFT_DRIVE_DIR_PIN);
					break;
				}
				break;
				
				case RIGHT_DRIVE:
				switch(arg2)
				{
					case FORWARD:
					cbi(RIGHT_DRIVE_DIR_PORT_REG, RIGHT_DRIVE_DIR_PIN);
					break;
					
					case REVERSE:
					sbi(RIGHT_DRIVE_DIR_PORT_REG, RIGHT_DRIVE_DIR_PIN);
					break;
				}				
				break;
				
				case PUMP_DRIVE:
				switch(arg2)
				{
					case FORWARD:
					cbi(PUMP_DRIVE_DIR_PORT_REG, PUMP_DRIVE_DIR_PIN);
					break;
					
					case REVERSE:
					sbi(PUMP_DRIVE_DIR_PORT_REG, PUMP_DRIVE_DIR_PIN);
					break;
				}				
				break;
				
				case DRILL_DRIVE:
				switch(arg2)
				{
					case FORWARD:
					cbi(DRILL_DRIVE_DIR_PORT_REG, DRILL_DRIVE_DIR_PIN);
					break;
					
					case REVERSE:
					sbi(DRILL_DRIVE_DIR_PORT_REG, DRILL_DRIVE_DIR_PIN);
					break;
				}				
				break;
			}
			break;
			
			case SET_POWER:
			switch(arg1)
			{
				case LEFT_DRIVE:
				LEFT_DRIVE_PWM_TIMER_REG = arg2;
				break;
				
				case RIGHT_DRIVE:
				RIGHT_DRIVE_PWM_TIMER_REG = arg2;
				break;
				
				case PUMP_DRIVE:
				PUMP_DRIVE_PWM_TIMER_REG = arg2;
				break;
				
				case DRILL_DRIVE:
				DRILL_DRIVE_PWM_TIMER_REG = arg2;
				break;
			}			
			break;
		}
		
		state = 0;
		break;
	}
}

int main(void)
{
	setupTimers();
	
	cbi(DRILL_ENDSTOP_DIR_REG, DRILL_ENDSTOP_PIN);
	
	sbi(LEFT_DRIVE_PWM_DIR_REG, LEFT_DRIVE_PWM_PIN);
	sbi(RIGHT_DRIVE_PWM_DIR_REG, RIGHT_DRIVE_PWM_PIN);
	sbi(DRILL_DRIVE_PWM_DIR_REG, DRILL_DRIVE_PWM_PIN);
	sbi(PUMP_DRIVE_PWM_DIR_REG, PUMP_DRIVE_PWM_PIN);
	
	sbi(LEFT_DRIVE_DIR_DIR_REG, LEFT_DRIVE_DIR_PIN);
	sbi(RIGHT_DRIVE_DIR_DIR_REG, RIGHT_DRIVE_DIR_PIN);
	sbi(DRILL_DRIVE_DIR_DIR_REG, DRILL_DRIVE_DIR_PIN);
	sbi(PUMP_DRIVE_DIR_DIR_REG, PUMP_DRIVE_DIR_PIN);
	
	cbi(LEFT_DRIVE_PWM_PORT_REG, LEFT_DRIVE_PWM_PIN);
	cbi(RIGHT_DRIVE_PWM_PORT_REG, RIGHT_DRIVE_PWM_PIN);
	cbi(DRILL_DRIVE_PWM_PORT_REG, DRILL_DRIVE_PWM_PIN);
	cbi(PUMP_DRIVE_PWM_PORT_REG, PUMP_DRIVE_PWM_PIN);	
	
	SPI_SlaveInit();	
	
    while (1) 
    {
		//sbi(LEFT_DRIVE_PWM_PORT_REG, LEFT_DRIVE_PWM_PIN);
		SPI_SlaveReceive();
		//cbi(LEFT_DRIVE_PWM_PORT_REG, LEFT_DRIVE_PWM_PIN);
    }
}

