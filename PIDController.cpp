#include "PIDController.h"

PIDController::PIDController(float Ts)
{
	_Ts = Ts;
	_sat_min = -1.0e10;
	_sat_max = 1.0e10;
	_y = 0.0;
	_y1 = 0.0;
	_u = 0.0;
	_u1 = 0.0;
	_err = 0.0;
	_err1 = 0.0;
	_err2 = 0.0;
	_P = 1.0;
	_I = 0.0;
	_D = 0.0;
	_Ts = 0.1;
}

void PIDController::setProportionalGain(float P)
{
	_P = P;
}

void PIDController::setIntegralGain(float I)
{
	_I = I;
}

void PIDController::setDerivativeGain(float D)
{
	_D = D;
}

void PIDController::setInitialConditions(float y1)
{
	_y1 = y1;
}

void PIDController::saturation(float sat_min, float sat_max)
{
	_sat_min = sat_min;
	_sat_max = sat_max;
}

void PIDController::reset()
{
	_y = 0.0;
	_y1 = 0.0;
	_u = 0.0;
	_u1 = 0.0;
	_err = 0.0;
	_err1 = 0.0;
	_err2 = 0.0;
}

float PIDController::compute(float y, float setpoint){
	_err = setpoint - y;
	_u = _u1 + (_P + _I*_Ts + _D/_Ts)*_err + (-_P-2*_D/_Ts)*_err1 + _D/_Ts*_err2;
	_u1 = _u;
	_err2 = _err1;
	_err1 = _err;
	_usat = clamp(_u, _sat_min, _sat_max);
	return _usat;
}

float PIDController::clamp(float val, float min, float max)
{
  const float x = val < min ? min : val;
  return x > max ? max : x;
}








