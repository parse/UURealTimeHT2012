#include <stdlib.h>
#include "kernel.h"
#include "kernel_id.h"
#include "ecrobot_interface.h"

#define PRIO_IDLE 10
#define PRIO_DIST 20
#define PRIO_BUTTON 30


#define MOTOR_PERIOD 50
#define BACK_OFF_PERIOD 1000
#define DISTANCE_GOAL 30 //strive to have 32 at the distance reader always

#define PORT_TOUCH NXT_PORT_S1
#define PORT_LIGHT NXT_PORT_S2
#define PORT_ULTRA NXT_PORT_S3
#define PORT_MOTOR_1 NXT_PORT_B
#define PORT_MOTOR_2 NXT_PORT_C

struct dc_t {
    U32 duration;
    S32 speed;
    int distance;
    int priority;
} dc = {0, 0, 0, PRIO_IDLE};

DeclareTask(MotorcontrolTask);
DeclareTask(ButtonPressTask);
DeclareTask(DisplayTask);
DeclareTask(DistanceTask);

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

// used to update the current  dc
void change_driving_command(int priority, int speed, int duration) 
{
	dc.priority = priority;
	dc.speed = speed;
	dc.duration = duration;
}

TASK(MotorcontrolTask)
{
	GetResource(dcResource);
	// when there is a positive duration we are supposed to drive. Else idle.
	if (dc.duration > 0) {
		change_driving_command (dc.priority, dc.speed, dc.duration - MOTOR_PERIOD);	
	} else {
		change_driving_command(PRIO_IDLE, 0, 0);
	}
	ReleaseResource(dcResource);

	ecrobot_set_motor_mode_speed(PORT_MOTOR_1, 1, dc.speed);		
	ecrobot_set_motor_mode_speed(PORT_MOTOR_2, 1, dc.speed);
	
	TerminateTask();
}


TASK(ButtonPressTask)
{
	// When the button is pressed set motors to BACK OFF mode, setting the motors to reverse for 1 second.
	if (ecrobot_get_touch_sensor(PORT_TOUCH)) {
		GetResource(dcResource);
		change_driving_command(PRIO_BUTTON, -50, BACK_OFF_PERIOD);
		ReleaseResource(dcResource);
	}

	TerminateTask();
}

TASK(DistanceTask)
{
	GetResource(dcResource);

	// If the distance reader priority is higher than the currently running prio, get readings and calibrate motor speed.
	if (dc.priority < PRIO_DIST) {
		int distance = ecrobot_get_sonar_sensor(PORT_ULTRA);
		int speed = distance - DISTANCE_GOAL;
		GetResource(dcResource);
		dc.distance = distance;
		ReleaseResource(dcResource);

		// if the speed is negative, we back off, if it is positive we drive forward. If speed is 0 (we are at our target distance) we dont run.
		if (speed != 0) {
			change_driving_command(PRIO_DIST, speed, 200);	
		}
	}

	ReleaseResource(dcResource);
	TerminateTask();
}

TASK(DisplayTask)
{
	GetResource(dcResource);
	display_clear(0);
	display_goto_xy(0,0);
	display_string("dc.priority: ");
	display_int(dc.priority, 2);

	display_goto_xy(0,1);
	display_string("dc.speed: ");
	display_int(dc.speed, 2);

	display_goto_xy(0,2);
	display_string("dc.duration: ");
	display_int(dc.duration, 2);

	display_goto_xy(0,3);
	display_string("dc.distance: ");
	display_int(dc.distance, 3);
	ReleaseResource(dcResource);

	display_update();

	TerminateTask();
}