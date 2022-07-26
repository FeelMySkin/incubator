#ifndef PID_CONTROLLER_H
#define PID_CONTROLLER_H

#include "defines.h"

struct PID_InitTypeDef
{
	double Kp;
	double Ki;
	double Kd;
	double max_val;
};
//

class PIDController
{
    public:
        PIDController();
        ~PIDController();
        void Init(PID_InitTypeDef);
		double CalculateError(double difference, double ms_ellapsed);

    private:
		PID_InitTypeDef pids;
		double PID_p,PID_i,PID_d;
		double last_error;
		double curr_error;

};


#endif
