/**\file
 *
 * j1939_struct.h
 *
 * Database variable definitions for J1939 buses.
 *
 * Note that first field for all structures is the time stamp, so it can be
 * altered by routines that do not know the type of the message.
 *
 * @author Abdul Rahman Kreidieh
 * @version 1.0.0
 * @date May 31, 2018
 */

#ifndef INCLUDE_JBUS_J1939_STRUCT_H_
#define INCLUDE_JBUS_J1939_STRUCT_H_

#include <string>
#include "j1939_utils.h"
#include "utils/timestamp.h"
#include "utils/common.h"		/* BYTE */


/** J1939 Protocol Data Unit (PDU) format.
 *
 * This is the format the messages are collected from the CAN card. This object
 * is used for storing information about Protocol Data Units that have been
 * received with a correct sum, with no field translation.
 *
 * See SAE J1939, 3.1.2
 */
typedef struct
{
    timestamp_t timestamp;	/**< time the message was received */
	BIT reserved;			/**< frame bit 5; ID 25; reserved for future use */
	BIT data_page;			/**< frame bit 6; ID 24; 0 currently  */
	int priority;         	/**< priority of message */
	int pdu_format;	    	/**< Protocol Data Unit Format (PF) */
	int pdu_specific;	    /**< PDU Specific (PS) */
	int src_address;	    /**< Source address */
	int data_field[8];		/**< 64 bits maximum */
	int num_bytes;			/**< number of bytes in data_field */
} j1939_pdu_typ;


/** PDU TSC1 (Torque/Speed Control) doc. in J1939 - 71, p149 */
typedef struct {
	timestamp_t timestamp;		/**< time the message was received */
	int ovrd_ctrl_m;			/**< Engine Override Control Mode (0-3) */
	int req_spd_ctrl;			/**< Engine Requested Speed Control Conditions (0-3) */
	int ovrd_ctrl_m_pr;			/**< Override Control Mode Priority (0-3) */
	double req_spd_lim;			/**< Engine Requested Speed/Speed Limit (0 RPM to 8,031.875 RPM) */
	double req_trq_lim;			/**< Engine Requested Torque/Torque Limit (-125% to 125%) */
	int destination_address;	/**< messages are transmitted to the engine or retarder */
	int src_address;			/**< sent in header, important for logging */
} j1939_tsc1_typ;


/** PDU EBC1 (Electronic Brake Controller #1) doc. in J1939 - 71, p151 */
typedef struct {
	timestamp_t timestamp;			/**< time the message was received */
	int asr_engine_ctrl_active;		/**< ASR Engine Control Active (0-3) */
	int asr_brk_ctrl_active;		/**< ASR Brake Control Active (0-3) */
	int antilock_brk_active;		/**< Anti-Lock Braking (ABS) Active (0-3) */
	int ebs_brk_switch;				/**< EBS Brake Switch (0-3) */
	double brk_pedal_pos;			/**< Brake Pedal Position (0-100%) */
	int abs_offroad_switch;			/**< ABS Off-road Switch (0-3) */
	int asr_offroad_switch;			/**< ASR Off-road Switch (0-3) */
	int asr_hillholder_switch;		/**< ASR "Hill Holder" Switch (0-3) */
	int trac_ctrl_override_switch;	/**< Traction Control Override Switch (0-3) */
	int accel_interlock_switch;		/**< Accelerator Interlock Switch (0-3) */
	int eng_derate_switch;			/**< Engine Derate Switch (0-3) */
	int aux_eng_shutdown_switch;	/**< Engine Auxiliary Shutdown Switch (0-3) */
	int accel_enable_switch;		/**< Remote Accelerator Enable Switch (0-3) */
	double eng_retarder_selection;	/**< Engine Retarder Selection (0-100%) */
	int abs_fully_operational;		/**< ABS Fully Operational (0-3) */
	int ebs_red_warning;			/**< EBS Red Warning Signal (0-3) */
	int abs_ebs_amber_warning;		/**< ABS/EBS Amber Warning Signal (Powered Vehicle) (0-3) */
	int src_address_ctrl;			/**< Source Address of Controlling Device for Brake Control (0-255) */
	double total_brk_demand;		/**< Total brake demand */
} j1939_ebc1_typ;


