/**\file
 *
 * j1939_utils.h
 *
 * This file contains several constants, macros, and methods that are used when
 * trying to interpret a J1939 message. This includes definitions of macros and
 * constants needed to receive, convert and scale the fields in J1939 Protocol
 * Data Units (frames).
 *
 * Utility functions used when processing raw bit/byte data into message-
 * specific units, as specified by Lane Assist Interface documentation.
 *
 * Routines return out of range values to indicate errors. Application programs
 * are responsible for checking that values are in range.
 *
 * @author Abdul Rahman Kreidieh
 * @version 1.0.0
 * @date May 31, 2018
 */

#ifndef INCLUDE_JBUS_J1939_UTILS_H_
#define INCLUDE_JBUS_J1939_UTILS_H_

#include <math.h>
#include <stdio.h>
#include "utils/common.h"	/* BYTE */


/* Parameter group numbers for supported messages (pdu_format, pdu_specific) */

#define PDU		0x00ff	/**< (0, 255) sample undefined parameter group number */
#define TSC1	0x0000	/**< (0, 0) Torque Speed Control 1, destination 0 */
#define EXAC	0x000b	/**< (0, 11) EXAC (WABCO proprietary) */
#define RQST	0xea00	/**< (234, 0) request transmission of a particular PGN */
#define ERC1	0xf000	/**< (240, 0) electronic retarder controller 1 */
#define EBC1	0xf001	/**< (240, 1) electronic brake controller 1 */
#define ETC1	0xf002	/**< (240, 2) electronic transmission controller 1 */
#define EEC2	0xf003  /**< (240, 3) electronic engine controller 2 */
#define EEC1	0xf004	/**< (240, 4) electronic engine controller 1 */
#define ETC2	0xf005	/**< (240, 5) electronic transmission controller 1 */
#define GFI2	0xfe81	/**< (254, 129) gaseous fuel information 2 */
#define EI		0xfe92	/**< (254, 146) engine information */
#define FD		0xfebd	/**< (254, 189) fan drive */
#define EBC2	0xfebf	/**< (254, 191) electronic brake controller 2 */
#define HRVD	0xfec1	/**< (254, 193) high resolution vehicle distance */
#define TURBO	0xfedd	/**< (254, 221) turbocharger */
#define EEC3	0xfedf	/**< (254, 223) electronic engine controller 3 */
#define VD		0xfee0	/**< (254, 224) vehicle distance */
#define RCFG	0xfee1	/**< (254, 225) retarder configuration */
#define TCFG	0xfee2	/**< (254, 226) transmission configuration */
#define ECFG	0xfee3	/**< (254, 227) engine configuration */
#define ETEMP	0xfeee	/**< (254, 238) engine temperature */
#define PTO		0xfef0	/**< (254, 240) power takeoff */
#define CCVS	0xfef1	/**< (254, 241) cruise control vehicle speed */
#define LFE		0xfef2	/**< (254, 242) fuel economy */
#define AMBC	0xfef5	/**< (254, 245) ambient conditions */
#define IEC		0xfef6	/**< (254, 246) inlet/exhaust conditions */
#define VEP		0xfef7	/**< (254, 247) vehicle electric power */
#define TF		0xfef8	/**< (254, 248) transmission fluids */
#define RF		0xfefb	/**< (254, 251) retarder fluids */


/* Return values when retrieving and processing J1939 messages. */

#define J1939_RECEIVE_FATAL_ERROR 	-1	/**< Returned if the error should lead */
										/**< to deactivating the process.*/
#define J1939_RECEIVE_MESSAGE_ERROR  0	/**< Returned if an error message was */
										/**< received from the CAN card.*/
#define J1939_RECEIVE_MESSAGE_VALID  1	/**< Returned if the message received */
										/**< from the CAN card is valid. */


/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */


/* General purpose macros for extracting fields.
 *
 * Bit number in these macros for bytes follows convention in J1939 docs.
 */

