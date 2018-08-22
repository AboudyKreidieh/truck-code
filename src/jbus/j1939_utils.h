/**\file
 *
 * j1939_utils.h
 *
 * This file contains several constants, macros, and methods that are used when
 * trying to interpret a J1939 message.
 *
 * @author Abdul Rahman Kreidieh
 * @version 1.0.0
 * @date May 31, 2018
 */

#ifndef INCLUDE_JBUS_J1939_UTILS_H_
#define INCLUDE_JBUS_J1939_UTILS_H_

#include <math.h>
#include <stdio.h>


/** Parameter group numbers for supported messages (pdu_format, pdu_specific) */

#define PDU		0x00ff	// (0, 255) sample undefined parameter group number
#define TSC1	0x0000	// (0, 0) Torque Speed Control 1, destination 0
#define EXAC	0x000b	// (0, 11) EXAC (WABCO proprietary)
#define RQST	0xea00	// (234, 0) request transmission of a particular PGN
#define ERC1	0xf000	// (240, 0) electronic retarder controller 1
#define EBC1	0xf001	// (240, 1) electronic brake controller 1
#define ETC1	0xf002	// (240, 2) electronic transmission controller 1
#define EEC2	0xf003  // (240, 3) electronic engine controller 2
#define EEC1	0xf004	// (240, 4) electronic engine controller 1
#define ETC2	0xf005	// (240, 5) electronic transmission controller 1
#define GFI2	0xfe81	// (254, 129) gaseous fuel information 2
#define EI		0xfe92	// (254, 146) engine information
#define FD		0xfebd	// (254, 189) fan drive
#define EBC2	0xfebf	// (254, 191) electronic brake controller 2
#define HRVD	0xfec1	// (254, 193) high resolution vehicle distance
#define TURBO	0xfedd	// (254, 221) turbocharger
#define EEC3	0xfedf	// (254, 223) electronic engine controller 3
#define VD		0xfee0	// (254, 224) vehicle distance
#define RCFG	0xfee1	// (254, 225) retarder configuration
#define TCFG	0xfee2	// (254, 226) transmission configuration
#define ECFG	0xfee3	// (254, 227) engine configuration
#define ETEMP	0xfeee	// (254, 238) engine temperature
#define PTO		0xfef0	// (254, 240) power takeoff
#define CCVS	0xfef1	// (254, 241) cruise control vehicle speed
#define LFE		0xfef2	// (254, 242) fuel economy
#define AMBC	0xfef5	// (254, 245) ambient conditions
#define IEC		0xfef6	// (254, 246) inlet/exhaust conditions
#define VEP		0xfef7	// (254, 247) vehicle electric power
#define TF		0xfef8	// (254, 248) transmission fluids
#define RF		0xfefb	// (254, 251) retarder fluids
#define EBC_ACC	0xff80	// (255, 128) EBC to ACC WABCO proprietary
#define TPDT	0xebff	// (235, 255) transfer protocol data transfer, global
#define TPCM	0xecff	// (236, 255) transfer protocol connection message, global


/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */


/** General purpose macros for extracting fields.
 * Bit number in these macros for bytes follows convention in J1939 docs */

/** Macros for short (two bit) values */
#define BITS87(x)		(((x) & 0xc0) >> 6)
#define BITS65(x)		(((x) & 0x30) >> 4)
#define BITS43(x)		(((x) & 0x0c) >> 2)
#define BITS21(x)		((x) & 0x03)
#define HINIBBLE(x)		(((x) & 0xf0) >> 4)
#define LONIBBLE(x)		((x) & 0x0f)

/** Macros for short (two byte) values */
#define HIBYTE(x)		(((x) & 0xff00) >> 8)
#define LOBYTE(x)		((x) & 0xff)
#define TWOBYTES(x, y)	((((x) & 0xff) << 8) | (y & 0xff))

/** Macros for int (four byte) values */
#define BYTE0(x)	(((x) & 0xff))
#define BYTE1(x)	(((x) & 0xff00) >> 8)
#define BYTE2(x)	(((x) & 0xff0000) >> 16)
#define BYTE3(x)	(((x) & 0xff000000) >> 24)
#define FOURBYTES(a3, a2, a1, a0)   \
			(((a3) & 0xff) << 24) | \
			(((a2) & 0xff) << 16) | \
			(((a1) & 0xff) << 8)  | \
			((a0) & 0xff)


/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */


/** Utility functions used when processing raw bit/byte data into message-
 * specific units, as specified by Lane Assist Interface documentation.
 *
 * Routines return out of range values to indicate errors. Application programs
 * are responsible for checking that values are in range.
 */