/** PDU EBC2 (Electronic Brake Controller 2) doc. in J1939 - 71, p170 */
typedef struct {
	timestamp_t timestamp;			/**< time the message was received */
	double front_axle_spd;			/**< Front Axle Speed (0 to 250.996 km/h) */
	double rel_spd_front_left;		/**< Relative Speed; Front Axle, Left Wheel (-7.8125 to 7.8125 km/h) */
	double rel_spd_front_right;		/**< Relative Speed; Front Axle, Right Wheel (-7.8125 to 7.8125 km/h) */
	double rel_spd_rear_left_1;		/**< Relative Speed; Rear Axle #1, Left Wheel (-7.8125 to 7.8125 km/h) */
	double rel_spd_rear_right_1;	/**< Relative Speed; Rear Axle #1, Right Wheel (-7.8125 to 7.8125 km/h) */
	double rel_spd_rear_left_2;		/**< Relative Speed; Rear Axle #2, Left Wheel (-7.8125 to 7.8125 km/h) */
	double rel_spd_rear_right_2;	/**< Relative Speed; Rear Axle #2, Right Wheel (-7.8125 to 7.8125 km/h) */
} j1939_ebc2_typ;


/** PDU RF (Retarder Fluids) doc. in J1939 - 71, p164 */
typedef struct {
	timestamp_t timestamp;	/**< time the message was received */
	double pressure;		/**< Hydraulic Retarder Pressure (0-4000 kPa) */
	double oil_temp;		/**< Hydraulic Retarder Oil Temperature (-40 to 210 deg C) */
} j1939_rf_typ;


/** PDU TC1 (Transmission Control) doc. in J1939 - 71, p149 */
typedef struct {
	timestamp_t timestamp;			/**< time the message was received */
	int disengage_driveline;		/**< Disengage Driveline Request (0-3) */
	int trq_conv_lockup_disable;	/**< Transmission Torque Converter Lockup Disable Request (0-3) */
	int gear_shift_inhibit;			/**< Transmission Reverse Gear Shift Inhibit Request (0-3) */
	double pcnt_clutch_slip;		/**< Requested Percent Clutch Slip (0-100%) */
	int req_gear;					/**< Transmission Requested Launch Gear (0-15) */
	int ddl_rear_axle2;				/**< Disengage Differential Lock Request - Rear Axle 2 (0-3) */
	int ddl_rear_axle1;				/**< Disengage Differential Lock Request - Rear Axle 1 (0-3) */
	int ddl_front_axle2;			/**< Disengage Differential Lock Request - Front Axle 2 (0-3) */
	int ddl_front_axle1;			/**< Disengage Differential Lock Request - Front Axle 1 (0-3) */
	int ddl_central_rear;			/**< Disengage Differential Lock Request - Central Rear (0-3) */
	int ddl_central_front;			/**< Disengage Differential Lock Request - Central Front (0-3) */
	int ddl_central;				/**< Disengage Differential Lock Request - Central (0-3) */
} j1939_tc1_typ;