/* Macros for short (two bit) values */
#define BITS87(x)		(((x) & 0xc0) >> 6)		/**< Extract bits 7 and 8 from a single byte. */
#define BITS65(x)		(((x) & 0x30) >> 4)		/**< Extract bits 5 and 6 from a single byte. */
#define BITS43(x)		(((x) & 0x0c) >> 2)		/**< Extract bits 3 and 4 from a single byte. */
#define BITS21(x)		((x) & 0x03)			/**< Extract bits 1 and 2 from a single byte. */
#define HINIBBLE(x)		(((x) & 0xf0) >> 4)		/**< Extract the high four bits from a single byte. */
#define LONIBBLE(x)		((x) & 0x0f)			/**< Extract the high low bits from a single byte. */

/* Macros for short (two byte) values */
#define HIBYTE(x)		(((x) & 0xff00) >> 8)				/**< Extract the high byte from two bytes. */
#define LOBYTE(x)		((x) & 0xff)						/**< Extract the low byte from two bytes. */
#define TWOBYTES(x, y)	((((x) & 0xff) << 8) | (y & 0xff))	/**< Combine two bytes to a single value. */

/* Macros for int (four byte) values */
#define BYTE0(x)		(((x) & 0xff))				/**< Extract the first byte from four bytes. */
#define BYTE1(x)		(((x) & 0xff00) >> 8)		/**< Extract the second byte from four bytes. */
#define BYTE2(x)		(((x) & 0xff0000) >> 16)	/**< Extract the third byte from four bytes. */
#define BYTE3(x)		(((x) & 0xff000000) >> 24)	/**< Extract the fourth byte from four bytes. */
#define FOURBYTES(a3, a2, a1, a0)   \
			(((a3) & 0xff) << 24) | \
			(((a2) & 0xff) << 16) | \
			(((a1) & 0xff) << 8)  | \
			((a0) & 0xff)							/**< Combine four bytes to a single value. */


/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */


/** Compute a percentage value between 0-100% from input data.
 *
 * Values from 251 to 255 indicate errors, stored as negative. Range from 0 to
 * 250 corresponds to 0 to 100% range of return values
 *
 * @param data one-byte value
 * @return percent value
 */
extern float percent_0_to_100(int data);


/** Compute a percentage value between 0-250% from input data.
 *
 * Values from 251 to 255 indicate errors, stored as negative. Range from 0 to
 * 250 corresponds to 0 to 250% range of return values
 *
 * @param data one-byte value
 * @return percent value
 */
extern float percent_0_to_250(int data);


/** Compute a percentage value between -125 to 125% from input data.
 *
 * Values from 251 to 255 indicate errors, stored as negative. Range from 0 to
 * 250 corresponds to -125 to 125% range of return values
 *
 * @param data one-byte value
 * @return percent value
 */
extern float percent_m125_to_p125(int data);


/** Compute the gear percentage from input data.
 *
 * Values of 251 indicates park. Values 252 to 255 indicate errors, returned
 * as negative.  Range from 0 to 250 corresponds to -125 to 125 range of return
 * values
 *
 * This is used by ETC2-formatted messages.
 *
 * @param data one-byte value
 * @return percentage value
 */
int gear_m125_to_p125(int data);


/** Compute the gear ratio from input data.
 *
 * Hibyte values from 251 to 255 indicate errors, stored as negative. Range
 * from 0 to 64255 corresponds to 0 to 64.255 range of return values
 *
 * @param data two-byte value
 * @return gear ratio
 */
extern float gear_ratio(int data);


/** Compute a pressure value between 0-4000 kPa.
 *
 * Values from 251 to 255 indicate errors, stored as negative. Range from 0 to
 * 250 corresponds to 0 to 4000 kilopascals pressure
 *
 * @param data one-byte value
 * @return pressure (kPa)
 */
extern float pressure_0_to_4000kpa(int data);


/** Compute a pressure value between 0-1000 kPa.
 *
 * Values from 251 to 255 indicate errors, stored as negative. Range from 0 to
 * 250 corresponds to 0 to 1000 kilopascals pressure
 *
 * @param data one-byte value
 * @return pressure (kPa)
 */
extern float pressure_0_to_1000kpa(int data);


/** Compute a pressure value between 0-500 kPa.
 *
 * Values from 251 to 255 indicate errors, stored as negative. Range from 0 to
 * 250 corresponds to 0 to 500 kilopascals pressure
 *
 * @param data one-byte value
 * @return pressure (kPa)
 */
extern float pressure_0_to_500kpa(int data);


