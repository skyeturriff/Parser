/*******************************************************************************
* File Name:		stable.c
* Compiler:			MS Visual Studio 2013
* Author:			Skye Turriff
* Course:			CST 8152 - Compilers, Lab Section:	011
* Assignment:		1
* Date:				18 November 2015
* Professor:		Sv. Ranev
* Purpose:			Function definition for protypes declared in stable.h
* Function list:	st_create(), st_install(), st_lookup(), st_update_type(),
*					std_update_value(), st_get_type(), st_destroy(), st_print(),
*					st_store(), st_sort()
*******************************************************************************/

/* The #define _CRT_SECURE_NO_WARNINGS should be used in MS Visual Studio projects
* to suppress the warnings about using "unsafe" functions like fopen()
* and standard sting library functions defined in string.h.
* The define does not have any effect in other compiler projects.
*/
#define _CRT_SECURE_NO_WARNINGS

#include "buffer.h"
#include "stable.h"
#include <string.h>
#include <stdlib.h>

/*******************************************************************************
*    FUNCTION PROTOTYPES
*******************************************************************************/

/* Static(local) functions */
static void st_setsize(void);
static void st_incoffset(void);

/*******************************************************************************
* Purpose:			To allocate memory for one SymbolTableDescriptor (STD) and
*					initialize members
* Author:			Skye Turriff
* History:			Version 1, 19 November 2015
* Called functions:	malloc(), b_create(), free()
* Parameters:		int st_size, the initial size in number of elements of the
*					array of STVR. Must be greater than 0.
* Return value:		On success, a STD struct with its st_size member set to 
*					st_size. Otherwise, a STD struct with its st_size set to 0.
* Algorithm:		Check for valid parameter st_size. If invalid, return std
*					with member st_size set to 0. Else, attempt to allocate 
*					memory for one array of STVR. If it fails, return std with 
*					member st_size set to 0. Else, attempt to allocate memory 
*					for one lexeme storage buffer. If failed, free memory for 
*					STVR array, and return std with member st_size set to 0. 
*					Else, set std member st_size to formal parameter st_size, 
*					and return std.
*******************************************************************************/
STD st_create(int st_size) {
	/* Create one empty SymbolTableDescriptor */
	STD std = { 0 };

	/* Invalid parameter */
	if (st_size == 0) 
		return std;

	/* Try to create array of SymbolTableVidRecord */
	if ((std.pstvr = (STVR*)malloc(sizeof(STVR)*st_size)) == NULL)
		return std;

	/* Try to create buffer for VID (lexeme) storage */
	if ((std.plsBD = b_create(CA_INIT_CAPACITY, CA_INC_FACTOR, 'a')) == NULL) {
		free(std.pstvr);
		return std;
	}

	/* Set member st_size */
	std.st_size = st_size;

	return std;
}

/*******************************************************************************
* Purpose:			Add a new entry into the symbol table array of STVR at the 
*					next available element. 
* Author:			Skye Turriff
* History:			Version 1, 19 November 2015
* Called functions:	st_lookup(), b_setmark(), b_size(), b_addc(), b_rflag(),
*					st_incoffset()
* Parameters:		STD sym_table struct with valid st_size
*					char* lexeme pointer to VID name to be added to table
*					char type of VID
*					int line of first occurence of VID
* Return value:		int offset into STVR array where VID record is installed, 
*					-2 on bad parameters, or -1 if symbol table is full.
* Algorithm:		Check parameters, return -2 if bad. If lexeme already in
*					symbol table, return the offset. If symbol table is full,
*					return -1. Set plex for new STVR to point to the next space
*					available in CA. Add each character in the VID lexeme to 
*					this location and make a C-type string. Once the new lexeme
*					has been added, if at any time the location of the CA was 
*					moved, re-calculate plex for each STVR in the STVR array. 
*					Finally, initialize remaining members for new STVR, and
*					increment st_offset of global sym_table. Return offset of
*					new STVR in STVR array.
*******************************************************************************/
int st_install(STD sym_table, char* lexeme, char type, int line) {
	int offset;		/* Offset into array of STVR */
	char r_flag;	/* Memory reallocation flag for lexeme storage buffer */
	char* tplex;	/* Used to iterate through lexeme storage buffer */
	int i;			/* Loop counter for iteration through STVR array */

	/* Check for valid symbol table */
	if (sym_table.st_size == 0) 
		return ERR_FAIL2;	

	/* Check if lexeme already exists */
	if ((offset = st_lookup(sym_table, lexeme)) != ERR_FAIL1) 
		return offset;

	/* Ensure there is room for a new record */
	if (sym_table.st_offset >= sym_table.st_size) 
		return ERR_FAIL1;

	/* Set plex for new VID record */
	sym_table.pstvr[sym_table.st_offset].plex =
		b_setmark(sym_table.plsBD, b_size(sym_table.plsBD));

	/* Install new entry into STVR array, make C-type string */
	r_flag = 0;
	for (; *lexeme; lexeme++) {
		b_addc(sym_table.plsBD, *lexeme);
		if (b_rflag(sym_table.plsBD)) 
			r_flag = 1;
	}
	b_addc(sym_table.plsBD, '\0');
	if (b_rflag(sym_table.plsBD)) 
		r_flag = 1;

	/* If r_flag set, use tplex to interate through lexeme storage 
	until '\0'. Then set plex of the current STVR to tplex + 1 */
	if (r_flag) {
		sym_table.pstvr[0].plex = b_setmark(sym_table.plsBD, 0);
		tplex = sym_table.pstvr[0].plex;
		for (i = 1; i <= sym_table.st_offset; i++) {
			while (*tplex) { tplex++;  continue; }
			sym_table.pstvr[i].plex = ++tplex;
		}
	}

	/* Record source line number */
	sym_table.pstvr[sym_table.st_offset].o_line = line;

	/* Initialize status_field and i_value */
	sym_table.pstvr[sym_table.st_offset].status_field &= DEFAULTZ;
	sym_table.pstvr[sym_table.st_offset].status_field |= DEFAULT;
	if (type == 'I') { /* integer */
		sym_table.pstvr[sym_table.st_offset].status_field |= DT_INT;
		sym_table.pstvr[sym_table.st_offset].i_value.int_val = 0;
	}
	else if (type == 'F') {	/* float */
		sym_table.pstvr[sym_table.st_offset].status_field |= DT_FPL;
		sym_table.pstvr[sym_table.st_offset].i_value.fpl_val = 0.0F;
	}
	else { /* string */
		sym_table.pstvr[sym_table.st_offset].status_field |= DT_STR;
		sym_table.pstvr[sym_table.st_offset].status_field |= SET_FLG;
		sym_table.pstvr[sym_table.st_offset].i_value.str_offset = -1;
	}

	st_incoffset();	/* Increment offset into STVR array of global sym_table */

	return sym_table.st_offset;
}

