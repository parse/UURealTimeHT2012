#include <stdlib.h>
#include "kernel.h"
#include "kernel_id.h"
#include "ecrobot_interface.h"

DeclareEvent(TouchOnEvent);
DeclareEvent(TableEdgeEvent);
DeclareEvent(TouchOffEvent);

DeclareTask(MotorcontrolTask);
DeclareTask(EventdispatcherTask);
DeclareCounter(SysTimerCnt);

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

		// The motorcontrol task waits for events.
		WaitEvent(TouchOffEvent | TouchOnEvent | TableEdgeEvent);
		GetEvent(MotorcontrolTask, &eventMask);

		// Check what kind of event was received
		if (eventMask & TouchOnEvent) {
			// Clear the display and indicate that we are using the button
			// Set motor speed to running
			display_clear(0);
			display_goto_xy(0,0);
			display_string("TouchOnEvent");
			display_update();

			ecrobot_set_motor_speed(NXT_PORT_B, 45);
	    	ecrobot_set_motor_speed(NXT_PORT_C, 45);
	    	ClearEvent(TouchOnEvent);

		} else if (eventMask & TouchOffEvent) {
			// Clear the display and indicate that we aren't using the button
			// Turn off the the motors
			display_clear(0);
			display_goto_xy(0,0);
			display_string("TouchOffEvent");
			display_update();

			ecrobot_set_motor_speed(NXT_PORT_B, 0);
	    	ecrobot_set_motor_speed(NXT_PORT_C, 0);
	    	ClearEvent(TouchOffEvent);
		} else if (eventMask & TableEdgeEvent) {
			// We hit the table edge, therefore stop the engines
			ecrobot_set_motor_speed(NXT_PORT_B, 0);
	    	ecrobot_set_motor_speed(NXT_PORT_C, 0);
	    	ClearEvent(TableEdgeEvent);
		}

	}
}

TASK(EventdispatcherTask)
{
	static int prevState = 0;
	int touch = ecrobot_get_touch_sensor(NXT_PORT_S4);
	int light = ecrobot_get_light_sensor(NXT_PORT_S2);

	// If the light reading is above 600 we consider it to be dark.
	if (light > 600) {
		SetEvent(MotorcontrolTask, TableEdgeEvent);
	}

	// If the touch reading is different from last call trigger the touch events.
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