/** Compute a pressure value between 0-125 kPa.
 *
 * Values from 251 to 255 indicate errors, stored as negative. Range from 0 to
 * 250 corresponds to 0 to 125 kilopascals pressure
 *
 * @param data one-byte value
 * @return pressure (kPa)
 */
extern float pressure_0_to_125kpa(int data);


/** Compute a pressure value between 0-12 kPa.
 *
 * Values from 251 to 255 indicate errors, stored as negative. Range from 0 to
 * 250 corresponds to 0 to 12.5 kilopascals pressure
 *
 * @param data one-byte value
 * @return pressure (kPa)
 */
extern float pressure_0_to_12kpa(int data);


/** Compute a pressure value between -250 to 252 kPa.
 *
 * Hibyte values from 251 to 255 indicate errors, stored as negative. Range
 * from 0 to 64255 corresponds to -250 to 251.96 kilopascals pressure
 *
 * @param data two-byte value
 * @return pressure (kPa)
 */
extern float pressure_m250_to_p252kpa(int data);


/** Compute the rotor speed from input data.
 *
 * Hibyte values from 251 to 255 indicate errors, stored as negative. Range
 * from 0 to 64255 corresponds to 0 to 257,020 range of return values
 *
 * This is used in TURBO-formatted messages.
 *
 * @param data two-byte value
 * @return angular speed (RPM)
 */
extern float rotor_speed_in_rpm(unsigned short data);


/** Compute the distance traveled from input data.
 *
 * Upper byte values from 251 to 255 indicate errors, stored as negative.
 * Range from 0 to 4211081215 corresponds to 0 to 526,385,151.875 km
 *
 * @param data four-byte value
 * @return distance (km)
 */
extern float distance_in_km(unsigned int data);


/** Compute the distance traveled from input data.
 *
 * Upper byte values from 251 to 255 indicate errors, stored as negative.
 * Range from 0 to 4,211,081,215 corresponds to 0 to 21,055,406.075 km
 *
 * @param data four-byte value
 * @return distance (km)
 */
extern float hr_distance_in_km(unsigned int data);


/** Compute the distance traveled from input data.
 *
 * Values from 251 to 255 indicate errors, stored as negative. Range from 0 to
 * 250 corresponds to 0 to 2500 range of return values
 *
 * @param data one-byte value
 * @return angular speed (RPM)
 */
extern float speed_in_rpm_1byte(int data);


/** Compute the angular speed from input data.
 *
 * Values of upper byte from 251 to 255 indicate errors, returned as negative.
 * Range from 0 to 64255 corresponds to 0 to 8031.875 range of return values
 *
 * @param data two-byte value
 * @return speed (RPM)
 */
extern float speed_in_rpm_2byte(int data);


/** Compute wheel-based meters per second.
 *
 * Values of upper byte, 0 to 250, in km/h; values of lower byte, 0 to 255.
 * Scaled by 1/256 to give fraction of km/h. Converted to m/sec to agree with
 * units used by AVCS programs. Values of upper byte from 251 to 255 indicate
 * errors, returned as negative. Range from 0 to 69.721 meters/sec.
 *
 * @param data two-byte value
 * @return speed (m/s)
 */
extern float wheel_based_mps(int data);


/** Compute the relative wheel-based meters per second.
 *
 * Values from 251 to 255 indicate errors, stored as negative. Range from 0 to
 * 250 corresponds to -2.170 to +2.170 meters/sec
 *
 * @param data one-byte value
 * @return relative speed (m/s)
 */
extern float wheel_based_mps_relative(int data);


/** Compute the cruise control desired speed.
 *
 * Values of byte, 0 to 250, in km/h; converted to m/sec to agree with units
 * used by AVCS programs. Values of byte from 251 to 255 indicate errors,
 * returned as negative.
 *
 * This is used in CCVS-formatted messages.
 *
 * @param data one-byte value
 * @return speed (m/s)
 */
extern float cruise_control_set_meters_per_sec(int data);


/** Compute the fuel rate.
 *
 * Values of upper byte from 251 to 255 indicate errors, returned as negative.
 * Range from 0 to 64255 corresponds to 0 to 3212.75 liters/hour or 0 to 892.430
 * cubic centimeters per second.
 *
 * @param data two-byte value
 * @return fuel rate (cm^3/s)
 */
