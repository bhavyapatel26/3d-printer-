#include <SPI.h>

#define CS                    10
#define DRILL_ENDSTOP_PIN     2
#define LEFT_DRIVE_PWM_PIN    3
#define LEFT_DRIVE_DIR_PIN    4
#define DRILL_DRIVE_PWM_PIN   5
#define DRILL_DRIVE_DIR_PIN   7
#define RIGHT_DRIVE_DIR_PIN   8
#define RIGHT_DRIVE_PWM_PIN   9

#define NUM_MOTORS  3

enum
{
  LEFT_DRIVE = 0,
  RIGHT_DRIVE,
  DRILL_DRIVE
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

volatile byte state = 0;
volatile byte curCommand = NO_COMMAND;
volatile byte arg1, arg2;
volatile byte motorPower[NUM_MOTORS], motorDir[NUM_MOTORS];

volatile bool updateNeeded = false;

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
    curCommand = c;
    state = 1;
    break;
    
    case 1:
    arg1 = c;
    
    switch(curCommand)
    {
      case GET_POWER:
      SPDR = motorPower[arg1];
      break;
      
      case GET_DIR:
      SPDR = motorDir[arg1];
      break;
    }
    
    state = 2;
    break;
    
    case 2:
    arg2 = c;
    
    switch(curCommand)
    {     
      case SET_POWER:
      setMotorPower(arg1, arg2);
      break;
      
      case SET_DIR:     
      setMotorDir(arg1, arg2);
      break;
    }
    
    state = 0;
    break;
  }
}

void loop (void)
{
  if(digitalRead(CS))
  {
    cli();
    state = 0;
    sei();
  }
}

inline void setMotorPower(byte motorIndex, byte power)
{
  Serial.print("Motor Power Command\tIndex: ");
  Serial.print(motorIndex);
  Serial.print("\tPower: ");
  Serial.println(power);
  
  motorPower[motorIndex] = power;
}

inline void setMotorDir(byte motorIndex, byte dir)
{
  Serial.print("Motor Dir Command\tIndex: ");
  Serial.print(motorIndex);
  Serial.print("\Dir: ");
  Serial.println(dir);  
  
  motorDir[motorIndex] = dir;
}
