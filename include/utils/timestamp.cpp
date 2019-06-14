/**\file
 *
 * timestamp.cpp
 *
 * Implementation of methods for timestamp.h header file.
 *
 * @author Abdul Rahman Kreidieh
 * @version 1.0.0
 * @date July 11, 2018
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


/** encodes a timestamp_t variable into a PPS encoder object */
void encode_timestamp(pps_encoder_t encoder, timestamp_t* t) {
	pps_encoder_start_object(&encoder, "time");
	pps_encoder_add_int(&encoder, "hour", t->hour);
	pps_encoder_add_int(&encoder, "minute", t->minute);
	pps_encoder_add_int(&encoder, "second", t->second);
	pps_encoder_add_int(&encoder, "millisecond", t->millisecond);
	pps_encoder_end_object(&encoder);
}

/** Decodes a timestamp_t variable from a PPS decoder object. */
void decode_timestamp(pps_decoder_t decoder, timestamp_t* t) {
	pps_decoder_push(&decoder, "time");
	pps_decoder_get_int(&decoder, "hour", &(t->hour));
	pps_decoder_get_int(&decoder, "minute", &(t->minute));
	pps_decoder_get_int(&decoder, "second", &(t->second));
	pps_decoder_get_int(&decoder, "millisecond", &(t->millisecond));
	pps_decoder_pop(&decoder);
}


/** imports a string timestamp into a timestamp object */
extern void import_timestamp(timestamp_t* t, std::string s) {
    t->hour = std::stoi(s.substr(0,2));
    t->minute = std::stoi(s.substr(3,5));
    t->second = std::stoi(s.substr(6,8));
    t->millisecond = std::stoi(s.substr(9,12));
}


/** Returns a timestamp variable for the current time. */
void get_current_timestamp(timestamp_t *t) {
    // get the current time
	timeb timebuf;
    ftime( &timebuf );
    int now = timebuf.time;

    // move the current time into a timestamp object
    t->millisecond = timebuf.millitm;
    t->second = now % 60;
    now /= 60;
    t->minute = now % 60;
    now /= 60;
    t->hour = now % 24;
}
