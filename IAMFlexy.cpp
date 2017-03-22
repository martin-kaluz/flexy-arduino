#include "IAMFlexy.h"

IAMFlexy::IAMFlexy(uint8_t pwm_control_pin, uint8_t adc_pin, uint8_t user_input_pin)
{
	_out_max = 1023;
	_out_min = 0;
	_pwm_control_pin = pwm_control_pin;
	_adc_pin = adc_pin;
	_filt_last = 0.0;
	_filt_act = 0.0;
	_user_input_pin = user_input_pin;
	_reading = 0;
	pinMode(_pwm_control_pin,OUTPUT);
}

void IAMFlexy::calibrate()
{
	_out_max = 0;
	_out_min = 1023;
	for(int i=0;i<100;i++){
		_reading = analogRead(_adc_pin);
		if(_reading<_out_min) _out_min = _reading;
		delay(10);
	}
	for(int i=0;i<3;i++){
		setFanSpeedPerc(50.0);
		delay(500);
		setFanSpeedPerc(100.0);
		for(int j=0;j<100;j++){
			_reading = analogRead(_adc_pin);
			if(_reading>_out_max) _out_max = _reading;
			delay(10);
		}
	}
	setFanSpeedPerc(0.0);
	delay(2000);
}

void IAMFlexy::setFanSpeedPerc(float perc)
{
	analogWrite(_pwm_control_pin, (int)(2.55*perc));
}

void IAMFlexy::setFanSpeedNorm(float norm)
{
	analogWrite(_pwm_control_pin, (int)(255.0*norm));
}

void IAMFlexy::setFanSpeedRPM(float rpm)
{
	analogWrite(_pwm_control_pin, (int)(0.02326*rpm));
}

float IAMFlexy::getBendPerc()
{
	_reading = analogRead(_adc_pin);
	return mapToFloat(_reading,(float)_out_min,(float)_out_max,0.0,100.0);
}

float IAMFlexy::getBendNorm()
{
	_reading = analogRead(_adc_pin);
	return mapToFloat(_reading,(float)_out_min,(float)_out_max,0.0,1.0);
}

float IAMFlexy::getUserInputPerc()
{
	_user_input = analogRead(_user_input_pin);
	return mapToFloat(_user_input,0.0,1023.0,0.0,100.0);
}

float IAMFlexy::getUserInputNorm()
{
	_user_input = analogRead(_user_input_pin);
	return mapToFloat(_user_input,0.0,1023.0,0.0,1.0);
}

float IAMFlexy::mapToFloat(int in, float in_min, float in_max, float out_min, float out_max)
{
  return ((float)in - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

void IAMFlexy::initFilter(float init)
{
	_filt_last = init;
}

float IAMFlexy::applyFilter(float input, float weight)
{
	_filt_act = weight*input+(1.0-weight)*_filt_last;
	_filt_last = _filt_act;
	return _filt_act;
}