/** Values from 251 to 255 in high byte indicate errors, returned as increment
 * to out-of-range value 1500.0. Range from 0 to 2,880,000 corresponds to
 * -1440.000 to +1440.000. Other values should never be sent. */
extern float scale_m1440_to_p1440(unsigned int);

/** Scale from -2.000 to 2.000 (used for ymeas and ycar) */
extern float scale_m2_to_p2(unsigned int);

/** Scale from -1.00000 to 1.00000 (used for the gyro rate) */
extern float scale_m1_to_p1(unsigned int);

/** Scale from -200 to 200 (used for lat_pos) */
extern int scale_m200_to_p200(unsigned int);

/** Scale from -720 to 720 (used for lat_est) */
extern int scale_m720_to_p720(unsigned int);

/** Range from 5000 to 45000 corresponds to 0.5 to 4.5 volts. Hibyte values from
 * 251 to 255 indicate errors, returned as negative. */
extern float scale_10000_to_volt(unsigned int);

/** Range from 0 to 1000 corresponds to 0 to 10 volts. Hibyte values from 251 to
 * 255 indicate errors, returned as negative. */
extern float scale_100_to_volt(unsigned int);

/** Scale by 100, used for mag_dist */
extern float scale_by_100(unsigned int);

/** Scale from msec/4 to sec (used for delta_timer_obs) */
extern float scale_to_sec(unsigned int);

/** Values from 251 to 255 indicate errors, stored as negative. Range from 0 to
 * 250 corresponds to 0 to 100% range of return values */
extern float percent_0_to_100(int);

/** Values from 251 to 255 indicate errors, stored as negative. Range from 0 to
 * 250 corresponds to 0 to 250% range of return values */
extern float percent_0_to_250(int);

/** Values from 251 to 255 indicate errors, stored as negative. Range from 0 to
 * 250 corresponds to -125 to 125% range of return values */
extern float percent_m25_to_p25(unsigned char);

/** Values from 251 to 255 indicate errors, stored as negative. Range from 0 to
 * 250 corresponds to -125 to 125% range of return values */
extern float percent_m125_to_p125(int);

/** Reverse routine used when coding up a percent to send */
extern unsigned char code_percent_m125_to_p125(float);

/** Values of 251 indicates park. Values 252 to 255 indicate errors, returned
 * as negative.  Range from 0 to 250 corresponds to -125 to 125 range of return
 * values */
int gear_m125_to_p125(int);

/** Hibyte values from 251 to 255 indicate errors, stored as negative. Range
 * from 0 to 64255 corresponds to 0 to 64.255 range of return values */
extern float gear_ratio(int);

/** Values from 251 to 255 indicate errors, stored as negative. Range from 0 to
 * 250 corresponds to 0 to 4000 kilopascals pressure */
extern float pressure_0_to_4000kpa(int);

/** Values from 251 to 255 indicate errors, stored as negative. Range from 0 to
 * 250 corresponds to 0 to 1000 kilopascals pressure */
extern float pressure_0_to_1000kpa(int);

/** Values from 251 to 255 indicate errors, stored as negative. Range from 0 to
 * 250 corresponds to 0 to 500 kilopascals pressure */
extern float pressure_0_to_500kpa(int);

/** Values from 251 to 255 indicate errors, stored as negative. Range from 0 to
 * 250 corresponds to 0 to 125 kilopascals pressure */
extern float pressure_0_to_125kpa(int);

/** Values from 251 to 255 indicate errors, stored as negative. Range from 0 to
 * 250 corresponds to 0 to 12.5 kilopascals pressure */
extern float pressure_0_to_12kpa(int);

/** Hibyte values from 251 to 255 indicate errors, stored as negative. Range
 * from 0 to 64255 corresponds to -250 to 251.96 kilopascals pressure */
extern float pressure_m250_to_p252kpa(int);

/** Hibyte values from 251 to 255 indicate errors, stored as negative. Range
 * from 0 to 64255 corresponds to 0 to 257,020 range of return values */
extern float rotor_speed_in_rpm(unsigned short);

/** Upper byte values from 251 to 255 indicate errors, stored as negative.
 * Range from 0 to 4211081215 corresponds to 0 to 526,385,151.875 km */
extern float distance_in_km(unsigned int);

/** Upper byte values from 251 to 255 indicate errors, stored as negative.
 * range from 0 to 4,211,081,215 corresponds to 0 to 21,055,406.075 km */
