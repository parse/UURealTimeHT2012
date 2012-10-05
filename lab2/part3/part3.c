#include <stdlib.h>
#include "kernel.h"
#include "kernel_id.h"
#include "ecrobot_interface.h"

// Button priority is set to highest to avoid running into nuclear bombs (the button is the boss)
#define PRIO_IDLE 10
#define PRIO_DIST 20
#define PRIO_BUTTON 30

// Run forward with a period of 50ms, reverse with a period of 1000ms
#define MOTOR_PERIOD 50
#define BACK_OFF_PERIOD 1000

// Strive to have 30 at the distance reader always
#define DISTANCE_GOAL 30 

#define PORT_TOUCH NXT_PORT_S4
#define PORT_LIGHT NXT_PORT_S2
#define PORT_ULTRA NXT_PORT_S3
#define PORT_MOTOR_1 NXT_PORT_B
#define PORT_MOTOR_2 NXT_PORT_C

// Driving command struct, it's used to manipulate the engine
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

// Update the motor driving commands
void change_driving_command(int priority, int speed, int duration) 
{
	dc.priority = priority;
	dc.speed = speed;
	dc.duration = duration;
}

TASK(MotorcontrolTask)
{
	// Lock the resource
	GetResource(dcResource);

	// When there is a positive duration we are supposed to drive. Else idle.
	if (dc.duration > 0) {
		change_driving_command (dc.priority, dc.speed, dc.duration - MOTOR_PERIOD);	
	} else {
		change_driving_command(PRIO_IDLE, 0, 0);
	}

	// Unlock the resource
	ReleaseResource(dcResource);

	// Send current speed to the motors
	ecrobot_set_motor_mode_speed(PORT_MOTOR_1, 1, dc.speed);		
	ecrobot_set_motor_mode_speed(PORT_MOTOR_2, 1, dc.speed);
	
	TerminateTask();
}


TASK(ButtonPressTask)
{
	// When the button is pressed set motors to BACK OFF mode, setting the motors to reverse for 1 second.
	if (ecrobot_get_touch_sensor(PORT_TOUCH)) {
		// Lock the resource
		GetResource(dcResource);
		change_driving_command(PRIO_BUTTON, -50, BACK_OFF_PERIOD);
		// Unlock the resource
		ReleaseResource(dcResource);
	}

	TerminateTask();
}

TASK(DistanceTask)
{
	// Only run when there isn't a more important task running
	if (dc.priority <= PRIO_DIST) {
		S16 distance = ecrobot_get_sonar_sensor(PORT_ULTRA);
		S16 diff = distance - DISTANCE_GOAL;

		// If sonar isn't ready
		if(distance == -1)
			TerminateTask();

		// If we have the perfect distance, do nothing
		if(!diff)
			TerminateTask();

		// Ramping function for speed
		int speed = diff * 100 / 15;

		// Clamp speed
		if(speed > 100)
			speed = 100;
		else if(speed < -100)
			speed = -100;

		// Get lock and update struct
		GetResource(dcResource);
		change_driving_command(PRIO_DIST, speed, 150);
		ReleaseResource(dcResource);
	}

	TerminateTask();
}

TASK(DisplayTask)
{
	// Get lock and print interesting data
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

	ReleaseResource(dcResource);

	display_update();

	TerminateTask();
}