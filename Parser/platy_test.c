/* The #define _CRT_SECURE_NO_WARNINGS should be used in MS Visual Studio projects
* to suppress the warnings about using "unsafe" functions like fopen()
* and standard sting library functions defined in string.h.
* The define does not have any effect in other compilers  projects.
*/
#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h> /* Constants for calls to exit()*/
#include <string.h>
#include <stdarg.h>
#include "buffer.h"
#include "token.h"
#include "stable.h"

/* Input buffer parameters */
#define INIT_CAPACITY 200		/* initial buffer capacity */
#define INC_FACTOR 15			/* increment factor */

/* String Literal Table parameters */
#define STR_INIT_CAPACITY 100	/* initial string literal table capacity */
#define STR_CAPACITY_INC  50	/* initial string literal table capacity inc */

/* Symbol Table default size */
#define ST_DEF_SIZE 100 

/* Global objects - variables */
static Buffer *sc_buf;	/* pointer to input (source) buffer */
Buffer * str_LTBL;		/* this buffer implements String Literal Table */
int scerrnum;			/* run-time error number = 0 by default (ANSI) */
STD sym_table;			/* Symbol Table Descriptor */

/*external objects */
extern int synerrno		/* number of syntax errors reported by the parser */;
extern int line;		/* source code line number - defined in scanner.c */

/* function declarations (prototypes) */
extern void parser(Buffer * sc_buf);

/* For testing purposes */
extern void scanner_init(Buffer * sc_buf);
/* extern Token malpar_next_token(Buffer * sc_buf);*/

void err_printf(char *fmt, ...);
void display(Buffer *ptrBuffer);
long get_filesize(char *fname);
void garbage_collect(void);

int main(void) {
	FILE *fi;						/* input file handle */
	int loadsize = 0;				/* the size of the file loaded in the buffer */
	int st_def_size = ST_DEF_SIZE;	/* Sumbol Table default size */

	/* create a source code input buffer - multiplicative mode */
	sc_buf = b_create(INIT_CAPACITY, INC_FACTOR, 'm');
	if (sc_buf == NULL){
		err_printf("Could not create source buffer");
		exit(EXIT_FAILURE);
	}

	/* create symbol table */
	sym_table = st_create(st_def_size);
	if (!sym_table.st_size){
		err_printf("Could not create symbol table");
		exit(EXIT_FAILURE);
	}

	/*open source file */
	if ((fi = fopen("using.txt", "r")) == NULL){
		err_printf("Cannot open file: ");
		exit(1);
	}

	/* load source file into input buffer  */
	printf("Reading file....Please wait\n");
	loadsize = b_load(fi, sc_buf);
	if (loadsize == R_FAIL_1)
		err_printf("Error in loading buffer.");

	/* close source file */
	fclose(fi);

	/*find the size of the file  */
	if (loadsize == LOAD_FAIL){
		printf("The input file is not completely loaded.");
		printf("Input file size: \n");
	}

	/* pack and display the source buffer */
	if (b_pack(sc_buf)){
		display(sc_buf);
	}

	/* create string Literal Table */
	str_LTBL = b_create(INIT_CAPACITY, INC_FACTOR, 'a');
	if (str_LTBL == NULL){
		err_printf("Could not create string buffer");
		exit(EXIT_FAILURE);
	}

	/*registrer exit function */
	atexit(garbage_collect);

	/*Testbed for buffer, scanner,symbol table and parser*/

	/* Initialize scanner  */
	scanner_init(sc_buf);

	/* Add SEOF to input buffer */
	b_addc(sc_buf, EOF);

	/* Start parsing */
	printf("\nParsing the source file...\n\n");
	parser(sc_buf);

	/* print Symbol Table */
	/*
	if(sym_table.st_size && sort_st){
	st_print(sym_table);
	if(sort_st){
	printf("\nSorting symbol table...\n");
	st_sort(sym_table,sort_st);
	st_print(sym_table);
	}
	}
	*/

	return (EXIT_SUCCESS); /* same effect as exit(0) */
}/*end of main */

/* Error printing function with variable number of arguments
*/
void err_printf(char *fmt, ...){

	va_list ap;
	va_start(ap, fmt);

	(void)vfprintf(stderr, fmt, ap);
	va_end(ap);

	/* Move to new line */
	if (strchr(fmt, '\n') == NULL)
		fprintf(stderr, "\n");
}

/* The function return the size of an open file
*/
long get_filesize(char  *fname){
	FILE *input;
	long flength;
	input = fopen(fname, "r");
	if (input == NULL)
		err_printf("%s%s", "Cannot open file: ", fname);
	fseek(input, 0L, SEEK_END);
	flength = ftell(input);
	fclose(input);
	return flength;
}

/* The function display buffer contents
*/
void display(Buffer *ptrBuffer){
	printf("\nPrinting input buffer parameters:\n\n");
	printf("The capacity of the buffer is:  %d\n", b_capacity(ptrBuffer));
	printf("The current size of the buffer is:  %d\n", b_size(ptrBuffer));
	printf("\nPrinting input buffer contents:\n\n");
	b_print(ptrBuffer);
}

/* the functions frees the allocated memory */
void garbage_collect(void){
	if (synerrno)
		printf("\nSyntax errors: %d\n", synerrno);
	printf("\nCollecting garbage...\n");
	b_destroy(sc_buf);
	b_destroy(str_LTBL);
	st_destroy(sym_table);
}
