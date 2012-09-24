#include <stdlib.h>
#include "kernel.h"
#include "kernel_id.h"
#include "ecrobot_interface.h"

DeclareEvent(TouchOnEvent);
DeclareEvent(TableEdgeEvent);
DeclareEvent(TouchOffEvent);

DeclareTask(MotorcontrolTask);
DeclareTask(ButtonPressTask);
DeclareCounter ( SysTimerCnt ) ;

void ecrobot_device_initialize() {
	ecrobot_set_light_sensor_active(NXT_PORT_S2);
}
void ecrobot_device_terminate() {
	ecrobot_set_motor_speed(NXT_PORT_B, 0);
    ecrobot_set_motor_speed(NXT_PORT_C, 0);
    ecrobot_set_light_sensor_inactive(NXT_PORT_S2);
}

void user_1ms_isr_type2 () { SignalCounter ( SysTimerCnt ) ; }

TASK(MotorcontrolTask)
{

	while (1) {
		EventMaskType eventMask = 0;

		WaitEvent(TouchOffEvent | TouchOnEvent | TableEdgeEvent);
		GetEvent(MotorcontrolTask, &eventMask);

		if (eventMask & TouchOnEvent) {
			display_clear(0);
			display_goto_xy(0,0);
			display_string("TouchOnEvent");
			display_update();

			ecrobot_set_motor_speed(NXT_PORT_B, 45);
	    	ecrobot_set_motor_speed(NXT_PORT_C, 45);
	    	ClearEvent(TouchOnEvent);

		} else if (eventMask & TouchOffEvent) {
			display_clear(0);
			display_goto_xy(0,0);
			display_string("TouchOffEvent");
			display_update();

			ecrobot_set_motor_speed(NXT_PORT_B, 0);
	    	ecrobot_set_motor_speed(NXT_PORT_C, 0);
	    	ClearEvent(TouchOffEvent);
		} else if (eventMask & TableEdgeEvent) {
			ecrobot_set_motor_speed(NXT_PORT_B, 0);
	    	ecrobot_set_motor_speed(NXT_PORT_C, 0);
	    	ClearEvent(TableEdgeEvent);
		}

	}
}

int prevState = 0;
int maxBrightness = 0;
int minBrightness = 0;

TASK(ButtonPressTask)
{
	int touch = ecrobot_get_touch_sensor(NXT_PORT_S1);
	int light = ecrobot_get_light_sensor(NXT_PORT_S2);

	if (light > 600) {
		SetEvent(MotorcontrolTask, TableEdgeEvent);
	}

	if (touch != prevState) {
		if (touch) {

			prevState = touch;
			SetEvent(MotorcontrolTask, TouchOnEvent);

		} else {

			prevState = touch;
			SetEvent(MotorcontrolTask, TouchOffEvent);
		}

	}
	TerminateTask();
}