/** PDU ERC1 (Electronic Retarder Controller #1) doc. in J1939 - 71, p150 */
typedef struct {
	timestamp_t timestamp;			/**< time the message was received */
	int trq_mode;					/**< Retarder Torque Mode (0-15) */
	int enable_brake_assist;		/**< Retarder Enable - Brake Assist Switch (0-3) */
	int enable_shift_assist;		/**< Retarder Enable - Shift Assist Switch (0-3) */
	double actual_ret_pcnt_trq;		/**< Actual Retarder - Percent Torque (-125% to 125%) */
	double intended_ret_pcnt_trq;	/**< Intended Retarder - Percent Torque (-125% to 125%) */
	int rq_brake_light;				/**< Retarder Requesting Brake Light (0-3) */
	int src_address_ctrl;			/**< Source Address of Controlling Device for Retarder Control (0-255) */
	int drvrs_demand_prcnt_trq;		/**< Drivers Demand Retarder -  Percent Torque (-125% to 125%) */
	double selection_nonengine;		/**< Retarder Selection, non-engine (0-100%) */
	int max_available_prcnt_trq;	/**< Actual Maximum Available Retarder - Percent Torque (-125% to 125%) */
} j1939_erc1_typ;


/** PDU ETC1 (Elec. Transmission Controller #1) doc. in J1939 - 71, p151 */
typedef struct {
	timestamp_t timestamp;			/**< time the message was received */
	int trans_driveline;			/**< Transmission Driveline Engaged (0-3) */
	int trq_conv_lockup;			/**< Transmission Torque Converter Lockup Engaged (0-3) */
	int trans_shift;				/**< Transmission Shift In Progress (0-3) */
	double tran_output_shaft_spd;	/**< Transmission Output Shaft Speed (0 to 8,031.875 RPM) */
	double prcnt_clutch_slip;		/**< Percent Clutch Slip (0-100%) */
	int eng_overspd_enable;			/**< Engine Momentary Overspeed Enable (0-3) */
	int prog_shift_disable;			/**< Progressive Shift Disable (0-3) */
	double trans_input_shaft_spd;	/**< Transmission Input Shaft Speed (0 to 8,031.875 RPM) */
	int src_address_ctrl;			/**< Source Address of Controlling Device for Transmission Control (0-255) */
} j1939_etc1_typ;


/** PDU EEC1 (Electronic Engine Controller #1) doc. in J1939 - 71, p152 */
typedef struct {
	timestamp_t timestamp;			/**< time the message was received */
	int eng_trq_mode;				/**< Engine Torque Mode (0-15) */
	double drvr_demand_eng_trq;		/**< Driver's Demand Engine - Percent Torque (-125% to 125%) */
	double actual_eng_trq;			/**< Actual Engine - Percent Torque (-125% to 125%) */
	double eng_spd;					/**< Engine Speed (0 to 8,031.875 RPM) */
	double eng_demand_trq;			/**< Engine Demand - Percent Torque (-125% to 125%) */
	int src_address;				/**< not supported by Cummins? */
} j1939_eec1_typ;


/** PDU EEC2 (Electronic Engine Controller #2) doc. in J1939 - 71, p152 */
typedef struct {
	timestamp_t timestamp;			/**< time the message was received */
	int accel_pedal1_idle;			/**< Accelerator Pedal 1 Low Idle Switch (0-3) */
	int accel_pedal_kickdown;		/**< Accelerator Pedal Kickdown Switch (0-3) */
	int spd_limit_status;			/**< Road Speed Limit Status (0-3) */
	int accel_pedal2_idle;			/**< Accelerator Pedal 2 Low Idle Switch (0-3) */
	double accel_pedal1_pos;		/**< Accelerator Pedal Position 1 (0-100%) */
	double eng_prcnt_load_curr_spd;	/**< Engine Percent Load At Current Speed (0-250%) */
	double accel_pedal2_pos;		/**< Accelerator Pedal Position 2 (0-100%) */
	double act_max_avail_eng_trq;	/**< Actual Maximum Available Engine - Percent Torque (0-100%) */
} j1939_eec2_typ;


/** PDU ETC2 (Electronic Transmission Controller #2) doc. in J1939 - 71, p152 */
typedef struct {
	timestamp_t timestamp;			/**< time the message was received */
	int trans_selected_gear;		/**< Transmission Selected Gear (-125 to 125) */
	double trans_act_gear_ratio;	/**< Transmission Actual Gear Ratio (0 to 64.255) */
	int trans_current_gear;			/**< Transmission Current Gear (-125 to 125) */
	int range_selected;				/**< Transmission Requested Range (0 to 255 per byte) */
	int range_attained;				/**< Transmission Current Range (0 to 255 per byte) */
} j1939_etc2_typ;


