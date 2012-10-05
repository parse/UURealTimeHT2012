#include <stdlib.h>
#include "kernel.h"
#include "kernel_id.h"
#include "ecrobot_interface.h"

#define DISTANCE_GOAL 35 //strive to have 32 at the distance reader always

#define PORT_TOUCH NXT_PORT_S4
#define PORT_LIGHT NXT_PORT_S2
#define PORT_ULTRA NXT_PORT_S3
#define PORT_MOTOR_1 NXT_PORT_B
#define PORT_MOTOR_2 NXT_PORT_C

#define MAX_SPEED_OUTER_WHEEL 80
#define MAX_SPEED_INNER_WHEEL 30

struct dc_t {
    int darkness;
    int maxLight;
    float distanceSpeedFactor;
    S32 speed;
} dc = {0, 0, 0.0f, 0};

DeclareTask(MotorcontrolTask);
DeclareTask(UltraTask);
DeclareTask(LightTask);
DeclareTask(ButtonPressTask);
DeclareTask(CalibrateTask);

DeclareEvent(DataUpdated);
DeclareEvent(StartRunning);
DeclareEvent(StartCalibrate);

DeclareResource(dcResource);

DeclareCounter ( SysTimerCnt ) ;

void ecrobot_device_initialize() 
{
	ecrobot_set_light_sensor_active(PORT_LIGHT);
	ecrobot_init_sonar_sensor(PORT_ULTRA);
}

void ecrobot_device_terminate() 
{
	ecrobot_term_sonar_sensor(PORT_ULTRA);
	ecrobot_set_motor_mode_speed(PORT_MOTOR_1, 1, 0);		
    ecrobot_set_motor_mode_speed(PORT_MOTOR_2, 1, 0);		
    ecrobot_set_light_sensor_inactive(PORT_LIGHT);
}

void user_1ms_isr_type2 () 
{ 
	SignalCounter(SysTimerCnt);
}

U8 darknessChanged(int darkness)
{	
	// If calibration failed, set a standard value
	if (dc.maxLight == 0) 
		dc.maxLight = 600;

	GetResource(dcResource);

	if (dc.darkness > dc.maxLight && darkness < dc.maxLight) {
		return 1;
	}

	if (dc.darkness < dc.maxLight && darkness > dc.maxLight) {
		return 1;
	}
	
	return 0;
	
	ReleaseResource(dcResource);
}

float speedForDistance(U8 distance)
{
	float distToGoal = distance - DISTANCE_GOAL;

	if (distToGoal > 20.0f) {
		return 1.0f;
	}

	if (distToGoal <0)
	{
		return 0.0f;
	}
	
	return distToGoal/20.0f;
}

TASK(MotorcontrolTask)
{
	WaitEvent(StartRunning);

	while (1) {
		WaitEvent(DataUpdated);
		ClearEvent(DataUpdated);

		GetResource(dcResource);

		float innerWheel = MAX_SPEED_INNER_WHEEL * dc.distanceSpeedFactor;
		float outerWheel = MAX_SPEED_OUTER_WHEEL * dc.distanceSpeedFactor;

		if(dc.distanceSpeedFactor < 0) {
			float tmp = innerWheel;
			innerWheel = outerWheel;
			outerWheel = tmp;
		}

		if (dc.darkness > dc.maxLight) {
			ecrobot_set_motor_mode_speed(PORT_MOTOR_1, 1, innerWheel);		
			ecrobot_set_motor_mode_speed(PORT_MOTOR_2, 1, outerWheel);
		} else {
			ecrobot_set_motor_mode_speed(PORT_MOTOR_1, 1, outerWheel);		
			ecrobot_set_motor_mode_speed(PORT_MOTOR_2, 1, innerWheel);
		}

		ReleaseResource(dcResource);
	}
	
	TerminateTask();
}

TASK(UltraTask)
{
	int dist = ecrobot_get_sonar_sensor(PORT_ULTRA);
	
	GetResource(dcResource);
	dc.distanceSpeedFactor = speedForDistance(dist);
	ReleaseResource(dcResource);

	SetEvent(MotorcontrolTask, DataUpdated);
	TerminateTask();
}

TASK(LightTask)
{	
	int darkness = ecrobot_get_light_sensor(PORT_LIGHT);

	GetResource(dcResource);
	if (darknessChanged(darkness)) {
		dc.darkness = darkness;

		SetEvent(MotorcontrolTask, DataUpdated);
	}

	ReleaseResource(dcResource);

	TerminateTask();
}

TASK(CalibrateTask)
{	
	WaitEvent(StartCalibrate);
	ClearEvent(StartCalibrate);
	
	display_clear(0);
	display_goto_xy(0, 0);
	display_string("Light: ");

	int darkness = ecrobot_get_light_sensor(PORT_LIGHT);

	display_int(darkness, 5);
	display_string("\n");
	display_update();

	dc.maxLight = darkness;

	display_string("Press to start");
	display_update();
	
	WaitEvent(StartCalibrate);

	SetEvent(MotorcontrolTask, StartRunning);

	TerminateTask();
}

TASK(ButtonPressTask)
{
	static int prevState = 0;
	int newState = ecrobot_get_touch_sensor(PORT_TOUCH);
	if (prevState == 0 && newState) {
		SetEvent(CalibrateTask, StartCalibrate);
	}
	prevState = newState;

	TerminateTask();
}