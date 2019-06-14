/**\file
 *
 * struct.h
 *
 * This file contains common constants and objects used by various components in
 * the codebase.
 *
 * @author Abdul Rahman Kreidieh
 * @version 1.0.0
 * @date April 4, 2019
 */

#include <setjmp.h>     	/* jmp_buf, setjmp, longjmp */
#include <sys/iofunc.h>		/* SIGINT, SIGQUIT, SIGTERM, SIGALRM */


#ifndef INCLUDE_UTILS_COMMON_H_
#define INCLUDE_UTILS_COMMON_H_


/* -------------------------------------------------------------------------- */
/* -------------------- Definitions for common constants -------------------- */
/* -------------------------------------------------------------------------- */


#define ERROR 			-1				/**< error status */
#define END_OF_STRING	((char)'\0')	/**< character representing the end of a string */
#define MAX_LINE_LEN	132				/**< largest number of characters in a string */
#define ERROR_LONG		(-1L)			/**< error status (in long format) */
#define ASC_EQUAL_SIGN	'='				/**< '=' symbol */
#define ASC_LEFT_ARRAY	'['				/**< '[' symbol */


/* -------------------------------------------------------------------------- */
/* --------------------- Definitions for common objects --------------------- */
/* -------------------------------------------------------------------------- */


typedef unsigned char BYTE;		/**< an object used to represent a "byte" of data */
typedef unsigned char BIT;		/**< an object used to represent a "bit" of data */


/* -------------------------------------------------------------------------- */
/* ----------------- Signal messages (used in CAN for now) ------------------ */
/* -------------------------------------------------------------------------- */


/** TODO */
static int sig_list[] = {
	SIGINT,
	SIGQUIT,
	SIGTERM,
	SIGALRM,        // for timer
	ERROR
};


/** TODO */
static jmp_buf exit_env;


/** TODO
 *
 * @param code
 * 		TODO
 */
static void sig_hand(int code) {
	if (code == SIGALRM)
		return;
	else
		longjmp(exit_env, code);
}


/** TODO
 *
 * This function installs the given function as a signal handler for the
 * specified signals.  All other signals are configured to be ignored by the
 * application code.
 *
 * @param sig_list
 * 		A list of signals to be handled by the given function.  The list should
 * 		be terminated by an invalid signal number, e.g. -1.
 * @param sig_hand
 * 		A signal handler with the prototype: void function(int sig);
 */
static void sig_ign(int *sig_list, void sig_hand(int sig)) {
	int i = 0;
	while (sig_list[i] != ERROR) {
		signal(sig_list[i], sig_hand);
		i++;
	}
}


#endif /* INCLUDE_UTILS_COMMON_H_ */
