#include <IAMFlexy.h>
#include <PIDController.h>

#define PWM_CONTROL_PIN 6     // Digital PWM pin for fan speed control is 6
#define ADC_PIN A4            // Flex sensor connected to analog input 4
#define USER_INPUT_PIN A5     // Potentiometer connected to analog input 5

IAMFlexy flexy(PWM_CONTROL_PIN, ADC_PIN, USER_INPUT_PIN);   // Create an instance of IAMFlexy
float Ts = 50.0;              // sampling period [sec]
float f = 1.0/Ts;             // sampling frequency [Hz]
PIDController pid(f);         // initialize PID controller with desired sampling frequency

float w;          // control setpoint
float ys = 33.0;  // output in selected steady state (this should be measured in open loop)
float us = 50.0;  // input in selected steady state
float y;          // actual output
float u;          // actual input

/*
 * flag is a time-dependent control switch for program flow.  
 * The variable changes its boolean value to TRUE with frequency f.
 * It is used to control the real-time execution of main loop.
 */
volatile boolean flag=false;

void setup() {
Serial.begin(115200);                     // initialize a serial interface. Want to see what is going on.

flexy.calibrate();                        // Make some calibration to get full output range. Takes about 6 seconds.
flexy.initFilter(flexy.getBendPerc());    // initialize a filter (use current output measurement as initial value)

/*
 * This example uses PI controller with parameters P = 10 and I = 0.01
 */

pid.setProportionalGain(10.0);            // set proportional gain
pid.setIntegralGain(0.01);                // set integral gain
pid.saturation(-50.0,50.0);               // Apply saturation to deviation of control variable (in percentage) from 
                                          // its reference point (us = 50) [umax = 100, umin = 0].

setup_realtime_execution(f);              // setup the timer for boolean variable 'flag'
}

void loop() {                             // let's loop around
  /*
   * The following code will be executed with precise timing defined by frequency 'f'.
   */
  if(flag){
    w = flexy.getUserInputPerc();                     // read user input (potentiomenter [0-100]) and use it as control setpoint
    y = flexy.applyFilter(flexy.getBendPerc(),0.4);   // read smoothed output
    u = pid.compute(y, w);                            // compute control 'u' (deviation from 'us')
    flexy.setFanSpeedPerc(u+us);                      // apply control to the system

    /*
     * Print some data to serial interface for user to see.
     */

    Serial.print(y);
    Serial.print(" ");
    Serial.print(w);
    Serial.println();
    flag=!flag;         // don't forget to switch the 'flag' and wait for another
  }
}

/*
 *  The following function configures the TIMER1 of ATMega328p for CTC mode on compare match register A.
 *  This function directly manipulates the registers of MCU. Do not change anything, unless you know what you are doing.
 *  For more information see the register description in chap. 20.14: 
 *  http://www.atmel.com/Images/Atmel-42735-8-bit-AVR-Microcontroller-ATmega328-328P_Datasheet.pdf
 */

void setup_realtime_execution(float freq){  
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

/*
 * Interrupt service routine is a low level function that is invoked by interrupt vector.s
 */

ISR(TIMER1_COMPA_vect){
  flag=true;  // just set the flag
}
