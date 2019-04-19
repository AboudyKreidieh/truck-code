/**\file
 *
 * sys.cpp
 *
 * @author Abdul Rahman Kreidieh
 * @version 1.0.0
 * @date February 21, 2019
 */

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include "common.h"
#include "sys.h"

static long section_offset;


bool allspace(char *pc) {
	while (*pc != END_OF_STRING) {
		if(!isspace(*pc++))
			return false;
	}
	return true;
}


int readline(FILE *pfile, char *pbuff, int size)
{
	char *pcursor;
	int counter;

	if(size <= 0)
		return ERROR;

	while(allspace(pbuff)) {	/*	Until got a valid line. */
		/*	fgets() will only read size - 1 characters.	*/
		if (fgets(pbuff, size, pfile) == NULL)
			return ERROR;

		pcursor = pbuff;
		for (counter = 0; counter < size; counter++, pcursor++) {
			if( (*pcursor == '\n') || (*pcursor == '#') )
				break;
		}
		*pcursor = END_OF_STRING;
	}

	return counter;
}


static char *get_entry(FILE *pfile, char *pentry)
{
	static char buffer[MAX_LINE_LEN+1];
	char *pstring;
	int len;

	if (fseek( pfile, section_offset, SEEK_SET ) != 0)
		return NULL;

	len = strlen(pentry);
	while(strncmp(buffer, pentry, len) != 0) {
		if (readline( pfile, buffer, MAX_LINE_LEN ) == EOF)
			return NULL;

		// TODO: is this a bug?
		for (pstring = buffer; isspace( *pstring ); pstring++)
			;

		if (*pstring == ASC_LEFT_ARRAY)  /* Overrun section? */
			return( NULL );
	}

	/*	Parse out the first equal sign. */
	pstring = strchr(buffer, ASC_EQUAL_SIGN);
	pstring++;
	if( strlen( pstring ) == 0 )		/*	No setting?		*/
		return NULL;
	else
		return pstring;
}


long get_ini_long(FILE *pfile, char *pentry, long long_def)
{
	char buffer[MAX_LINE_LEN+1];
	char *pbuffer, *presult;

	if ((presult = get_entry(pfile, pentry)) == NULL)
		return long_def;
	else
	{
		for( pbuffer = buffer; isdigit( *presult )
			&& (pbuffer < (buffer + MAX_LINE_LEN)); )
		{
			*pbuffer++ = *presult++;
		}
	}

	*pbuffer = END_OF_STRING;
	if( strlen( buffer ) == 0 )
		return long_def;
	else
		return atol(buffer);
}

unsigned get_ini_hex(FILE *pfile, char *pentry, unsigned hex_def)
{
	char buffer[MAX_LINE_LEN+1];
	char *pbuffer, *presult;

	if ((presult = get_entry(pfile, pentry)) == NULL)
		return hex_def;
	else {
		for(pbuffer = buffer; isxdigit(*presult)
			&& (pbuffer < (buffer + MAX_LINE_LEN)); ) {
			*pbuffer++ = *presult++;
		}
	}

	*pbuffer = END_OF_STRING;
	if(strlen(buffer) == 0)
		return hex_def;
	else
		return strtoul(buffer, NULL, 16);  // replaced atoh with this for QNX 7 support
}


FILE *get_ini_section(char *pname, char *psection)
{
	FILE *pfin;
	char section[MAX_LINE_LEN+1];
	char buffer[MAX_LINE_LEN+1];

	section_offset = ERROR_LONG;
	if( (pfin = fopen(pname, "r")) == NULL )
		return NULL;

	sprintf(section,"[%s]", psection);	/* Format the section name */

	/* Move through file 1 line at a time until a section is matched or EOF */
	while(strcmp(buffer, section) != 0)
	{
		if(readline(pfin, buffer, MAX_LINE_LEN) == EOF)
		{
			fclose( pfin );
			return( NULL );
		}
	}

	section_offset = ftell( pfin );

	return pfin;
}