/** PDU TURBO (Turbocharger) doc. in J1939 - 71, p153 */
typedef struct {
	timestamp_t timestamp;			/**< time the message was received */
	double turbo_lube_oil_pressure;	/**< Engine Turbocharger Lube Oil Pressure 1 (0-1000 kPa) */
	double turbo_speed;				/**< Engine Turbocharger 1 Speed (0-257,020 RPM) */
} j1939_turbo_typ;


/** PDU EEC3 (Electronic Engine Controller #3) doc. in J1939 - 71, p154 */
typedef struct {
	timestamp_t timestamp;			/**< time the message was received */
	double nominal_friction;		/**< Nominal Friction - Percent Torque (-125 to 125 %) */
	double desired_operating_spd;	/**< Engine's Desired Operating Speed (0 to 8,031.875 RPM) */
	int operating_spd_adjust;		/**< Engine's Desired Operating Speed Asymmetry Adjustment (0 to 250) */
	double est_eng_prstic_loss;		/**< Estimated Engine Parasitic Losses - Percent Torque (-125 to 125 %) */
} j1939_eec3_typ;


/** PDU VD (Vehicle Distance) doc. in J1939 - 71, p154 */
typedef struct {
	timestamp_t timestamp;		/**< time the message was received */
	double trip_dist;			/**< Trip Distance (0 to 526,385,151.9 km) */
	double tot_vehicle_dist;	/**< Total Vehicle Distance (0 to 526,385,151.9 km) */
} j1939_vd_typ;


/** PDU RCFG (Retarder Configuration) doc. in J1939 - 71, p155 */
typedef struct {
	timestamp_t timestamp;			/**< time the message was received */
	int retarder_type;				/**< Retarder Type (0-15) */
	int retarder_loc;				/**< Retarder Location (0-15) */
	int retarder_ctrl_steps;		/**< Retarder Control Method (0 to 250 steps) */
	double retarder_speed[5];		/**< Retarder Speed At Points 1-5 (0 to 8,031.875 RPM) */
	double percent_torque[5];		/**< Percent Torque At Idle, Points 1-5 (-125 to 125 %) */
	double reference_retarder_trq;	/**< Reference Retarder Torque (0-64,255 Nm) */
} j1939_rcfg_typ;


#define MAX_FORWARD_GEARS 16
#define MAX_REVERSE_GEARS 8


/** PDU TCFG (Transmission Configuration) doc. in J1939 - 71, p155 */
typedef struct {
	timestamp_t timestamp;						/**< time the message was received */
	int num_rev_gear_ratios;					/**< Number of Reverse Gear Ratios (0-250) */
	int num_fwd_gear_ratios;					/**< Number of Forward Gear Ratios (0-250) */
	double rev_gear_ratios[MAX_REVERSE_GEARS];	/**< Gear Ratios for Reverse Gears */
	double fwd_gear_rations[MAX_FORWARD_GEARS];	/**< Gear Ratios for Forward Gears */
} j1939_tcfg_typ;


