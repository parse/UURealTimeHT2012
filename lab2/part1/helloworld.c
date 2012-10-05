#include <stdlib.h>
#include "kernel.h"
#include "kernel_id.h"
#include "ecrobot_interface.h"

DeclareTask(HelloworldTask);
DeclareCounter ( SysTimerCnt ) ;

// Further, you need to define three OSEK hooks:

void ecrobot_device_initialize() {
	ecrobot_set_light_sensor_active(NXT_PORT_S2);
}
void ecrobot_device_terminate() {
	ecrobot_set_light_sensor_inactive(NXT_PORT_S2);
}

void user_1ms_isr_type2 () { SignalCounter ( SysTimerCnt ) ; }

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