#include <stdlib.h>
#include "kernel.h"
#include "kernel_id.h"
#include "ecrobot_interface.h"

#define PRIO_IDLE 10
#define PRIO_BUTTON 20
#define MOTOR_PERIOD 50
#define BACK_OFF_PERIOD 1000

#define PORT_TOUCH NXT_PORT_S1
#define PORT_LIGHT NXT_PORT_S2
#define PORT_ULTRA NXT_PORT_S3
#define PORT_MOTOR_1 NXT_PORT_B
#define PORT_MOTOR_2 NXT_PORT_C

struct dc_t {
    U32 duration;
    S32 speed;
    int priority;
} dc = {0, 0, PRIO_IDLE};

DeclareEvent(TouchOnEvent);
DeclareEvent(TableEdgeEvent);
DeclareEvent(TouchOffEvent);

DeclareTask(MotorcontrolTask);
DeclareTask(ButtonPressTask);
DeclareTask(DisplayTask);

DeclareCounter ( SysTimerCnt ) ;

void ecrobot_device_initialize() 
{
	ecrobot_set_light_sensor_active(PORT_LIGHT);
}

void ecrobot_device_terminate() 
{
	ecrobot_set_motor_speed(PORT_MOTOR_1, 0);
    ecrobot_set_motor_speed(PORT_MOTOR_2, 0);
    ecrobot_set_light_sensor_inactive(PORT_LIGHT);
}

void user_1ms_isr_type2 () 
{ 
	SignalCounter(SysTimerCnt);
}

void change_driving_command(int priority, int speed, int duration) 
{
	dc.priority = priority;
	dc.speed = speed;
	dc.duration = duration;
}

TASK(MotorcontrolTask)
{
	if (dc.duration > 0) {
		ecrobot_set_motor_speed(PORT_MOTOR_1, dc.speed);		
		ecrobot_set_motor_speed(PORT_MOTOR_2, dc.speed);
		change_driving_command (dc.priority, dc.speed, dc.duration - MOTOR_PERIOD);	
	} else {
		change_driving_command(PRIO_IDLE, 0, 0);
	}
	
	TerminateTask();
}

int prevState = 0;

TASK(ButtonPressTask)
{
	int touch = ecrobot_get_touch_sensor(PORT_TOUCH);
	if (touch != prevState) {
		if (touch) {
			change_driving_command(PRIO_BUTTON, -50, BACK_OFF_PERIOD);
		}
	}
	TerminateTask();
}

TASK(DisplayTask)
{

}