/** PDU ECFG (Engine Configuration) doc. in J1939 - 71, p156 */
typedef struct {
	timestamp_t timestamp;				/**< time the message was received */
	double engine_spd[7];				/**< Engine Speed, Points 1-7 (0-8,031.875 RPM) */
	double percent_trq[5];				/**< Engine Percent Torque, Points 1-5 (-125 to 125 %) */
	double gain_endspeed_governor;		/**< Engine Gain (Kp) Of The Endspeed Governor (0-50.2 %/RPM) */
	double reference_eng_trq;			/**< Engine Reference Torque (0-64,255 Nm)*/
	double max_momentary_overide_time;	/**< Engine Maximum Momentary Override Time Limit (0-25 s)*/
	double spd_ctrl_lower_lim;			/**< Engine Requested Speed Control Range Lower Limit (0-2500 RPM)*/
	double spd_ctrl_upper_lim;			/**< Engine Requested Speed Control Range Upper Limit (0-2500 RPM)*/
	double trq_ctrl_lower_lim;			/**< Engine Requested Torque Control Range Lower Limit (-125 to 125 %) */
	double trq_ctrl_upper_lim;			/**< Engine Requested Torque Control Range Upper Limit (-125 to 125 %) */
	int receive_status; 				/**< mask of frames received */
} j1939_ecfg_typ;


/** PDU ETEMP (Engine Temperature) doc. in J1939 - 71, p160 */
typedef struct {
	timestamp_t timestamp;						/**< time the message was received */
	double eng_coolant_temp;					/**< Engine Coolant Temperature (-40 to 210 deg C) */
	double fuel_temp;							/**< Engine Fuel Temperature (-40 to 210 deg C) */
	double eng_oil_temp;						/**< Engine Oil Temperature (-273 to 1735 deg C) */
	double turbo_oil_temp;						/**< Engine Turbocharger Oil Temperature (-273 to 1735 deg C) */
	double eng_intercooler_temp;				/**< Engine Intercooler Temperature (-40 to 210 deg C) */
	double eng_intercooler_thermostat_opening;	/**< Engine Intercooler Thermostat Opening (0-100 %) */
} j1939_etemp_typ;


/** PDU PTO (Power Takeoff Information) doc. in J1939 - 71, p161 */
typedef struct {
	timestamp_t timestamp;			/**< time the message was received */
	double oil_temp;					/**< Power Takeoff Oil Temperature (-40 to 210 deg C) */
	double speed;					/**< Power Takeoff Speed (0-8,031.875 RPM) */
	double set_speed;				/**< Power Takeoff Set Speed (0-8,031.875 RPM) */
	int enable_switch;				/**< Engine PTO Governor Enable Switch (0-3) */
	int remote_preprogramm_status;	/**< Engine Remote PTO Governor Preprogrammed Speed Control Switch (0-3) */
	int remote_variable_spd_status;	/**< Engine Remote PTO Governor Variable Speed Control Switch (0-3) */
	int set_switch;					/**< Engine PTO Governor Set Switch (0-3) */
	int coast_decel_switch;			/**< Engine PTO Governor Coast/Decelerate Switch (0-3) */
	int resume_switch;				/**< Engine PTO Governor Resume Switch (0-3) */
	int accel_switch;				/**< Engine PTO Governor Accelerate Switch (0-3) */
} j1939_pto_typ;


/** PDU CCVS (Cruise Control/Vehicle Speed) doc. in J1939 - 71, p162 */
typedef struct {
	timestamp_t timestamp;		/**< time the message was received */
	int two_spd_axle_switch;	/**< Two Speed Axle Switch (0-3) */
	int parking_brk_switch;		/**< Parking Brake Switch (0-3) */
	int cc_pause_switch;		/**< Cruise Control Pause Switch (0-3) */
	int park_brk_release;		/**< Park Brake Release Inhibit Request (0-3) */
	double vehicle_spd;			/**< Wheel-Based Vehicle Speed (0 to 250.996 km/h) */
	int cc_active;				/**< Cruise Control Active (0-3) */
	int cc_enable_switch;		/**< Cruise Control Enable Switch (0-3) */
	int brk_switch;				/**< Brake Switch (0-3) */
	int clutch_switch;			/**< Clutch Switch (0-3) */
	int cc_set_switch;			/**< Cruise Control Set Switch (0-3) */
	int cc_coast_switch;		/**< Cruise Control Coast (Decelerate) Switch (0-3) */
	int cc_resume_switch;		/**< Cruise Control Resume Switch (0-3) */
	int cc_accel_switch;		/**< Cruise Control Accelerate Switch (0-3) */
	double cc_set_speed;		/**< Cruise Control Set Speed (0-250 km/h) */
	int pto_state;				/**< PTO Governor State (0-31) */
	int cc_state;				/**< Cruise Control States (0-7) */
	int eng_idle_incr_switch;	/**< Engine Idle Increment Switch (0-3) */
	int eng_idle_decr_switch;	/**< Engine Idle Decrement Switch (0-3) */
	int eng_test_mode_switch;	/**< Engine Test Mode Switch (0-3) */
	int eng_shutdown_override;	/**< Engine Shutdown Override Switch (0-3) */
} j1939_ccvs_typ;


