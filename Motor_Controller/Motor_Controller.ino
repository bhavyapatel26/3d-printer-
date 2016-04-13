#include <SPI.h>

#define CS                    10
#define DRILL_ENDSTOP_PIN     2
#define LEFT_DRIVE_PWM_PIN    3
#define LEFT_DRIVE_DIR_PIN    4
#define DRILL_DRIVE_PWM_PIN   5
#define DRILL_DRIVE_DIR_PIN   A1
#define RIGHT_DRIVE_DIR_PIN   8
#define RIGHT_DRIVE_PWM_PIN   9

#define PUMP_DRIVE_PWM_PIN    6
#define PUMP_DRIVE_DIR_PIN    7

#define HEARTBEAT_PIN         A2

enum
{
  LEFT_DRIVE = 0,
  RIGHT_DRIVE,
  DRILL_DRIVE,
  PUMP_DRIVE,
  NUM_MOTORS
};

byte motorPWMPin[] = {LEFT_DRIVE_PWM_PIN, RIGHT_DRIVE_PWM_PIN, DRILL_DRIVE_PWM_PIN, PUMP_DRIVE_PWM_PIN};
byte motorDirPin[] = {LEFT_DRIVE_DIR_PIN, RIGHT_DRIVE_DIR_PIN, DRILL_DRIVE_DIR_PIN, PUMP_DRIVE_DIR_PIN};

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
volatile byte updateMotor = 0;

long lastHeartbeat = 0;

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

  pinMode(DRILL_ENDSTOP_PIN, INPUT);
  
  for(int i = 0; i < NUM_MOTORS; i++)
  {
    pinMode(motorPWMPin[i], OUTPUT);
    pinMode(motorDirPin[i], OUTPUT);
    
    digitalWrite(motorPWMPin[i], LOW);
    digitalWrite(motorDirPin[i], LOW);
  }

  // now turn on interrupts
  SPI.attachInterrupt();
}


// SPI interrupt routine
ISR (SPI_STC_vect)
{
  byte c = SPDR;  // grab byte from SPI Data Register
  
  Serial.print(state);
  Serial.print('\t');
  Serial.println(c);
  
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
  if(state != 0 && digitalRead(CS) == HIGH)
  {
    state = 0;
  }
  
  if(millis() - lastHeartbeat > 1000)
  {
    lastHeartbeat = millis();
    digitalWrite(HEARTBEAT_PIN, !digitalRead(HEARTBEAT_PIN));
  }
  
  if(updateNeeded)
  {
    updateNeeded = false;
    
    digitalWrite(motorDirPin[updateMotor], motorDir[updateMotor]);
    analogWrite(motorPWMPin[updateMotor], motorPower[updateMotor]);
  }
}

inline void setMotorPower(byte motorIndex, byte power)
{  
  motorPower[motorIndex] = power;

  analogWrite(motorPWMPin[motorIndex], power);
}

inline void setMotorDir(byte motorIndex, byte dir)
{
  motorDir[motorIndex] = dir;

  digitalWrite(motorDirPin[motorIndex], dir);
}
