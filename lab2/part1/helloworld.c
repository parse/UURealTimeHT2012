#include <stdlib.h>
#include "kernel.h"
#include "kernel_id.h"
#include "ecrobot_interface.h"

// We need a task to display the hello world and our light readings
DeclareTask(HelloworldTask);
DeclareCounter(SysTimerCnt);

// Activate light sensor
void ecrobot_device_initialize() {
	ecrobot_set_light_sensor_active(NXT_PORT_S2);
}

// Deactivate light sensor
void ecrobot_device_terminate() {
	ecrobot_set_light_sensor_inactive(NXT_PORT_S2);
}

void user_1ms_isr_type2 () { SignalCounter ( SysTimerCnt ) ; }

// Clear display and print the light reading every 100ms
TASK(HelloworldTask)
{
	display_clear(0);
	display_goto_xy(0,0);
	display_string("Hello World ");
	display_update();
	int i = ecrobot_get_light_sensor(NXT_PORT_S2);
	display_goto_xy(0,1);
	display_int(i, 3);	

    TerminateTask();
}