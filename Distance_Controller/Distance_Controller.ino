#include <SPI.h>

#define CS                    10
#define BASE_SENSOR_PIN       2

#define NUM_MOTORS  3

enum
{
  FRONT_RIGHT_SEN = 0,
  FRONT_LEFT_SEN,
  LEFT_LEFT_SEN,
  LEFT_RIGHT_SEN,
  RIGHT_LEFT_SEN,
  RIGHT_RIGHT_SEN,
  BOTTOM_SEN,
  NUM_SENSORS
};

volatile byte state = 0;
volatile byte arg1 = 0;
int distanceReading[NUM_SENSORS];

int pulse(byte pin)
{
  int time = 0;
  
  pinMode(pin, OUTPUT);
  
  digitalWrite(pin, LOW);
  delayMicroseconds(2);
  digitalWrite(pin, HIGH);
  delayMicroseconds(5);
  digitalWrite(pin, LOW);
  
  pinMode(pin, INPUT);
  time = pulseIn(pin, HIGH, 65535);  
  
  return time;
}

void setup (void)
{
  Serial.begin (115200);   // debugging

  // turn on SPI in slave mode
  SPCR |= bit (SPE);

  // have to send on master in, *slave out*
  pinMode(MISO, OUTPUT);
  pinMode(MOSI, INPUT);
  pinMode(SCK, INPUT);
  pinMode(CS, INPUT);

  for(byte i = BASE_SENSOR_PIN; i < NUM_SENSORS + BASE_SENSOR_PIN; i++)
  {
    pinMode(i, OUTPUT);
    digitalWrite(i, LOW);
  }

  // now turn on interrupts
  SPI.attachInterrupt();
}


// SPI interrupt routine
ISR (SPI_STC_vect)
{
  byte c = SPDR;  // grab byte from SPI Data Register
  
  switch(state)
  {
    case 0:
    arg1 = c;
    SPDR = *((unsigned char*)&distanceReading[arg1]);
    state = 1;
    break;
    
    case 1:
    SPDR = *((unsigned char*)&distanceReading[arg1] + 1);
    state = 2;
    break;
    
    case 2:
    state = 0;
    break;
  }
}

byte i = 0;

void loop (void)
{
  if(digitalRead(CS))
  {
    cli();
    state = 0;
    sei();
  }
  
  cli();
  distanceReading[i] = pulse(i + BASE_SENSOR_PIN);
  sei();
  
  if(i >= NUM_SENSORS + BASE_SENSOR_PIN)
    i = 0;
  else
    i++;
  
}
