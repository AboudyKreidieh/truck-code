/**
 * timestamp.h
 *
 * This file contains a timestamp object and methods needed to collect, print,
 * and publish timestamps, for use in jbus/j1939 message and when sending
 * lateral / longitudinal commands.
 *
 *  Created on: July 11, 2018
 *      Author: Abdul Rahman Kreidieh
 */

#ifndef SRC_UTILS_TIMESTAMP_H_
#define SRC_UTILS_TIMESTAMP_H_

#include <string>
#include <sys/pps.h>


/** timestamp used to identify when each datapoint was issued. All J1939
 * datasets contain a timestamp_t variable. */
typedef struct {
	int hour;
	int minute;
	int second;
	int millisecond;
} timestamp_t;

/** method used to print data from a timestamp_t variable */
extern void print_timestamp(FILE*, timestamp_t*);

/** encodes a timestamp_t variable into a pps encoder object */
extern void encode_timestamp(pps_encoder_t, timestamp_t*);

/** imports a string timestamp into a timestamp object */
extern void import_timestamp(timestamp_t*, std::string);


/** Returns a timestamp variable for the current time. */
timestamp_t *get_current_timestamp();


#endif /* SRC_UTILS_TIMESTAMP_H_ */