extern float hr_distance_in_km(unsigned int);

/** Values from 251 to 255 indicate errors, stored as negative. Range from 0 to
 * 250 corresponds to 0 to 2500 range of return values */
extern float speed_in_rpm_1byte(int);

/** Values of upper byte from 251 to 255 indicate errors, returned as negative.
 * Range from 0 to 64255 corresponds to 0 to 8031.875 range of return values */
extern float speed_in_rpm_2byte(int);

/** Reverse routine used to code a float into a 2-byte engine speed for
 * transmission. */
extern unsigned short code_engine_speed(float);

/** Values of upper byte, 0 to 250, in km/h; values of lower byte, 0 to 255.
 * Scaled by 1/256 to give fraction of km/h. Converted to m/sec to agree with
 * units used by AVCS programs. Values of upper byte from 251 to 255 indicate
 * errors, returned as negative. Range from 0 to 69.721 meters/sec. */
extern float wheel_based_mps(int);

/** Values from 251 to 255 indicate errors, stored as negative. Range from 0 to
 * 250 corresponds to -2.170 to +2.170 meters/sec */
extern float wheel_based_mps_relative(int);

/** Values of byte, 0 to 250, in km/h; donverted to m/sec to agree with units
 * used by AVCS programs. Values of byte from 251 to 255 indicate errors,
 * returned as negative. */
extern float cruise_control_set_meters_per_sec(int);

/** Values of upper byte from 251 to 255 indicate errors, returned as negative.
 * Range from 0 to 64255 corresponds to 0 to 3212.75 liters/hour or 0 to 892.430
 * cubic centimeters per second. */
extern float fuel_rate_cm3_per_sec(int);

/** Values of upper byte from 251 to 255 indicate errors, returned as negative.
 * Range from 0 to 64555 corresponds to 0 to 128.498 meters/cubic centimeter */
extern float fuel_economy_meters_per_cm3(int);

/** Values of upper byte from 251 to 255 indicate errors, returned as negative.
 * Range from 0 to 64255 Nm */
extern float torque_in_nm(unsigned short);

/** Values of byte from 251 to 255 indicate errors, returned as negative. Range
 * from 0 to 250 scale to 0 to 25 seconds. */
extern float time_0_to_25sec(unsigned char);

/** Values of byte from 251 to 255 indicate errors, returned as negative. Range
 * of 0 to 50.2% reference engine torque/RPM. */
extern float gain_in_kp(int);

/** Values of byte from 251 to 255 indicate errors, returned as negative. Range
 * from 0 to 250 scale to -40 to +210 degrees Celsius. */
extern float temp_m40_to_p210(int);

/** Values of byte from 251 to 255 indicate errors, returned as negative. Range
 * of -273 to +1735 degrees Celsius. Since -251 to -255 in allowed range,
 * multiply error indicators by 10 before subtracting from 0. */
extern float temp_m273_to_p1735(int);

/** Values of byte from 251 to 255 indicate errors, returned as negative.
 * Range from 0 to 250 scale to -125 to +125 amperes. */
extern float current_m125_to_p125amp(int);

/** Values of byte from 251 to 255 indicate errors, returned as negative.
 * Range from 0 to 250 in amperes. */
extern float current_0_to_250amp(int);

/** Values of byte from 251 to 255 indicate errors, returned as negative.
 * Range from 0 to 250 scale to 0 to 3212.75 V. */
extern float voltage(int);

/** Values of byte from 251 to 255 indicate errors, returned. Range from 0 to
 * 250 scale to -10 to 0 m/s^2. */
extern float brake_demand(int);

/** Values of high byte from 251 to 255 indicate errors, returned as negative.
 * Range from 0 to 64255 scale to 0 to 3212.75 kg/h. */
extern float mass_flow(int);

/** Values of high byte from 251 to 255 indicate errors, returned as negative.
 * Range from 0 to 64255 scale to 0 to 32127.5 kW. */
extern float power_in_kw(int);

/** Values from 251 to 255 indicate errors, stored as negative. Range from 0 to
 * 250 corresponds to 0 to 100 metric tons */
extern float mass_0_to_100t(unsigned char);

/** Value of 0.0 m/s^2 converts to 25000, value of -10.0 m/s^2 to 0, values
 * above 25000 are ignored, per WABCO EXAC docs */
extern short deceleration_to_short(float);

/** for use in printing of EXAC message, decodes two byte value to float m/s^2
 */
extern float short_to_deceleration(short);


#endif /* INCLUDE_JBUS_J1939_UTILS_H_ */
