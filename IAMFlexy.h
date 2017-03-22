#ifndef IAMFlexy_h
#define IAMFlexy_h

#if ARDUINO >= 100
  #include "Arduino.h"
#else
  #include "WProgram.h"
  #include "pins_arduino.h"
  #include "WConstants.h"
#endif

// Definition of IAMFlexy class header 

class IAMFlexy {
	public:
		IAMFlexy(byte pwm_control_pin, byte adc_pin, byte user_input_pin);

		void calibrate();

		void setFanSpeedPerc(float perc);
		void setFanSpeedNorm(float norm);
		void setFanSpeedRPM(float rpm);

		float getBendPerc();
		float getBendNorm();
		float getUserInputPerc();
		float getUserInputNorm();

		void initFilter(float init);
		float applyFilter(float input, float weight);

	private:
		int _out_max;
		int _out_min;
		uint8_t _pwm_control_pin;
		uint8_t _adc_pin;
		uint8_t _user_input_pin;
		int _reading;
		int _user_input;
		float _filt_last;
		float _filt_act;
		float mapToFloat(int in, float in_min, float in_max, float out_min, float out_max);
};

#endif