/**\file
 *
 * sys.h
 *
 * @author Abdul Rahman Kreidieh
 * @version 1.0.0
 * @date February 21, 2019
 */

#ifndef INCLUDE_UTILS_SYS_H_
#define INCLUDE_UTILS_SYS_H_

#include <stdio.h>


/** Check whether a give string is all spaces.
 *
 * @param pc
 * 		the string that is being tested
 * @return
 * 		True is all spaces, False otherwise
 */
extern bool allspace(char *pc);


/** TODO
 *
 * This function attempts to read in a block of data of length "size" form the
 * file pointed to by "pfile".  The read operation is done from the current
 * position of the file pointer (in the file).  If there are fewer characters
 * than "size" left on the line (a line is a string of characters terminated by
 * a "/n" or an EOF character) then only those characters will be read in.
 *
 * The integer value returned by the function will be equal to the number of
 * characters read in unless readline is called when the file pointer is
 * pointing directly at "EOF" then the function will return a -1 (the system
 * constant "ERROR" ).
 *
 * The string of characters returned in pbuff will always be terminated by a
 * NULL character, "\O".  The first NULL character in pbuff indicates the end
 * of the string that was read in. Be forewarned there may be more than one NULL
 * character in pbuff, but it is only the first one that is significant. Any
 * characters returned in the buffer after the first NULL should be ignored.
 *
 * If no characters are read in then the first character in pbuff will be the
 * NULL character and the function will return a value of -1 (the system
 * constant, "ERROR" ).
 *
 * Bugs:
 *
 * - If the value of size is selected such that it is less than the number of
 *   characters on the line pointed to by pfile then the next time readline is
 *   called it will continue by grabbing characters from the same line.
 * - So it is possible that many "readline"s could be called on the same line
 *   before all the characters on the line are exhausted.
 *
 * @param pfile
 * 		pointer to file you're reading from.
 * @param pbuff
 * 		character array into which character are being read.
 * @param size
 * 		The size of the buffer. (counted from 1, ie. 1 .. size).
 *
 * TODO RETURN
 *		ERROR	size <= 0
 *		ERROR	EOF
 *		#char	String length.
 */
extern int readline(FILE *pfile, char *pbuff, int size);


/** TODO
 *
 * @param pfile
 * 		pointer to file you're reading from.
 */
extern long get_ini_long(FILE *pfile, char *pentry, long long_def);


/** TODO
 *
 * @param pfile
 * 		pointer to file you're reading from.
 */
extern unsigned get_ini_hex(FILE *pfile, char *pentry, unsigned hex_def);


/** TODO
 *
 * This routine opens the given configuration file, and locates the relevant
 * section.  It is necessary to call get_ini_section() before reading
 * initialization values.  It is necessary to call fclose() with the file
 * pointer when done. It is also necessary to close one section before calling
 * get_ini_section() again within the same program.
 *
 * A typical configuration file is text, and might contain lines like:
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
 * Note that:
 *
 * - Blank lines are ignored.
 * - Comments begin with a # symbol.
 * - Section and variable names should all be on separate lines.
 *
 * Bugs
 *
 * - Within a single program, only one configuration section and file can be
 *   open at once.
 *
 * @param pname
 * 		The name of a configuration file.
 * @param psection
 * 		The name of a section within a configuration file. It is not necessary
 * 		to place brackets around the section name.
 * @return
 * 		A file pointer for the given file name and section name. NUL if the
 * 		section or file can't be found.
 */
extern FILE *get_ini_section(char *pname, char *psection);


#endif /* INCLUDE_UTILS_SYS_H_ */
