#include <IAMFlexy.h>

#define PWM_CONTROL_PIN 6   // Digital PWM pin for fan speed control is 6
#define ADC_PIN A4          // Flex sensor connected to analog input 4
#define USER_INPUT_PIN A5   // Potentiometer connected to analog input 5
#define TERMINAL_INPUT_PIN A3 // Input pin at front terminal

IAMFlexy flexy(PWM_CONTROL_PIN, ADC_PIN, USER_INPUT_PIN);   // Create an instance of IAMFlexy

float Ts = 0.1;              // sampling period [sec]
float f = 1.0/Ts;             // sampling frequency [Hz]
volatile boolean flag=false;

boolean printData = false;
const char startChar = '<';
const char endChar = '>';
const byte buffSize = 8;
char command;
uint8_t cmdVal;
char buff[buffSize];
byte numOfBytes = 0;
char actByte;
boolean reading = false;
boolean finishedReading = false;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  setup_realtime_execution(f);              // setup the timer for boolean variable 'flag'
}

void loop() {
  readCommand();
  processCommand();
  printDataFun();
}

void readCommand(){
  if(Serial.available()>0){
    actByte = Serial.read();
    if(actByte == endChar){
      reading = false;
      finishedReading = true;
      buff[numOfBytes] = 0;
      parseCommand();
      }
    
    if(reading){
      buff[numOfBytes] = actByte;
      numOfBytes++;
      }
    
    if(actByte == startChar){
      numOfBytes = 0;
      reading = true;
      }
    }
  }

void parseCommand(){
  if(finishedReading){
    char *token = strtok(buff,":");
    command = token[0];
    token = strtok(NULL,":");
    cmdVal = atoi(token);
    }
  }

void processCommand(){
  if(finishedReading){
    finishedReading = false;
    switch(command){
      case 'C':
        flexy.calibrate();        // Make some calibration to get full output range. Takes about 6 seconds.
        break;
      case 'P':
        if(cmdVal==1){ 
            printData = true;
            }
        if(cmdVal==0){
            printData = false;
            }
        break;
      case 'F':
        analogWrite(PWM_CONTROL_PIN,cmdVal);
        break;
      case 'S':
        setup_realtime_execution(cmdVal);
        break;
      default:
        break;
      }
    }
  }

void printDataFun(){
  if(flag && printData){
    Serial.print("<D:");
    Serial.print(flexy.getBendPerc());
    Serial.print(",");
    Serial.print(flexy.getUserInputPerc());
    Serial.print(",");
    Serial.print(map(analogRead(TERMINAL_INPUT_PIN),0,1023,0,10000)/100.0);
    Serial.println(">");
    flag = false;
    }
  }

void setup_realtime_execution(float freq){  
  cli();
  //set TIMER1 CTC interrupts at {freq}Hz (Ts = 1/{freq} [s])
  TCCR1A = B00000000;   // set entire TCCR1A register to 0
  TCCR1B = B00000000;   // set entire TCCR1B register to 0
  TCNT1  = 0;           // set counter1 to 0
  // set compare match register to achieve {freq}Hz frequency
  OCR1A = (int)((16.0e6)/(freq*64.0) - 1.0);   //(must be <65536 for TIMER1)
  // enable CTC mode
  TCCR1B |= (1 << WGM12);
  // Set CS11 and CS10 bits for 64 prescaler
  TCCR1B |= (1 << CS11) | (1 << CS10);
  // enable timer compare interrupt
  TIMSK1 |= (1 << OCIE1A);
  //enable global interrupts
  sei();
}

ISR(TIMER1_COMPA_vect){
  flag=true;  // just set the flag
}
