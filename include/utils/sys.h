/**\file
 *
 * sys.h
 *
 * @author Abdul Rahman Kreidieh
 * @version 1.0.0
 * @date February 21, 2019
 */
#include <stdio.h>

#ifndef INCLUDE_UTILS_SYS_H_
#define INCLUDE_UTILS_SYS_H_


extern bool allspace(char *pc);


/*
 *	SYNOPSIS
 *	#include <sys_lib.h>
 *
 *	int readline( FILE *pfin, char *pbuff, int size );
 *	*pfin 		-	pointer to file you're reading from.
 *	*pbuff		-	character array into which character are being read.
 *	 size		-	The size of the buffer.
 *					(counted from 1, ie. 1 .. size).
 *
 *	DESCRIPTION
 *
 *  This function attempts to read in a block of data of length "size"
 *  form the file pointed to by "pfile".  The read operation is done
 *  from the current position of the file pointer (in the file).  If
 *  there are fewer characters than "size" left on the line (a line is
 *  a string of characters terminated by a "/n" or an EOF character)
 *  then only those characters will be read in.
 *
 *	The integer value returned by the function will be equal to the
 *  number of characters read in unless readline is called when the
 *  file pointer is pointing directly at "EOF" then the function
 *  will return a -1 (the system constant "ERROR" ).
 *
 *  The string of characters returned in pbuff will always be
 *  terminated by a NULL character, "\O".  The first NULL character
 *  in pbuff indicates the end of the string that was read in.
 *  Be forewarned there may be more than one NULL character in pbuff,
 *  but it is only the first one that is significant.
 *  Any characters returned in the buffer after the first NULL should
 *  be ignored.
 *
 *  If no characters are read in then the first character in pbuff
 *  will be the NULL character and the function will return a value
 *  of -1 (the system constant, "ERROR" ).
 *
 *	RETURN
 *		ERROR	size <= 0
 *		ERROR	EOF
 *		#char	String length.
 *
 *	BUGS
 *  If the value of size is selected such that it is less than the
 *  number of characters on the line pointed to by pfile then the next
 *  time readline is called it will continue by grabbing characters
 *  from the same line.
 *  So it is possible that many "readline"s could be called on the same
 *  line before all the characters on the line are exhausted.
 */
extern int readline(FILE *pfile, char *pbuff, int size);

/**blank FIXME
 *
 * This function installs the given function as a signal handler for the
 * specified signals. All other signals are configured to be ignored by the
 * application code.
 *
 * @param plist
 *		A list of signals to be handled by the given function. The list should
 *		be terminated by an invalid signal number, e.g. -1.
 * @param pfunc
 * 		A signal handler with the prototype: void function(int sig);
 */
extern void sig_ign(int *sig_list, void sig_hand(int sig));

extern long get_ini_long(FILE *pfile, char *pentry, long long_def);

extern unsigned get_ini_hex(FILE *pfile, char *pentry, unsigned hex_def);

/*
 *	SYNOPSIS
 *
 *	#include <stdio.h>
 *
 *	#include <local.h>
 *	#include <sys_ini.h>
 *
 *	FILE *get_ini_section( char *pname, char *psection );
 *
 *	pname		-	The name of a configuration file.
 *	psection	-	The name of a section within a configuration file.
 *					It is not necessary to place brackets around the
 *					section name.
 *
 *	DESCRIPTION
 *	This routine opens the given configuration file, and locates the
 *	relevent section.  It is necessary to call get_ini_section() before
 *	reading initialization values.  It is necessary to call fclose() with
 *	the file pointer when done. It is also necessary to close one section
 *	before calling get_ini_section() again withing the same program.
 *
 *	A typical configuration file is text, and might contain lines like:
 *
 *	[first_section_name]
 *	MoreData=TRUE
 *	AnotherVariable=1.2345
 *
 *	[windows_section]
 *	ScreenSaveActive=1
 *	DoubleClickSpeed=452
 *
 *	[last_section]
 *	MoreData=FALSE
 *
 *	Note that:
 *	Blank lines are ignored.
 *	Comments begin with a # symbol.
 *	Section and variable names should all be on separate lines.
 *
 *	RETURN
 *	non-NULL	-	A file pointer for the given file name and section name.
 *	NULL		-	If the section or file can't be found.
 *
 *	BUGS
 *	Within a single program, only one configuration section and file can be
 *	open at once.
 */
extern FILE *get_ini_section( char *pname, char *psection );


/* -------------------------------------------------------------------------- */
/* ------------------ Extracted from path/local/sys_buff.h ------------------ */
/* -------------------------------------------------------------------------- */

/**
 * Circular buffer type used with data item saving
 * Data from last "data_size" samples can be saved in this
 * structure.
 */
typedef struct {
  void *data_array;     /**< circular buffer */
  int data_size;  		/**< number of structures malloced */
  int data_count; 		/**< control intervals saved (<=data_size) */
  int data_start; 		/**< index of oldest item in the circular buffer */
} cbuff_typ;


/**
 * init_circular_buffer is used to set up buffer of items of a type defined
 * by call program. sizeof(the_type_name) is used for the
 * item_size parameter. The most recent max_data items will be saved.
 */
extern void init_circular_buffer(cbuff_typ *pbuff, int max_data, int item_size);

/**
 * get_circular_index is used to get the index of insertion into the
 * circular buffer's data array.
 */
extern int get_circular_index (cbuff_typ *pbuff);


#endif /* INCLUDE_UTILS_SYS_H_ */
