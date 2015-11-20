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

#include "buffer.h"
#include "stable.h"

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
* Called functions:	malloc(), b_create()
* Parameters:		int st_size, the initial size in number of elements of the
*					array of STVR. Must be greater than 0.
* Return value:		On success, a STD struct with its st_size member set to 
*					st_size. Otherwise, a STD struct with its st_size set to 0.
* Algorithm:		
*******************************************************************************/
STD st_create(int st_size) {
	/* Create one empty SymbolTableDescriptor with members initialized to 0 */
	STD std = { 0 };

	/* Check for invalid parameter */
	if (st_size == 0)
		return std;

	/* Try to create array of SymbolTableVidRecord */
	std.pstvr = (STVR*)malloc(sizeof(STVR)*st_size);
	if (std.pstvr == NULL)
		return std;

	/* Try to create buffer for VID (lexeme) storage */
	std.plsBD = b_create(CA_INIT_CAPACITY, CA_INC_FACTOR, 'f');
	if (std.plsBD == NULL)
		return std;

	/* Allocation successful, set member st_size to st_size */
	std.st_size = st_size;

	return std;
}

/*******************************************************************************
* Purpose:
* Author:			Skye Turriff
* History:
* Called functions:
* Parameters:
* Return value:
* Algorithm:
*******************************************************************************/
int st_install(STD sym_table, char* lexeme, char type, int line) {
	int offset;		/* Offset into array of STVR */
	STVR newSTVR;

	/* Check for valid symbol table and type */
	if (sym_table.st_size == 0 || (type != 'I' && type != 'F' && type != 'S'))
		return R_FAIL_2;	

	/* Check if lexeme already exists in symbol table */
	if ((offset = st_lookup(sym_table, lexeme)) != -1)
		return offset;

	/* Ensure there is room in symbol table for a new record */
	if (sym_table.st_offset >= sym_table.st_size)
		return R_FAIL_1;

	/* Install new entry into STVR array */
	//newSTVR = sym_table.pstvr[sym_table.st_offset];

	/* Set plex to location in sym_table lexeme storage where new VID starts */
	sym_table.pstvr[sym_table.st_offset].plex = b_setmark(sym_table.plsBD, b_size(sym_table.plsBD));
	for (; *lexeme; lexeme++)
		b_addc(sym_table.plsBD, *lexeme);
	b_addc(sym_table.plsBD, '\0');

	/* CHECK IF R_FLAG WAS SET AFTER EACH CALL TO ADDC!! 
	IF IT WAS MUST RESET ALL POINTERS */

	sym_table.pstvr[sym_table.st_offset].o_line = line;

	/* Initialize status_field */
	sym_table.pstvr[sym_table.st_offset].status_field &= DEFAULTZ;
	sym_table.pstvr[sym_table.st_offset].status_field |= DEFAULT;
	if (type == 'I') {
		sym_table.pstvr[sym_table.st_offset].status_field |= DT_INT;
		sym_table.pstvr[sym_table.st_offset].i_value.int_val = 0;
	}
	else if (type == 'F') {
		sym_table.pstvr[sym_table.st_offset].status_field |= DT_FPL;
		sym_table.pstvr[sym_table.st_offset].i_value.fpl_val = 0;
	}
	else {
		sym_table.pstvr[sym_table.st_offset].status_field |= DT_STR;
		sym_table.pstvr[sym_table.st_offset].i_value.str_offset = -1;
		sym_table.pstvr[sym_table.st_offset].status_field |= SET_FLG;
	}

	st_incoffset();	/* Increment offset into global sym_table STVR array */

	return sym_table.st_offset - 1;
}

/*******************************************************************************
* Purpose:
* Author:			Skye Turriff
* History:
* Called functions:
* Parameters:
* Return value:
* Algorithm:
*******************************************************************************/
int st_lookup(STD sym_table, char* lexeme) {
	return -1;
}

/*******************************************************************************
* Purpose:
* Author:			Skye Turriff
* History:
* Called functions:
* Parameters:
* Return value:
* Algorithm:
*******************************************************************************/
int st_update_type(STD sym_table, int vid_offset, char v_type) {
	return 0;
}

/*******************************************************************************
* Purpose:
* Author:			Skye Turriff
* History:
* Called functions:
* Parameters:
* Return value:
* Algorithm:
*******************************************************************************/
int std_update_value(STD sym_table, int vid_offset, InitialValue i_value) {
	return 0;
}

/*******************************************************************************
* Purpose:
* Author:			Skye Turriff
* History:
* Called functions:
* Parameters:
* Return value:
* Algorithm:
*******************************************************************************/
char st_get_type(STD sym_table, int vid_offset) {
	return 0;
}

/*******************************************************************************
* Purpose:
* Author:			Skye Turriff
* History:
* Called functions:
* Parameters:
* Return value:
* Algorithm:
*******************************************************************************/
void st_destroy(STD sym_table) {

}

/*******************************************************************************
* Purpose:
* Author:			Skye Turriff
* History:
* Called functions:
* Parameters:
* Return value:
* Algorithm:
*******************************************************************************/
int st_print(STD sym_table) {
	int i;

	/* Check for valid symbol table and type */
	if (sym_table.st_size == 0)
		return R_FAIL_1;

	printf("Symbol Table\n____________\n\n");
	printf("Line Number    Variable Identifier\n");
	for (i = 0; i < sym_table.st_offset; i++)
		printf("%d          %s\n", sym_table.pstvr[i].o_line, sym_table.pstvr[i].plex);
	
	return i;
}

/*******************************************************************************
* Purpose:
* Author:			Skye Turriff
* History:
* Called functions:
* Parameters:
* Return value:
* Algorithm:
*******************************************************************************/
int st_store(STD sym_table) {
	return 0;
}

/*******************************************************************************
* Purpose:
* Author:			Skye Turriff
* History:
* Called functions:
* Parameters:
* Return value:
* Algorithm:
*******************************************************************************/
int st_sort(STD symb_table, char s_order) {
	return 0;
}

/*******************************************************************************
* Purpose:
* Author:			Skye Turriff
* History:
* Called functions:
* Parameters:
* Return value:
*******************************************************************************/
static void st_setsize(void) {
	sym_table.st_size = 0;
}

/*******************************************************************************
* Purpose:
* Author:			Skye Turriff
* History:
* Called functions:
* Parameters:
* Return value:
*******************************************************************************/
static void st_incoffset(void) {
	sym_table.st_offset++;
}