/*******************************************************************************
* Purpose:			Searches for a VID lexeme in the symbol table
* Author:			Skye Turriff
* History:			Version 1, 20 November 2015
* Called functions: 
* Parameters:		STD sym_table stuct with valid st_size (>0)
*					char* lexeme to be searched for
* Return value:		Returns -2 on invalid parameters. Else if found, the offset 
*					of the entry into STVR array. Returns -1 if lexeme not found
* Algorithm:		Beginning from the last entry in array of STVR, compare each
*					stored lexeme with the formal parameter lexeme.
*******************************************************************************/
int st_lookup(STD sym_table, char* lexeme) {
	/* Check for valid symbol table */
	if (sym_table.st_size == 0) return ERR_FAIL2;

	while (sym_table.st_offset > 0) {
		if (!strcmp(sym_table.pstvr[--sym_table.st_offset].plex, lexeme))
			return sym_table.st_offset;
	}

	return ERR_FAIL1;	/* If lexeme not found */
}

/*******************************************************************************
* Purpose:			Updates the data type of the STVR specified by vid_offset
* Author:			Skye Turriff
* History:			Version 2, 12 December 2015
* Called functions:	None
* Parameters:		STD sym_table with a valid st_size (>0)
*					int vid_offset into STVR array of the entry to update
*					char v_type to update the entry with, one of 'F' for float,
*					or 'I' for integer				
* Return value:		On success, the offset of the entry updated, -1 on failure,
*					or -2 on invalid symbol table.
* Algorithm:		If invalid symbol table return -2, if invalid parameters
*					return -1. If entry is a string, or has already been 
*					updated, return -1. Else, set status_field bits 0, 1 and 2
*					to 0. If updating type to float turn on bit 1. Else turn on
*					bit 2 to update to int. Set update flag. Return offset of
*					updated entry.
*******************************************************************************/
int st_update_type(STD sym_table, int vid_offset, char v_type) {
	/* Check for valid symbol table */
	if (sym_table.st_size == 0)
		return ERR_FAIL2;

	/* Check for invalid vid_offset */
	if (vid_offset < 0 || vid_offset >= sym_table.st_offset)
		return ERR_FAIL1;

	/* Can't update type more than once or from/to string */
	if ((sym_table.pstvr[vid_offset].status_field & CHK_FLG)
		//|| (st_get_type(sym_table, vid_offset) == 'S')
		|| (v_type == 'S'))
		return ERR_FAIL1;

	/* Reset data type bits, set bits for new type and update flag */
	sym_table.pstvr[vid_offset].status_field &= DEFAULT;
	sym_table.pstvr[vid_offset].status_field |= 
		((v_type == 'F' ? DT_FPL : DT_INT) | SET_FLG);

	return vid_offset;
}

