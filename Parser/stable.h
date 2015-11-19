/*******************************************************************************
* File Name:		stable.h
* Compiler:			MS Visual Studio 2013
* Author:			Skye Turriff
* Course:			CST 8152 - Compilers, Lab Section:	011
* Assignment:		1
* Date:				18 November 2015
* Professor:		Sv. Ranev
* Purpose:			Preprocessor directives, type declarations and prototypes
*					necessary for symbol table  implementation as required for 
*					CST8152, Assignment #1, Fall 2015.
* Function list:	st_create(), st_install(), st_lookup(), st_update_type(),
*					std_update_value(), st_get_type(), st_destroy(), st_print(),
*					st_store(), st_sort()
*******************************************************************************/

#ifndef STABLE_H_
#define STABLE_H_

/* Declare Symbol Table Database (STDB) stucture*/

typedef union InitialValue {
	int int_val;		/* Integer variable initial value */
	float fpl_val;		/* Floating-point variable initial value */
	int str_offset;		/* String variable initial value (offset) */
}InitialValue;

typedef struct SymbolTableVidRecord {
	unsigned short status_field;	/* Variable record status field */
	char* plex;						/* Pointer to lexeme (VID) in CA */
	int o_line;						/* Line of first occurence */
	InitialValue i_value;			/* Variable initial value */
	size_t reserved;				/* Reserved for future use */
}STVR;

typedef struct SybolTableDescriptor {
	STVR* pstvr;		/* Pointer to array of STVR */
	int st_size;		/* Size in number of STVR elements */
	int st_offset;		/* Offset in number of STVR elements */
	Buffer* plsBD;		/* Pointer to leme storage BufferDescriptor */
}STD;

/* Function prototypes */

STD st_create(int st_size);
int st_install(STD sym_table, char* lexeme, char type, int line);
int st_lookup(STD sym_table, char* lexeme);
int st_update_type(STD sym_table, int vid_offset, char v_type);
int std_update_value(STD sym_table, int vid_offset, InitialValue i_value);
char st_get_type(STD sym_table, int vid_offset);
void st_destroy(STD sym_table);
int st_print(STD sym_table);
int st_store(STD sym_table);
int st_sort(STD symb_table, char s_order);

#endif