/** PDU LFE (Fuel Economy) doc. in J1939 - 71, p162 */
typedef struct {
	timestamp_t timestamp;			/**< time the message was received */
	double eng_fuel_rate;			/**< Engine Fuel Rate (0-3212.75 L/h) */
	double eng_inst_fuel_economy;	/**< Engine Instantaneous Fuel Economy (0-125.5 km/L) */
	double eng_avg_fuel_economy;	/**< Engine Average Fuel Economy (0-125.5 km/L) */
	double eng_throttle1_pos;		/**< Engine Throttle 1 Position (0-100%) */
	double eng_throttle2_pos;		/**< Engine Throttle 2 Position (0-100%) */
} j1939_lfe_typ;


/** PDU AMBC (Ambient Conditions) doc. in J1939 - 71, p163 */
typedef struct {
	timestamp_t timestamp;			/**< time the message was received */
	double barometric_pressure;		/**< Barometric Pressure (0-125 kPa) */
	double cab_interior_temp;		/**< Cab Interior Temperature (-273 to 1735 deg C) */
	double ambient_air_temp;		/**< Ambient Air Temperature (-273 to 1735 deg C) */
	double air_inlet_temp;			/**< Engine Air Inlet Temperature (-40 to 210 deg C) */
	double road_surface_temp;		/**< Road Surface Temperature (-273 to 1735 deg C) */
} j1939_ambc_typ;


/** PDU IEC (Inlet/Exhaust Conditions) doc. in J1939 - 71, p164 */
typedef struct {
	timestamp_t timestamp;					/**< time the message was received */
	double particulate_inlet_pressure;		/**< Engine Diesel Particulate Filter Inlet Pressure (0-125 kPa) */
	double boost_pressure;					/**< Engine Intake Manifold #1 Pressure (0-500 kPa) */
	double intake_manifold_temp;			/**< Engine Intake Manifold 1 Temperature (-40 to 210 deg C) */
	double air_inlet_pressure;				/**< Engine Air Inlet Pressure (0-500 kPa) */
	double air_filter_diff_pressure;		/**< Engine Air Filter 1 Differential Pressure (0-12.5 kPa) */
	double exhaust_gas_temp;				/**< Engine Exhaust Gas Temperature (-273 to 1735 deg C) */
	double coolant_filter_diff_pressure;	/**< Engine Coolant Filter Differential Pressure (0-125 kPa) */
} j1939_iec_typ;


/** PDU VEP (Vehicle Electrical Power) doc. in J1939 - 71, p164 */
typedef struct {
	timestamp_t timestamp;			/**< time the message was received */
	double net_battery_current;		/**< Net Battery Current (-125 to 125 A) */
	double alternator_current;		/**< Alternator Current (0-250 A) */
	double alternator_potential;	/**< Charging System Potential (Voltage) (0-3212.75 V) */
	double electrical_potential;	/**< Battery Potential / Power Input 1 (0-3212.75 V) */
	double battery_potential;		/**< Keyswitch Battery Potential (0-3212.75 V) */
} j1939_vep_typ;


