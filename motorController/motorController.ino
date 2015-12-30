#define MOTOR1_PWM_PIN  0
#define MOTOR1_DIR_PIN  1

#define MOTOR2_PWM_PIN  2
#define MOTOR2_DIR_PIN  3

#define SPEED_CMD  0x01
#define DIR_CMD    0x02
#define CMD_MSK    0xF0
#define MTR_MSK    0x0F

unsigned char motorPWMPins[] = {MOTOR1_PWM_PIN, MOTOR2_PWM_PIN};
unsigned char motorDirPins[] = {MOTOR1_DIR_PIN, MOTOR2_DIR_PIN};

unsigned char dataBuf[256];
unsigned char writePtr = 0;
unsigned char readPtr = 0;

boolean isDataByte = false;
unsigned char cmdByte = 0;
unsigned char dataByte = 0;

void setup() 
{
	pinMode(MISO, OUTPUT);
	pinMode(SS, INPUT);
	pinMode(MOSI, INPUT);
	pinMode(SCK, INPUT);

	pinMode(MOTOR1_PWM_PIN, OUTPUT);
	pinMode(MOTOR1_DIR_PIN, OUTPUT);

	pinMode(MOTOR2_PWM_PIN, OUTPUT);
	pinMode(MOTOR2_DIR_PIN, OUTPUT);

	SPCR |= _BV(SPE);
	SPCR |= _BV(SPIE);
}

void loop() 
{
	if(readPtr != writePtr)
	{
		while(readPtr != writePtr)
		{
			if(!isDataByte)
			{
				unsigned char cmdSec = (dataBuf[readPtr] & CMD_MSK) >> 4;
				unsigned char motorSec = dataBuf[readPtr] & MTR_MSK;

				if(cmdSec == SPEED_CMD || cmdSec == DIR_CMD)
				{
					isDataByte = true;
					cmdByte = dataBuf[readPtr];
				}
			}
			else
			{
				unsigned char cmdSec = (cmdByte & CMD_MSK) >> 4;
				unsigned char motorSec = cmdByte & MTR_MSK;
				
				dataByte = dataBuf[readPtr];
				
				switch(cmdSec)
				{
					case SPEED_CMD:				
					{
						analogWrite(motorPWMPins[motorSec], dataByte);
						break;
					}
					
					case DIR_CMD:
					{
						digitalWrite(motorDirPins[motorSec], dataByte);
						break;
					}
				}
				
				isDataByte = false;
			}
			
			readPtr++;
		}
	}
}

ISR(SPI_STC_vect)
{
	dataBuf[writePtr] = SPDR;
	writePtr++;
}
