/* The #define _CRT_SECURE_NO_WARNINGS should be used in MS Visual Studio projects
* to suppress the warnings about using "unsafe" functions like fopen()
* and standard sting library functions defined in string.h.
* The define does not have any effect in other compiler projects.
*/
#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h> /* Constants for calls to exit() */

#include <string.h>
#include <stdarg.h>

#include "buffer.h"
#include "token.h"
#include "stable.h" /* Do not remove this line. SiR */
#include "stable.h" /* Do not remove this line. SiR */

/*******************************************************************************
*    CONSTANT DEFINITIONS
*******************************************************************************/
/* Input buffer parameters */
#define INIT_CAPACITY 200       /* initial buffer capacity */
#define INC_FACTOR 15           /* increment factor */

/* String Literal Table parameters */
#define STR_INIT_CAPACITY 100   /* initial string literal table capacity */
#define STR_CAPACITY_INC  50    /* initial string literal table capacity inc */

/* Symbol Table default size */
#define ST_DEF_SIZE 100 

/*******************************************************************************
*    GLOBAL VARIABLES
*******************************************************************************/
static Buffer *sc_buf;  /* pointer to input (source) buffer */
Buffer * str_LTBL;      /* this buffer implements String Literal Table */
int scerrnum;           /* run-time error number = 0 by default (ANSI) */
STD sym_table;          /* Symbol Table Descriptor */

/* external objects */
extern int line;        /* source code line numbers - defined in scanner.c */
extern int scanner_init(Buffer * sc_buf);
extern Token mlwpar_next_token(Buffer * sc_buf);

/*******************************************************************************
*    FUNCTION PROTOTYPES
*******************************************************************************/
void err_printf(char *fmt, ...);
void display(Buffer *ptrBuffer);
long get_filesize(char *fname);
void garbage_collect(void);


int main(int argc, char ** argv) {

    FILE *fi;                       /* input file handle */
    Token t;                        /* token produced by the scanner */
    int loadsize = 0;               /* the size of the file loaded in the buffer */
    int st_def_size = ST_DEF_SIZE;  /* Symbol Table default size */
    char sort_st = 0;               /* Symbol Table sort switch */

    /* create a source code input buffer - multiplicative mode */
    sc_buf = b_create(INIT_CAPACITY, INC_FACTOR, 'm');
    if (sc_buf == NULL) {
        err_printf("%s", "stable: Could not create source buffer");
        exit(EXIT_FAILURE);
    }

    /* create symbol table */
    sym_table = st_create(st_def_size);
    if (!sym_table.st_size){
        err_printf("%s", "stable: Could not create symbol table");
        exit(EXIT_FAILURE);
    }

    /*open source file */
    if ((fi = fopen("name%.txt", "r")) == NULL){
        err_printf("%s", "stable: Cannot open file");
        exit(1);
    }

    /* load source file into input buffer  */
    printf("Reading file ....Please wait\n");
    loadsize = b_load(fi, sc_buf);
    if (loadsize == R_FAIL_1)
        err_printf("%s", "stable: Error in loading buffer.");

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
        err_printf("%s", "Could not create string buffer");
        exit(EXIT_FAILURE);
    }

    /*registrer exit function */
    atexit(garbage_collect);

    /*** TEST BED for the scanner and the symbol table ***/

    /* add SEOF to input program buffer*/
    b_addc(sc_buf, '\0');

    /* Initialize the scanner*/
    if (scanner_init(sc_buf)){
        ;
        err_printf("%s", "Empty program buffer - scanning canceled");
        exit(1);
    }

    printf("Scanning source file...\n\n");

    do{
        t = mlwpar_next_token(sc_buf);
    } while (t.code != SEOF_T);

    /* print Symbol Table */
    if (sym_table.st_size){
        st_print(sym_table);
        if (sort_st){
            printf("\nSorting symbol table...\n");
            st_sort(sym_table, sort_st);
            st_print(sym_table);
        }
    }

    /*** TEST BED for type and update functions ***/

    /*** TEST BED for bonus ***/

    return (0); /* same effect as exit(EXIT_SUCCESS) */
}

/* Error printing function with variable number of arguments */
void err_printf(char *fmt, ...){
    va_list ap;
    va_start(ap, fmt);

    (void)vfprintf(stderr, fmt, ap);
    va_end(ap);

    /* Move to new line */
    if (strchr(fmt, '\n') == NULL)
        fprintf(stderr, "\n");
}

/* The function display buffer contents */
void display(Buffer *ptrBuffer){
    printf("\nPrinting input buffer parameters:\n\n");
    printf("The capacity of the buffer is:  %d\n", b_capacity(ptrBuffer));
    printf("The current size of the buffer is:  %d\n", b_size(ptrBuffer));
    printf("The reallocation flag is:   %d\n", b_rflag(ptrBuffer));
    printf("\nPrinting input buffer contents:\n\n");
    b_print(ptrBuffer);
}

/* The function frees all dynamically allocated memory.
This function is always called despite how the program 
terminates - normally or abnormally. */
void garbage_collect(void){
    printf("\nCollecting garbage...\n");
    b_destroy(sc_buf);
    b_destroy(str_LTBL);
    st_destroy(sym_table);
}