/** PDU TF (Transmission Fluids) doc. in J1939 - 71, p164 */
typedef struct {
	timestamp_t timestamp;	/**< time the message was received */
	double clutch_pressure;	/**< Clutch Pressure (0-4000 kPa) */
	double oil_level;		/**< Transmission Oil Level (0-100%) */
	double diff_pressure;	/**< Transmission Filter Differential Pressure (0-500 kPa) */
	double oil_pressure;	/**< Transmission Oil Pressure (0-4000 kPa) */
	double oil_temp;		/**< Transmission Oil Temperature (-273 to 1735 deg C) */
} j1939_tf_typ;


/** PDU HRVD (High Resolution Vehicle Distance) doc. in J1939 - 71, p170 */
typedef struct {
	timestamp_t timestamp;		/**< time the message was received */
	double vehicle_distance;	/**< High Resolution Total Vehicle Distance (0 to 21,055,406 km) */
	double trip_distance;		/**< High Resolution Trip Distance (0 to 21,055,406 km) */
} j1939_hrvd_typ;


/** PDU EBC5 (Electronic Brake Controller 5)*/
typedef struct {
	timestamp_t timestamp;		/**< time the message was received */
	int halt_brk_mode;			/**< Halt brake mode (0-7) */
	int brk_use;				/**< Foundation Brake Use (0-3) */
	int xbr_active_ctrl_mode;	/**< XBR Active Control Mode (0-15) */
	double xbr_accel_limit;		/**< XBR Acceleration Limit (-12.5 to +12.5 m/s²) */
} j1939_ebc5_typ;


/** PDU VDC2 */
typedef struct {
	timestamp_t timestamp;			/**< time the message was received */
	double steering_wheel_angle;	/**< Steering Wheel Angle (-31.374 to +31.374 rad) */
	int steering_wheel_turn_ctr;	/**< Steering Wheel Turn Counter (-32 to 29 turns) */
	double yaw_rate;				/**< Yaw Rate (-3.92 to +3.92 rad/s) */
	double lat_accel;				/**< Lateral Acceleration (-15.687 to +15.687 m/s2) */
	double long_accel;				/**< Longitudinal Acceleration (-12.5 to +12.5 m/s2) */
} j1939_vdc2_typ;


/** PDU FD (Fan Drive) doc. in J1939 - 71, sec. 5.3.58 */
typedef struct {
	timestamp_t timestamp;	/**< time the message was received */
	double prcnt_fan_spd;	/**< Estimated Percent Fan Speed (0-100%) */
	int fan_drive_state;	/**< Fan Drive State (0-15) */
} j1939_fd_typ;


/** PDU GFI2 (Gaseous Fuel Information 2), J1939-71, sec 5.3.123 */
typedef struct {
	timestamp_t timestamp;		/**< time the message was received */
	double fuel_flow_rate1;		/**< Engine Fuel Flow Rate 1 (0-6425.5 m^3/h) */
	double fuel_flow_rate2;		/**< Engine Fuel Flow Rate 2 (0-6425.5 m^3/h) */
	double fuel_valve_pos1;		/**< Engine Fuel Valve 1 Position (0-100%) */
	double fuel_valve_pos2;		/**< Engine Fuel Valve 2 Position (0-100%) */
} j1939_gfi2_typ;


/** PDU EI (Engine Information), J1939-71, sec 5.3.105 */
typedef struct {
	timestamp_t timestamp;				/**< time the message was received */
	double pre_filter_oil_pressure;     /**< Engine Pre-filter Oil Pressure */
										/**< (0-1000 kPa) */
	double exhaust_gas_pressure;	    /**< Engine Exhaust Gas Pressure */
										/**< (-250 kPa to 251.99 kPa) */
	double rack_position;               /**< Engine Fuel Rack Position (0-100%) */
	double eng_gas_mass_flow;       	/**< Engine Gas Mass Flow Rate 1 (0 */
										/**< to 3212.75 kg/h) */
	double inst_estimated_brake_power;	/**< Instantaneous Estimated Brake */
										/**< Power (0-32127.5 kW) */
} j1939_ei_typ;


