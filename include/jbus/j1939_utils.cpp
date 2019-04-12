/**\file
 *
 * j1939_utls.cpp
 *
 * @author Abdul Rahman Kreidieh
 * @version 1.0.0
 * @date June 21, 2018
 */

#include "j1939_utils.h"
#include "utils/common.h"	/* BYTE */


float percent_0_to_100(int data) {
	if (data <= 250)
		return data * 0.4;
	else
		return 0.0 - data;
}

float percent_0_to_250(int data) {
	if (data <= 250)
		return data * 1.0;
	else
		return 0.0 - data;
}

float percent_m125_to_p125(int data) {
	if (data <= 250)
		return data - 125.0;
	else
		return 0.0 - data;
}

int gear_m125_to_p125(int data) {
	int val = data;

	if (val <= 250)
		return val - 125;
	else if (val == 251)
		return 251;
	else
		return(0 - val);
}

float gear_ratio(int data) {
	if (HIBYTE(data) <= 250)
		return data * 0.001;
	else
		return 0.0 - HIBYTE(data);
}

float pressure_0_to_4000kpa(int data) {
	if (data <= 250)
		return data * 16.0;
	else
		return 0.0 - data;
}

float pressure_0_to_1000kpa(int data) {
	if (data <= 250)
		return (data * 4.0);
	else
		return(0.0 - data);
}

float pressure_0_to_500kpa(int data) {
	if (data <= 250)
		return (data * 2.0);
	else
		return(0.0 - data);
}

float pressure_0_to_125kpa(int data) {
	if (data <= 250)
		return (data * 0.5);
	else
		return(0.0 - data);
}

float pressure_0_to_12kpa(int data) {
	if (data <= 250)
		return (data * 0.05);
	else
		return(0.0 - data);
}

float pressure_m250_to_p252kpa(int data) {
	if (HIBYTE(data) <= 250)
		return (data/128 - 250.0);
	else
		return(0.0 - data);
}

float rotor_speed_in_rpm(unsigned short data) {
	if (HIBYTE(data) <= 250)
		return (data * 4.0);
	else
		return(0.0 - HIBYTE(data));
}

float distance_in_km(unsigned int data) {
	if (BYTE3(data) <= 250)
		return (data * 0.125);
	else
		return(0.0 - BYTE3(data));
}

float hr_distance_in_km(unsigned int data) {
	if (BYTE3(data) <= 250)
    	return data * 0.005;
	else
		return 0.0 - BYTE3(data);
}

float speed_in_rpm_1byte(int data) {
	if (data <= 250)
    	return data * 10.0;
	else
		return 0.0 - data;
}

float speed_in_rpm_2byte(int data) {
	if (HIBYTE(data) <= 250)
    	return data * 0.125;
	else
		return 0.0 - HIBYTE(data);
}

float wheel_based_mps(int data) {
	if (HIBYTE(data) <= 250)
		return (HIBYTE(data) + LOBYTE(data)/256.0) * (1000.0/3600.0);
	else
		return 0.0 - HIBYTE(data);
}

float wheel_based_mps_relative(int data) {
	if (data <= 250)
    	return (data * 0.0625 - 7.8125) * (1000.0/3600.0);
	else
		return 0.0 - data;
}

float cruise_control_set_meters_per_sec(int data) {
	if (data <= 250)
		return data * (1000.0/3600.0);
	else
		return 0.0 - data;
}

float fuel_rate_cm3_per_sec(int data) {
	if (HIBYTE(data) <= 250)
    	return data * 0.05 * 1000.0/3600.0;
	else
    	return 0.0 - HIBYTE(data);
}

float fuel_economy_meters_per_cm3(int data) {
	if (HIBYTE(data) <= 250)
    	return data * (1.0/512.0);
	else
		return 0.0 - HIBYTE(data);
}

float torque_in_nm(unsigned short data) {
	if (HIBYTE(data) <= 250)
    	return data * (1.0);
	else
    	return 0.0 - HIBYTE(data);
}

float time_0_to_25sec(BYTE data) {
	if (data <= 250)
		return data * (1.0/10.0);
    else
    	return 0.0 - data;
}

float gain_in_kp(int data) {
	if (HIBYTE(data) <= 250)
    	return data * (1.0/1280.0);
	else
    	return 0.0 - HIBYTE(data);
}

float temp_m40_to_p210(int data) {
	if (data <= 250)
    	return data - 40.0;
	else
    	return 0.0 - data;
}

float temp_m273_to_p1735(int data) {
	if (HIBYTE(data) <= 250)
    	return data * 0.03125 - 273.0;
	else
    	return 0.0 - 10.0*HIBYTE(data);
}

float current_m125_to_p125amp(int data) {
	if (data <= 250)
    	return data - 125.0;
	else
    	return 0.0 - data;
}

float current_0_to_250amp(int data) {
	if (data <= 250)
    	return data - 0.0;
	else
    	return 0.0 - data;
}

float voltage(int data) {
	if (HIBYTE(data) <= 250)
    	return data * 0.05;
	else
		return 0.0 - HIBYTE(data);
}

float brake_demand(int data) {
	if (data <= 250)
    	return data * 0.04 - 10.0;
    else
		return data;
}

float mass_flow(int data) {
	if (HIBYTE(data) <= 250)
    	return data * 0.05;
    else
		return 0.0 - HIBYTE(data);
}

float power_in_kw(int data) {
	if (HIBYTE(data) <= 250)
    	return data * 0.5;
	else
    	return 0.0 - HIBYTE(data);
}
