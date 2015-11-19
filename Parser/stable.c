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
*    VARIABLES
*******************************************************************************/

/* Global variables */
STD sym_table;			/* Symbol Table Descriptor */

/* Local(file) global variables */

/*******************************************************************************
*    FUNCTION PROTOTYPES
*******************************************************************************/

/* Static(local) functions */
static void st_setsize(void);
static void st_incoffset(void);

/*******************************************************************************
* Purpose:			
* Author:			Skye Turriff
* History:			
* Called functions:	
* Parameters:		
* Return value:		
* Algorithm:		
*******************************************************************************/
STD st_create(int st_size) {
	return sym_table;
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
int st_lookup(STD sym_table, char* lexeme) {
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
int st_print(STD sym_table) {
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