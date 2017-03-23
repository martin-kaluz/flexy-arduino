#include <IAMFlexy.h>

#define PWM_CONTROL_PIN 6   // Digital PWM pin for fan speed control is 6
#define ADC_PIN A4          // Flex sensor connected to analog input 4
#define USER_INPUT_PIN A5   // Potentiometer connected to analog input 5

IAMFlexy flexy(PWM_CONTROL_PIN, ADC_PIN, USER_INPUT_PIN);   // Create an instance of IAMFlexy

void setup() {
  Serial.begin(115200);     // Initialize a serial interface. Want to see what is going on.
  
  flexy.calibrate();        // Make some calibration to get full output range. Takes about 6 seconds.

  
  /*  // A simple exponential smoothing filter can be optionally used for measurements
   *  float init_val_for_filter = flexy.getBendPerc();
   *  flexy.initFilter(init_val_for_filter);
   */ 
}

void loop() {
  /* 
   * Get value of potentiometer turn in percentage (user input). 
   */
  float user_input_perc = flexy.getUserInputPerc();  

  /*
   * Apply percentual user input directly to fan speed in percentage.
   */
  flexy.setFanSpeedPerc(user_input_perc);

  /*
   * Get reading of flex sensor in percentage. 
   * If flexy.calibrate() was called in setup, 
   * the values will be scaled to full usable range.   
   */
  float bend_perc = flexy.getBendPerc();

  /* // Optionally a smoothing can be used. Value 0.4 is the weight. 
   * bend_perc = flexy.applyFilter(bend_perc, 0.4);
   */

  /*
   * Print an open-loop values to the serial link for plotting.
   */

  Serial.print(user_input_perc);
  Serial.print(" ");
  Serial.println(bend_perc);
  delay(50);
}
