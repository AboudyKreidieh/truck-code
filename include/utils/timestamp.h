/**\file
 *
 * timestamp.h
 *
 * This file contains a timestamp object and methods needed to collect, print,
 * and publish timestamps, for use in jbus/j1939 message and when sending
 * lateral / longitudinal commands.
 *
 * @author Abdul Rahman Kreidieh
 * @version 1.0.0
 * @date July 11, 2018
 */

#ifndef SRC_UTILS_TIMESTAMP_H_
#define SRC_UTILS_TIMESTAMP_H_

#include <string>
#include <sys/pps.h>


/** timestamp used to identify when each datapoint was issued. All J1939
 * datasets contain a timestamp_t variable. */
typedef struct {
	int hour;			/**< hour */
	int minute;			/**< minute */
	int second;			/**< second */
	int millisecond;	/**< millisecond */
} timestamp_t;

/** method used to print data from a timestamp_t variable */
extern void print_timestamp(FILE*, timestamp_t*);

/** encodes a timestamp_t variable into a pps encoder object */
extern void encode_timestamp(pps_encoder_t, timestamp_t*);

/** Decodes a timestamp_t variable from a PPS decoder object. */
extern void decode_timestamp(pps_decoder_t decoder, timestamp_t* t);

/** imports a string timestamp into a timestamp object */
extern void import_timestamp(timestamp_t*, std::string);


/** Returns a timestamp variable for the current time. */
extern void get_current_timestamp(timestamp_t*);


#endif /* SRC_UTILS_TIMESTAMP_H_ */