extern float fuel_rate_cm3_per_sec(int data);


/** Compute the fuel efficiency from input data bytes.
 *
 * Values of upper byte from 251 to 255 indicate errors, returned as negative.
 * Range from 0 to 64555 corresponds to 0 to 128.498 meters/cubic centimeter
 *
 * @param data two-byte value
 * @return efficiency (m/cm^3)
 */
extern float fuel_economy_meters_per_cm3(int data);


/** Compute the torque.
 *
 * Values of upper byte from 251 to 255 indicate errors, returned as negative.
 * Range from 0 to 64255 Nm
 *
 * @param data two-byte value
 * @return torque (Nm)
 */
extern float torque_in_nm(unsigned short data);


/** Compute the time.
 *
 * Values of byte from 251 to 255 indicate errors, returned as negative. Range
 * from 0 to 250 scale to 0 to 25 seconds.
 *
 * @param data one-byte value
 * @return time (s)
 */
extern float time_0_to_25sec(BYTE data);


/** Compute the gain.
 *
 * Values of byte from 251 to 255 indicate errors, returned as negative. Range
 * of 0 to 50.2% reference engine torque/RPM.
 *
 * @param data one-byte value
 * @return gain
 */
extern float gain_in_kp(int data);


/** Get a temperature value between -40 to 210.
 *
 * Values of byte from 251 to 255 indicate errors, returned as negative. Range
 * from 0 to 250 scale to -40 to +210 degrees Celsius.
 *
 * @param data one-byte value
 * @return temperature (deg C)
 */
extern float temp_m40_to_p210(int data);


/** Get a temperature value between -273 to 1735.
 *
 * Values of byte from 251 to 255 indicate errors, returned as negative. Range
 * of -273 to +1735 degrees Celsius. Since -251 to -255 in allowed range,
 * multiply error indicators by 10 before subtracting from 0.
 *
 * @param data two-byte value
 * @return temperature (deg C)
 */
extern float temp_m273_to_p1735(int data);


/** Compute a current value between -125 to 125 A.
 *
 * Values of byte from 251 to 255 indicate errors, returned as negative.
 * Range from 0 to 250 scale to -125 to +125 amperes.
 *
 * This is used by VEP-formated J1939 messages.
 *
 * @param data one-byte value
 * @return current (A)
 */
extern float current_m125_to_p125amp(int data);


/** Compute a current value between 0-250 A.
 *
 * Values of byte from 251 to 255 indicate errors, returned as negative.
 * Range from 0 to 250 in amperes.
 *
 * This is used by VEP-formated J1939 messages.
 *
 * @param data one-byte value
 * @return current (A)
 */
extern float current_0_to_250amp(int data);


/** Compute the voltage from input data bytes.
 *
 * Values of byte from 251 to 255 indicate errors, returned as negative.
 * Range from 0 to 250 scale to 0 to 3212.75 V.
 *
 * This is used by VEP-formated J1939 messages.
 *
 * @param data two-byte value
 * @return voltage (V)
 */
extern float voltage(int data);


/** Compute the break demand from input data bytes.
 *
 * Values of byte from 251 to 255 indicate errors, returned. Range from 0 to
 * 250 scale to -10 to 0 m/s^2.
 *
 * This is used by EBC1-formated J1939 messages.
 *
 * @param data one-byte value
 * @return break (m/s^2)
 */
extern float brake_demand(int data);


/** Compute the estimated mass flow from input data bytes.
 *
 * Values of high byte from 251 to 255 indicate errors, returned as negative.
 * Range from 0 to 64255 scale to 0 to 3212.75 kg/h.
 *
 * This is used by EI-formated J1939 messages.
 *
 * @param data two-byte value
 * @return mass flow (kg/h)
 */
extern float mass_flow(int data);


/** Compute the estimated break power from input data bytes.
 *
 * Values of high byte from 251 to 255 indicate errors, returned as negative.
 * Range from 0 to 64255 scale to 0 to 32127.5 kW.
 *
 * This is used by EI-formated J1939 messages.
 *
 * @param data two-byte value
 * @return power (kW)
 */
extern float power_in_kw(int data);


#endif /* INCLUDE_JBUS_J1939_UTILS_H_ */
