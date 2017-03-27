#ifndef PIDController_h
#define PIDController_h

#if ARDUINO >= 100
  #include "Arduino.h"
#else
  #include "WProgram.h"
  #include "pins_arduino.h"
  #include "WConstants.h"
#endif

// Definition of PIDController class header

class PIDController {
	public:
		PIDController(float Ts);

		void setProportionalGain(float P);
		void setIntegralGain(float I);
		void setDerivativeGain(float D);
		void setInitialConditions(float y1);
		void setIntegralDisableAntiwindup(boolean aw_on_off, float sat_hysteresis);
		void saturation(float sat_min, float sat_max);
		void reset();

		float compute(float y, float setpoint);

	private:
		float clamp(float val, float min, float max);
		float _sat_hysteresis;
		float _sat_min;
		float _sat_max;
		float _y;
		float _y1;
		float _u;
		float _u1;
		float _usat;
		float _err;
		float _err1;
		float _err2;
		float _P;
		float _I;
		float _D;
		float _Ts;
		float _aw;
		boolean _aw_on_off;
};

#endif