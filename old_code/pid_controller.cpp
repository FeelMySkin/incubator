#include "pid_controller.h"

PIDController::PIDController()
{

}
//

PIDController::~PIDController()
{

}
//

void PIDController::Init(PID_InitTypeDef pids)
{
	this->pids = pids;
	PID_i = PID_d = PID_p = 0;
}
//

double PIDController::CalculateError(double difference, double ms_ellapsed)
{
	curr_error = difference;
	PID_p = pids.Kp * curr_error;
	
	if(-3 < difference && difference <3)
	{
		PID_i = PID_i + (pids.Ki * curr_error);
	}
	
	PID_d = pids.Kd*((curr_error - last_error)/(ms_ellapsed/1000.0));
	
	//Final total PID value is the sum of P + I + D
	double PID_value = PID_p + PID_i + PID_d;
	
	if(PID_value <0) PID_value = 0;
	else if(PID_value>pids.max_val) PID_value = pids.max_val;
	
	last_error = curr_error;
	return PID_value;
}
//