/*******************************************************************************
* Purpose:			Updates the i_value of the variable specified by vid_offset
* Author:			Skye Turriff
* History:			Version 2, 12 December 2015
* Called functions:	None
* Parameters:		STD sym_table struct with valid size (>0)
*					int vid_offset of entry in STVR array
*					InitialValue i_value to update to
* Return value:		If bad parameters returns -1, or -2 on invalid symbol table.
*					On success returns vid_offset
*******************************************************************************/
int st_update_value(STD sym_table, int vid_offset, InitialValue i_value) {
	/* Check for valid symbol table  */
	if (sym_table.st_size == 0)
		return ERR_FAIL2;

	/* Check for valid vid_offset */
	if (vid_offset < 0 || vid_offset >= sym_table.st_offset)
		return ERR_FAIL1;

	sym_table.pstvr[vid_offset].i_value = i_value;

	return vid_offset;
}

/*******************************************************************************
* Purpose:			Returns the data type of the entry specified by vid_offset
* Author:			Skye Turriff
* History:			Version 1, 21 November 2015
* Called functions:	None
* Parameters:		STD sym_table struct with valid size (>0)
*					int vid_offset of entry in STVR array
* Return value:		char 'F' for float, 'I' for integer, or 'S' for string, or
*					-1 on failure. Returns -2 on bad parameters.
*******************************************************************************/
char st_get_type(STD sym_table, int vid_offset) {
	unsigned short status_field = sym_table.pstvr[vid_offset].status_field;

	/* Check for valid symbol table */
	if (sym_table.st_size == 0) 
		return ERR_FAIL2;

	/* Extract data type */
	if ((status_field & DT_INT) && (status_field & DT_FPL))
		return 'S';
	if (status_field & DT_INT)
		return 'I';
	if (status_field & DT_FPL)
		return 'F';
	
	return ERR_FAIL1;	/* Invalid datatype */
}

/*******************************************************************************
* Purpose:			Frees memory occupied by symbol table dynamic areas and sets
*					global symbol table size to 0.
* Author:			Skye Turriff
* History:			Version 1, 21 November 2015
* Called functions:	b_destroy(), free(), st_setsize()
* Parameters:		STD sym_table struct
* Return value:		None
*******************************************************************************/
void st_destroy(STD sym_table) {
	if (sym_table.st_size != 0) {
		b_destroy(sym_table.plsBD);
		free(sym_table.pstvr);
		st_setsize();
	}
}

/*******************************************************************************
* Purpose:			Prints the contents of the symbol table to standard output
* Author:			Skye Turriff
* History:			Version 1, 21 November 2015
* Called functions: printf()
* Parameters:		STD sym_table struct with valid st_size (>0)
* Return value:		On success, the number of records printed, 
*******************************************************************************/
int st_print(STD sym_table) {
	int i;

	/* Check for valid symbol table */
	if (sym_table.st_size == 0) 
		return R_FAIL_2;

	printf("\nSymbol Table\n____________\n\n");
	printf("Line Number Variable Identifier\n");
	for (i = 0; i < sym_table.st_offset; i++)
		printf("%2d          %s\n",	sym_table.pstvr[i].o_line, 
		sym_table.pstvr[i].plex);
	
	return i;
}

/*******************************************************************************
* Purpose:			Stores the contents of the symbol table to file $stable.ste
* Author:			Skye Turriff
* History:			Version 1, 21 November 2015
* Called functions:	fopen(), fprintf(), st_get_type(), printf()
* Parameters:		STD sym_table with valid st_size (>0)
* Return value:		On success, the number of records printed, else -1
*******************************************************************************/
int st_store(STD sym_table) {
	FILE* fp;	/* Output file handle */
	char type;	/* Type of variable */
	int i; 

	/* Check for valid symbol table and fp */
	if (sym_table.st_size == 0) 
		return ERR_FAIL2;
	if ((fp = fopen("$stable.ste", "wt")) == NULL)
		return ERR_FAIL1;

	/* Store symbol table contents in file */
	fprintf(fp, "%d", sym_table.st_size);
	for (i = 0; i < sym_table.st_offset; i++) {
		type = st_get_type(sym_table, i);
		fprintf(fp, " %hX %d %s %d %s", 
			sym_table.pstvr[i].status_field,	/* status field */
			strlen(sym_table.pstvr[i].plex),	/* lexeme length */
			sym_table.pstvr[i].plex,			/* lexeme */
			sym_table.pstvr[i].o_line,			/* line number */
			(type == 'S' ? "-1" :				/* -1 if string, else */
			(type == 'I' ? "0" : "0.00")));		/* 0 if int, else 0.00 */
	}

	fclose(fp);
	printf("Symbol Table stored.\n");

	return i;	/* Numer of records stored */
}

/*******************************************************************************
* Purpose:			Set the size of the global sym_table to 0
* Author:			Skye Turriff
* History:			Version 1, 20 November 2015
* Called functions:	None
* Parameters:		None
* Return value:		None
*******************************************************************************/
static void st_setsize(void) {
	sym_table.st_size = 0;
}

/*******************************************************************************
* Purpose:			Increments the st_offset member of the global sym_table
* Author:			Skye Turriff
* History:			Version 2, 12 December 2015
* Called functions:	None
* Parameters:		None
* Return value:		None
*******************************************************************************/
static void st_incoffset(void) {
	++sym_table.st_offset;
}