/** PDU VOLVO_XBR_WARN (Volvo brake message) */
typedef struct {
	timestamp_t timestamp;		/**< time the message was received */
	BYTE src_address;			/** Source address */
	BYTE destination_address;	/** Destination address */
	BYTE pdu_format;			/**< Protocol Data Unit Format (PF) */
	BYTE byte1;					/**< 0xFF */
	BYTE byte2;					/**< 0x31 */
	BYTE byte3;					/**< 0xFF */
	BYTE byte4;					/**< 0xFF */
	BYTE byte5;					/**< 0xFF */
	BYTE byte6;					/**< 0xFF */
	BYTE byte7;					/**< 0xFF */
	BYTE byte8;					/**< 0xFF */
} j1939_volvo_xbr_warn_typ;


/** PDU VOLVO_XBR (Volvo brake message) */
typedef struct {
	timestamp_t timestamp;				/**< time the message was received */
	double ExternalAccelerationDemand;	//
	BYTE src_address;              		//
	BYTE destination_address;      		//
	BYTE pdu_format;               		//
	BYTE XBREBIMode;	            	//
	BYTE XBRPriority;	            	//
	BYTE XBRControlMode;	        	//
	BYTE XBRUrgency;	            	//
	BYTE spare1;		            	// 0xFF
	BYTE spare2;		            	// 0xFF
	BYTE spare3;		            	// 0xFF
	BYTE XBRMessageCounter;        		//
	BYTE XBRMessageChecksum;       		//
} j1939_volvo_xbr_typ;


// /** PDU ETC2_E (Electronic Transmission Controller #2, source engine) doc. in
//  * J1939 - 71, p152 */
// typedef struct {
// 	timestamp_t timestamp;
// 	char ETC2_E_TransmissionSelectedGear;
// 	float ETC2_E_TransmissionActualGearRatio;
// 	char ETC2_E_TransmissionCurrentGear;
// 	int ETC2_E_TransmissionRangeSelected;
// 	int ETC2_E_TransmissionRangeAttained;
// } j1939_etc2_e_typ;


// /** PDU VP_X_TGW */
// typedef struct {
// 	timestamp_t timestamp;
// 	float VP_X_TGW_Latitude_BB1_X_TGW;
// 	float VP_X_TGW_Longitude_BB1_X_TGW;
// } j1939_vp_x_typ;


// /** PDU VOLVO_TARGET(Volvo target data)*/
// typedef struct {
// 	timestamp_t timestamp;
// 	float TargetDist;	            // 0-655.35 m
// 	float TargetVel;	            // 0-655.35 m/sec
// 	float TargetAcc;	            // -327.68-327.67 m/sec/sec
// 	BYTE TargetAvailable;  // 0=no target, 1=target
// } j1939_volvo_target_typ;


// /** PDU VOLVO_EGO(Volvo self data)*/
// typedef struct {
// 	timestamp_t timestamp;
// 	float EgoVel;	                // 0 to +655.35 m/sec
// 	float EgoAcc;	                // -327.68 to +327.67 m/sec/sec
// 	float EgoRoadGrade;             // -25 to +26 %
// } j1939_volvo_ego_typ;


// /** PDU VBRK (Volvo brake type, Bendix brakes) */
// typedef struct {
// 	timestamp_t timestamp;
//     float VBRK_BrkAppPressure;
//     float VBRK_BrkPrimaryPressure;
//     float VBRK_BrkSecondaryPressure;
//     BYTE VBRK_BrkStatParkBrkActuator;
//     BYTE VBRK_ParkBrkRedWarningSignal;
//     BYTE VBRK_ParkBrkReleaseInhibitStat;
// } j1939_volvo_brk_t;


#endif /* INCLUDE_JBUS_J1939_STRUCT_H_ */
