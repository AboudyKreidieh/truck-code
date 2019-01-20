///**\file
// *
// * long_utils.h
// *
// * This file contains utility structures and loose methods used during the
// * longitudonal control process.
// *
// * @author Abdul Rahman Kreidieh
// * @version 1.0.0
// * @date July 11, 2018
// */
//
//#ifndef SRC_CONTROL_LONG_UTILS_H_
//#define SRC_CONTROL_LONG_UTILS_H_
//
//
///** Longitudinal control structure
// *
// * long_vehicle_state includes description of a vehicle's current state
// * as read from the database, or transmitted by vehicle-to-vehicle
// * communication from another vehicle.
// */
//typedef struct {
//	/* long_input_typ data */
//	float acc_pedal_voltage;		// blank
//	float fb_applied;				// blank
//	float rb_applied;				// blank
//	float fb_monitor;				// blank
//	float rb_monitor;				// blank
//	float fb_axle;					// blank
//	float mb_axle;					// blank
//	float rb_axle;					// blank
//	float trans_retarder_voltage;	// blank
//
//	/* lat_input_sensors_typ data */
//	float long_accel;	// blank
//	float lat_accel;	// blank
//
//	/* j1939 bus data */
//	float vehicle_speed;					// from engine
//	float engine_speed;						// from engine
//	float engine_torque;					// from engine
//	float accelerator_pedal_position;		// from engine
//	int engine_mode;						// from engine
//	float retarder_torque;					// from engine retarder
//	int retarder_mode;						// from engine retarder
//	unsigned char eng_rtdr_src;				// from engine retarder
//	int ebs_brake_switch_status;			// from EBS
//	int abs_ebs_amber_warning_state;		// from EBS
//	float brake_demand;						// from EBS
//	float brake_pedal_position;				// from EBS
//	float input_shaft_speed;				// from transmission
//	float output_shaft_speed;				// from transmission
//	int shift_in_progress;					// from transmission
//	int lockup_engaged;						// from transmission
//	int driveline_engaged;					// from transmission
//	int current_gear;						// from transmission
//	int selected_gear;						// from transmission
//	float actual_gear_ratio;				// from transmission
//	float fuel_rate;						// from engine
//	int fan_drive_state;					// from engine
//	float nominal_friction_torque; 			// from engine
//	float actual_retarder_percent_torque;	//
//	float boost_pressure;					//
//	float estimated_percent_fan_speed;		//
//	float percent_load_current_speed;		//
//	float fuel_flow_rate1;					//
//	float fuel_flow_rate2;					//
//	float fuel_valve1_position;				//
//	float fuel_valve2_position;				//
//	float exhaust_gas_pressure;				//
//	float rack_position;					//
//	float natural_gas_mass_flow;			//
//	float instantaneous_brake_power;		//
//	float front_axle_speed;	    			// from ebc2 on D1
//	float front_left_wheel_relative;  		// from ebc2 on D1
//	float front_right_wheel_relative; 		// from ebc2 on D1
//	float rear1_left_wheel_relative;  		// from ebc2 on D1
//	float rear1_right_wheel_relative; 		// from ebc2 on D1
//	float trans_retarder_value;				//
//	float trans_retarder_mode;				//
//	float coolant_load_increase;			//
//	unsigned char trans_retarder_source;	//
//
//	/* other data streams */
//	evt300_radar_typ evt300;		// blank
//	long_lidarA_typ lidarA;			// blank
//	long_lidarB_typ lidarB;			// blank
//	mdl_lidar_typ mdl_lidar;		// blank
//	path_gps_point_t self_gps;		// self GPS information
//	veh_comm_packet_t lead_trk;		// packet from LEAD vehicle
//	veh_comm_packet_t second_trk;	// packet from SECOND vehicle
//	veh_comm_packet_t third_trk;	// packet from THIRD vehicle
//	long_dig_in_typ dig_in;			// switches digital in
//} long_vehicle_state;
//
//
//#endif /* SRC_CONTROL_LONG_UTILS_H_ */
