/**
 * timestamp.cpp
 *
 * Implementation of methods for timestamp.h header file.
 *
 *  Created on: July 11, 2018
 *      Author: Abdul Rahman Kreidieh
 */

#include "timestamp.h"
#include <string>
#include <sys/pps.h>
#include <sys/timeb.h>


/** method used to print data from a timestamp_t variable */
void print_timestamp(FILE *fp, timestamp_t *t) {
	fprintf(fp, " %02d:%02d:%02d.%03d",
		t->hour, t->minute, t->second, t->millisecond);
}


/** encodes a timestamp_t variable into a pps encoder object */
void encode_timestamp(pps_encoder_t encoder, timestamp_t* t) {
	pps_encoder_start_object(&encoder, "time");
	pps_encoder_add_int(&encoder, "hour", t->hour);
	pps_encoder_add_int(&encoder, "minute", t->minute);
	pps_encoder_add_int(&encoder, "second", t->second);
	pps_encoder_add_int(&encoder, "millisecond", t->millisecond);
	pps_encoder_end_object(&encoder);
}


/** imports a string timestamp into a timestamp object */
extern void import_timestamp(timestamp_t* t, std::string s) {
    t->hour = std::stoi(s.substr(0,2));
    t->minute = std::stoi(s.substr(3,5));
    t->second = std::stoi(s.substr(6,8));
    t->millisecond = std::stoi(s.substr(9,12));
}


/** Returns a timestamp variable for the current time. */
timestamp_t *get_current_timestamp() {
    timestamp_t *t = new timestamp_t();

    // get the current time
	struct timeb timebuf;
    ftime( &timebuf );
    int now = timebuf.time;

    // move the current time into a timestamp object
    t->millisecond = timebuf.millitm;
    t->second = now % 60;
    now /= 60;
    t->minute = now % 60;
    now /= 60;
    t->hour = now % 24;

    return t;
}
