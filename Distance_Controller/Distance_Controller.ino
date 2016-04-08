#include <SPI.h>

#define CS                    10
#define BASE_SENSOR_PIN       2

#define HEARTBEAT_PIN         A0

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
volatile boolean updateNeeded = false;
int distanceReading[NUM_SENSORS];

long lastHeartbeat = 0;

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

  pinMode(HEARTBEAT_PIN, OUTPUT);

  for(byte i = BASE_SENSOR_PIN; i < NUM_SENSORS + BASE_SENSOR_PIN; i++)
  {
    pinMode(i, OUTPUT);
    digitalWrite(i, LOW);
  }

  for(byte i = 0; i < NUM_SENSORS; i++)
  {
    distanceReading[i] = 0xFFFF;
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
    
    if(arg1 == NUM_SENSORS)
    {
      updateNeeded = true;
      SPDR = 0xFF;
    }
    else    
    {
      SPDR = *((unsigned char*)(&distanceReading[arg1]));
    }
      
    state = 1;
    break;
    
    case 1:
    if(arg1 == NUM_SENSORS)
    {
      SPDR = 0xFF;
    }
    else
    {
      SPDR = *(((unsigned char*)(&distanceReading[arg1])) + 1);
    }
    state = 2;
    break;
    
    case 2:
    SPDR = 0;
    state = 0;
    break;
  }
}

byte i = 0;

void loop (void)
{
  if(millis() - lastHeartbeat > 1000)
  {
    lastHeartbeat = millis();
    digitalWrite(HEARTBEAT_PIN, !digitalRead(HEARTBEAT_PIN));
  }
  
  if(state != 0 && digitalRead(CS) == HIGH)
  {
    state = 0;
  }
  
  if(updateNeeded)
  {
    for(byte i = 0; i < NUM_SENSORS; i++)
    {
      distanceReading[i] = pulse(i + BASE_SENSOR_PIN);
    }
    updateNeeded = false;
  }  
}
