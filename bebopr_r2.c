
#include <stdio.h>

#include "analog.h"
#include "temp.h"
#include "beaglebone.h"
#include "thermistor.h"
#include "bebopr.h"
#include "heater.h"
#include "pwm.h"

/*
 * Here one defines where the kernel puts the analog inputs,
 * this happens to change from kernel to kernel :-(
 */
//#define AIN_PATH_PREFIX "/sys/devices/platform/tsc/"		/* kernel 3.2.0 */
#define AIN_PATH_PREFIX "/sys/devices/platform/omap/tsc/"	/* kernel 3.2.16 */

#define PWM_PATH_PREFIX "/sys/class/pwm/"

/*
 * Note, for the easy of implementation, the string addresses are used.
 * This means one cannot use identical strings, but must use pointers
 * to the one and only string!
 */
//#define GENERATE_TAG( name) static const char name[] = #name
#define GENERATE_TAG( name) static const char name[] = #name
GENERATE_TAG( bed_thermistor);
GENERATE_TAG( extruder_thermistor);
GENERATE_TAG( spare_ain);
GENERATE_TAG( temp_extruder);
GENERATE_TAG( temp_bed);
GENERATE_TAG( heater_extruder);
GENERATE_TAG( heater_bed);
GENERATE_TAG( pwm_extruder);
GENERATE_TAG( pwm_bed);
GENERATE_TAG( pwm_fan);

static const analog_config_record analog_config_data[] = {
  {
    .tag                = bed_thermistor,
    .device_path	= AIN_PATH_PREFIX "ain2",	// BEBOPR_R2_J6 - THRM0 (hardware ain1)
    .filter_length	= 0,
  },
  {
    .tag                = spare_ain,
    .device_path	= AIN_PATH_PREFIX "ain4",	// BEBOPR_R2_J7 - THRM1 (hardware ain3)
    .filter_length	= 10,
  },
  {
    .tag                = extruder_thermistor,
    .device_path	= AIN_PATH_PREFIX "ain6",	// BEBOPR_R2_J8 - THRM2 (hardware ain5)
    .filter_length	= 50,
  },
};

static const temp_config_record temp_config_data[] = {
  {
    .tag                = temp_extruder,
    .source		= extruder_thermistor,
    .in_range_time	= 5000,
    .conversion		= bone_epcos_b5760g104f,
  },
  {
    .tag                = temp_bed,
    .source		= bed_thermistor,
    .in_range_time	= 5000,
    .conversion		= bone_thermistor_100k,
  },
};

static const heater_config_record heater_config_data[] = {
  {
    .tag		= heater_extruder,
    .analog_input	= temp_extruder,
    .analog_output	= pwm_extruder,
    .pid =
    {
	    .K = 0.0,
	    .P = 15.0,
	    .I = 0.0,
	    .D = 0.0,
	    .I_limit = 12.0,
    },
  },
  {
    .tag		= heater_bed,
    .analog_input	= temp_bed,
    .analog_output	= pwm_bed,
    .pid =
    {
	    .K = 0.0,
	    .P = 1.0,
	    .I = 0.0,
	    .D = 0.0,
	    .I_limit = 0.0,
    },
  },
};

int bebopr_pre_init( void)
{
  int result = -1;
  fprintf( stderr, "<bebopr_pre_init>");

  result = analog_config( analog_config_data, NR_ITEMS( analog_config_data));
  if (result < 0) {
    fprintf( stderr, "analog_config failed!\n");
    goto done;
  }
  result = temp_config( temp_config_data, NR_ITEMS( temp_config_data));
  if (result < 0) {
    fprintf( stderr, "temp_config failed!\n");
    goto done;
  }
  result = heater_config( heater_config_data, NR_ITEMS( heater_config_data));
  if (result < 0) {
    fprintf( stderr, "heater_config failed!\n");
    goto done;
  }
 done:
  return result;
}

// Limit switches present in the system.
// only return 0 or 1
int limsw_x_has_max( void) { return 0; }
int limsw_x_has_min( void) { return 1; }
int limsw_y_has_max( void) { return 0; }
int limsw_y_has_min( void) { return 1; }
int limsw_z_has_max( void) { return 1; }
int limsw_z_has_min( void) { return 1; }

// Limit switch polarity, only return 0 or 1
// input has inverter! led on = 1, led off = 0
// If the LED turns off activating the switch,
// the switch is active low and vice versa.
int limsw_x_max_is_active_low( void) { return 0; }
int limsw_x_min_is_active_low( void) { return 1; }
int limsw_y_max_is_active_low( void) { return 0; }
int limsw_y_min_is_active_low( void) { return 1; }
int limsw_z_max_is_active_low( void) { return 1; }
int limsw_z_min_is_active_low( void) { return 0; }

int use_pololu_drivers( void) { return 0; }
