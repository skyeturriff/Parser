/*******************************************************************************
* File Name:		scanner.c
* Compiler:			MS Visual Studio 2013
* Author:			S^R with additions by Skye Turriff
* Course:			CST 8152 - Compilers, Lab Section:	011
* Assignment:		1
* Date:				5 October 2015
* Professor:		Sv. Ranev
* Purpose:			Functions implementing a Lexical Analyzer (Scanner)
*					as required for CST8152, Assignment #2. Function
*					scanner_init() must be called before using the scanner.
* Function list:
*******************************************************************************/

/* The #define _CRT_SECURE_NO_WARNINGS should be used in MS VS projects
* to suppress the warnings about using "unsafe" functions like fopen()
* and standard sting library functions defined in string.h.
* The define does not have any effect in Borland compiler projects. */
#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>   /* standard input / output */
#include <ctype.h>   /* conversion functions */
#include <stdlib.h>  /* standard library functions and constants */
#include <string.h>  /* string functions */
#include <limits.h>  /* integer types constants */
#include <float.h>   /* floating-point types constants */

/* #define NDEBUG */ /* to suppress assert() call */
#include <assert.h>	 /* assert() prototype */

/* project header files */
#include "buffer.h"
#include "token.h"
#include "table.h"
#include "stable.h"

#define DEBUG  /* for conditional processing */
#undef  DEBUG


/*******************************************************************************
*    VARIABLES
*******************************************************************************/

/* Global variables */
extern Buffer * str_LTBL;	/* String literal table */
extern int scerrnum;		/* Run-time error number */

int line;					/* Current line number of the source code */

/* Local(file) global variables */
static Buffer *lex_buf;		/* Pointer to temporary lexeme buffer */

/*******************************************************************************
*    FUNCTION PROTOTYPES
*******************************************************************************/

/* Static(local) functions */
static int char_class(char c);					/* Character class function */
static int get_next_state(int, char, int *);	/* State machine function */
static int iskeyword(char * kw_lexeme);			/* Keywords lookup function */
static long atool(char * lexeme);	/* Converts octal string to decimal value */

/*******************************************************************************
* Purpose:
* Author:			Skye Turriff
* History:
* Called functions:
* Parameters:
* Return value:
* Algorithm:
*******************************************************************************/
int scanner_init(Buffer * sc_buf) {
	if (b_isempty(sc_buf))
		return EXIT_FAILURE;

	/* in case the buffer has been read previously */
	b_setmark(sc_buf, 0);
	b_retract_to_mark(sc_buf);
	b_reset(str_LTBL);
	line = 1;
	return EXIT_SUCCESS;
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
Token mlwpar_next_token(Buffer * sc_buf) {
	Token t;				/* token to return after recognition */
	unsigned char c;		/* input symbol */
	int state = 0;			/* initial state of the FSM */
	short lexstart;			/* start offset of a lexeme in the input buffer */
	short lexend;			/* end offset of a lexeme in the input buffer */
	int accept = NOAS;		/* type of state - initially not accepting */

	/* The following are used when recognizing string literals, comments */
	short temp_offset;		/* storage for offset to beginning of string */
	char temp_char;			/* storage for char found in string or comment */
	short str_len;			/* length of the string */
	int i;					/* loop counter */

	while (1) { /* Endless loop broken by token returns. Will generate warning */

		c = b_getc(sc_buf);	/* Get the next symbol from the input buffer */

		/* Test for end of source file */
		if (c == '\0' || c == 255) {
			t.code = SEOF_T;
			return t;
		}

		/* Test for white space */
		else if (c == ' ' || c == '\t' || c == 'VT' || c == 'FF')
			continue;

		/* Test for line terminator */
		else if (c == '\n' || c == '\r') {
			line++;
			continue;
		}

		/* Test for left brace */
		else if (c == '{') {
			t.code = LBR_T;
			return t;
		}

		/* Test for right brace */
		else if (c == '}') {
			t.code = RBR_T;
			return t;
		}

		/* Test for left parenthesis */
		else if (c == '(') {
			t.code = LPR_T;
			return t;
		}

		/* Test for right parenthesis */
		else if (c == ')') {
			t.code = RPR_T;
			return t;
		}

		/* Test for comma */
		else if (c == ',') {
			t.code = COM_T;
			return t;
		}

		/* Test for semi-colon */
		else if (c == ';') {
			t.code = EOS_T;
			return t;
		}

		/* Test for string concatenation operator*/
		else if (c == '#') {
			t.code = SCC_OP_T;
			return t;
		}

		/* Test for assignment (=) or equals (==) */
		else if (c == '=') {
			if (b_getc(sc_buf) == '=') {	/* equals */
				t.code = REL_OP_T;
				t.attribute.rel_op = EQ;
				return t;
			}
			b_retract(sc_buf);	/* assignment */
			t.code = ASS_OP_T;
			return t;
		}

		/* Test for less than (<) or not equal (<>) */
		else if (c == '<') {
			if (b_getc(sc_buf) == '>') {	/* not equal */
				t.code = REL_OP_T;
				t.attribute.rel_op = NE;
				return t;
			}
			b_retract(sc_buf);	/* less than */
			t.code = REL_OP_T;
			t.attribute.rel_op = LT;
			return t;
		}

		/* Test for greater than */
		else if (c == '>') {
			t.code = REL_OP_T;
			t.attribute.rel_op = GT;
			return t;
		}

		/* Test for addition operator */
		else if (c == '+') {
			t.code = ART_OP_T;
			t.attribute.arr_op = PLUS;
			return t;
		}

		/* Test for subtraction operator */
		else if (c == '-') {
			t.code = ART_OP_T;
			t.attribute.arr_op = MINUS;
			return t;
		}

		/* Test for multiplication operator */
		else if (c == '*') {
			t.code = ART_OP_T;
			t.attribute.arr_op = MULT;
			return t;
		}

		/* Test for division operator */
		else if (c == '/') {
			t.code = ART_OP_T;
			t.attribute.arr_op = DIV;
			return t;
		}

		/* Test for logical operators (.AND., .OR) */
		else if (c == '.') {

			/* Set mark at next symbol, in case of error token */
			b_setmark(sc_buf, b_getc_offset(sc_buf));

			/* Test for logical OR */
			if (b_getc(sc_buf) == 'O') {
				if (b_getc(sc_buf) == 'R') {
					if (b_getc(sc_buf) == '.') {
						t.code = LOG_OP_T;
						t.attribute.log_op = OR;
						return t;
					}
				}
			}

			/* Retract and test for logical AND */
			b_retract_to_mark(sc_buf);
			if (b_getc(sc_buf) == 'A') {
				if (b_getc(sc_buf) == 'N') {
					if (b_getc(sc_buf) == 'D') {
						if (b_getc(sc_buf) == '.') {
							t.code = LOG_OP_T;
							t.attribute.log_op = AND;
							return t;
						}
					}
				}
			}

			/* Retract and set error token */
			b_retract_to_mark(sc_buf);
			t.code = ERR_T;
			t.attribute.err_lex[0] = c;
			t.attribute.err_lex[1] = '\0';
			return t;
		}

		/* Test for comment */
		else if (c == '!') {
			temp_char = b_getc(sc_buf);

			/* Check next symbol, must be '<' for legal comment */
			if (temp_char == '<') {

				/* Skip all symbols until line terminator */
				temp_char = b_getc(sc_buf);
				while (temp_char != '\n' && temp_char != '\r') {

					/* Error if comment does not end in line terminator */
					if ((temp_char == '\0') || (temp_char == EOF)) {
						t.code = ERR_T;
						t.attribute.err_lex[0] = '!';
						t.attribute.err_lex[1] = '<';
						t.attribute.err_lex[2] = '\0';
						b_retract(sc_buf);	/* Next read must recognize SEOF */
						return t;
					}

					/* Keep checking for line terminator */
					temp_char = b_getc(sc_buf);
				}

				/* Process line terminator and continue */
				line++;
				continue;
			}

			/* Process illegal comment token (missing '<') */
			t.code = ERR_T;
			t.attribute.err_lex[0] = c;
			t.attribute.err_lex[1] = temp_char;
			t.attribute.err_lex[2] = '\0';

			/* Skip all symbols until line terminator or SEOF */
			temp_char = b_getc(sc_buf);
			while (temp_char != '\n' && temp_char != '\r') {

				/* If SEOF, retract so it will be recognized next read */
				if ((temp_char == '\0') || (temp_char == EOF)) {
					b_retract(sc_buf);
					return t;
				}

				/* Keep checking for line terminator */
				temp_char = b_getc(sc_buf);
			}

			/* Process line terminator and return error token */
			line++;
			return t;
		}

		/* Test for string literal */
		else if (c == '"') {

			/* Set mark to beginning of string */
			b_setmark(sc_buf, b_getc_offset(sc_buf) - 1);

			/* Check for legal string  */
			temp_char = b_getc(sc_buf);
			while (temp_char != '"') {

				/* If string literal crosses to new line, count it*/
				if (temp_char == '\n' || temp_char == '\r')
					line++;

				/* If SEOF found before closing ", illegal string */
				if ((temp_char == '\0') || (temp_char == EOF)) {
					t.code = ERR_T;
					lexend = b_getc_offset(sc_buf) - 1;
					str_len = lexend - b_mark(sc_buf);
					b_retract_to_mark(sc_buf);

					/* Add string to error token attribute */
					for (i = 0; i < str_len && i < ERR_LEN; i++)
						t.attribute.err_lex[i] = b_getc(sc_buf);
					t.attribute.err_lex[i] = '\0';	/* Make C-type string */

					/* Append "..." and '\0' to lengthy error token attribute,
					and set input buffer offset to end of error string */
					if (str_len > ERR_LEN) {
						t.attribute.err_lex[--i] = '.';
						t.attribute.err_lex[--i] = '.';
						t.attribute.err_lex[--i] = '.';
						b_setmark(sc_buf, lexend);
						b_retract_to_mark(sc_buf);
					}

					/* Return error token */
					return t;
				}

				/* Keep looking for closing " or SEOF */
				temp_char = b_getc(sc_buf);
			}

			/* Closing " found, process legal string literal */
			/* Calculate length of string literal */
			str_len = (b_getc_offset(sc_buf) - 1) - b_mark(sc_buf);
			b_retract_to_mark(sc_buf);

			/* Mark offset in string literal table for token attribute */
			temp_offset = b_size(str_LTBL);

			/* Add string to string literal table, excluding "" */
			for (i = 0; i <= str_len; i++) {
				temp_char = b_getc(sc_buf);
				if (temp_char != '"')
					b_addc(str_LTBL, temp_char);
			}
			b_addc(str_LTBL, '\0');

			/* Return string literal token */
			t.code = STR_T;
			t.attribute.str_offset = temp_offset;
			return t;
		}

		/* Process state transition table */
		else if (isalpha(c) || isdigit(c)) {
			b_setmark(sc_buf, b_getc_offset(sc_buf) - 1);
			state = 0;
			state = get_next_state(state, c, &accept);

			while (accept == NOAS) {
				c = b_getc(sc_buf);
				state = get_next_state(state, c, &accept);
			}

			/* Token found - process final state */
			if (accept == ASWR)
				b_retract(sc_buf);

			/* Get beginning and end of the lexeme */
			lexstart = b_mark(sc_buf);
			lexend = b_getc_offset(sc_buf);// -1;

			/* Create temporary lexeme buffer and store lexeme */
			lex_buf = b_create(100, 0, 'f');
			str_len = lexend - lexstart;
			b_retract_to_mark(sc_buf);
			for (i = 0; i < str_len; i++)
				b_addc(lex_buf, b_getc(sc_buf));
			b_addc(lex_buf, '\0');

			/* Call accepting function, pass pointer to start of lex_buf */
			t = aa_table[state](b_setmark(lex_buf, 0));
			b_destroy(lex_buf);
			return t;
		}

		else {
			t.code = ERR_T;
			t.attribute.err_lex[0] = c;
			t.attribute.err_lex[1] = '\0';
			return t;
		}

	}//end while(1)
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
int get_next_state(int state, char c, int *accept) {
	int col;
	int next;
	col = char_class(c);
	next = st_table[state][col];

#ifdef DEBUG
	printf("Input symbol: %c Row: %d Column: %d Next: %d \n", c, state, col, next);
#endif

	assert(next != IS);

#ifdef DEBUG
	if (next == IS){
		printf("Scanner Error: Illegal state:\n");
		printf("Input symbol: %c Row: %d Column: %d\n", c, state, col);
		exit(1);
	}
#endif

	*accept = as_table[next];
	return next;
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
int char_class(char c) {
	int val;

	if (isalpha(c)) val = 0;			/* Column 0: letter */
	else if (c == '0') val = 1;			/* Column 1: zero */
	else if ((c >= '1') && (c <= '7'))
		val = 2;						/* Column 2: 1-7 */
	else if ((c == '8') || (c == '9'))
		val = 3;						/* Column 3: 8-9 */
	else if (c == '.') val = 4;			/* Column 4: . */
	else if (c == '%') val = 5;			/* Column 5: % */
	else val = 6;						/* Column 6: other */

	return val;
}

/*******************************************************************************
* Purpose:			Processes token recognized by scanner as keyword or an AVID
* Author:			Skye Turriff
* History:			Version 3, 12 December 2015
* Called functions:	iskeyword(), strlen(), strcpy(), st_install(), printf(),
*					st_store(), st_destroy()
* Parameters:		char* lexeme the recognized lexeme
* Return value:		On success, the token, on failure, system exit
* Algorithm:		Check if lexeme is a keyword and return if true. Else,
*					determine variable datatype, shorten lexeme if necessary,
*					and send to install function of symbol table. Check for
*					successful install, return token on success. Else store
*					symbol table, delete memory, and exit.
*******************************************************************************/
Token aa_func02(char *lexeme) {
	int kwt_index;				/* Index into keywors table */
	char lex_8[VID_LEN + 1];	/* Storage for shortened lexeme, if necessary*/
	char type;					/* Type of variable (float or int) */
	Token t;

	/* Check if token is a keyword */
	kwt_index = iskeyword(lexeme);
	if (kwt_index != -1) {
		t.code = KW_T;
		t.attribute.kwt_idx = kwt_index;
		return t;
	}

	/* Create token for AVID */
	t.code = AVID_T;

	/* Determine type */
	if (*lexeme == 'i' || *lexeme == 'o' || *lexeme == 'd' || *lexeme == 'w')
		type = 'I';
	else type = 'F';

	/* If lexeme is longer than VID_LEN, shorten it */
	if (strlen(lexeme) > VID_LEN) {
		strncpy(lex_8, lexeme, VID_LEN);
		lex_8[VID_LEN] = '\0';		/* Make C-type string */
		t.attribute.vid_offset = st_install(sym_table, lex_8, type, line);
	}
	else t.attribute.vid_offset = st_install(sym_table, lexeme, type, line);

	/* Check for successful install */
	if (t.attribute.str_offset == ERR_FAIL1) {
		printf("\nError: The Symbol Table is full - install failed.\n\n");
		st_store(sym_table);
		st_destroy(sym_table);
		exit(1);
	}

	return t;
}

/*******************************************************************************
* Purpose:			Processes token recognized by scanner as SVID
* Author:			Skye Turriff
* History:			Version 3, 12 December 2015
* Called functions:	strlen(), strcpy(), st_install(), printf(), st_store(),
*					st_destroy()
* Parameters:		char* lexeme the recognized lexeme
* Return value:		On success, the token, on failure, system exit
* Algorithm:		Shorten lexeme if necessary, and send to install function
*					of symbol table. Check for successful install, return token
*					on success. Else store symbol table, delete memory, and exit
*******************************************************************************/
Token aa_func03(char *lexeme) {
	Token t;
	char lex_8[VID_LEN+1];	/* Storage for shortened lexeme, if necessary*/

	/* Create token for SVID */
	t.code = SVID_T;

	/* If lexeme is longer than VID_LEN, shorten it */
	if (strlen(lexeme) > VID_LEN) {
		strncpy(lex_8, lexeme, VID_LEN - 1);
		lex_8[VID_LEN - 1] = '%';	/* Append SVID terminator */
		lex_8[VID_LEN] = '\0';		/* Make C-type string */
		t.attribute.vid_offset = st_install(sym_table, lex_8, 'S', line);
	} 
	else t.attribute.vid_offset = st_install(sym_table, lexeme, 'S', line);

	/* Check for successful install */
	if (t.attribute.str_offset == ERR_FAIL1) {
		printf("Error: The Symbol Table is full - install failed\n");
		st_store(sym_table);
		st_destroy(sym_table);
		exit(1);
	}

	return t;
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
Token aa_func05(char *lexeme) {
	Token t;
	long value;

	if (strlen(lexeme) > INL_LEN)
		return aa_table[ES](lexeme);

	value = atol(lexeme);

	/* Catch overflow error and produce error token */
	if (value > PLATY_MAX)
		return aa_table[ES](lexeme);

	/* Create token for DIL */
	t.code = INL_T;
	t.attribute.int_value = (int)value;

	return t;
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
Token aa_func08(char *lexeme) {
	Token t;
	double floatValue = atof(lexeme);

	/* FPL can't be more that 4 bytes in memory and must be non-negative */
	/* Catch overflow error and produce error token */
	if (floatValue > FLT_MAX || (floatValue < FLT_MIN && floatValue != 0.0))
		return aa_table[ES](lexeme);

	/* Create token for FPL */
	t.code = FPL_T;
	t.attribute.flt_value = (float)floatValue;

	return t;
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
Token aa_func10(char *lexeme) {
	Token t;
	long value;

	if (strlen(lexeme) > INL_LEN + 1)
		return aa_table[ES](lexeme);

	value = atool(lexeme);

	/* Catch overflow error and produce error token */
	if (value > PLATY_MAX)
		return aa_table[ES](lexeme);

	/* Create token for OIL */
	t.code = INL_T;
	t.attribute.int_value = (int)value;

	return t;
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
Token aa_func11(char *lexeme) {
	return aa_table[ES](lexeme);
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
Token aa_func12(char *lexeme) {
	Token t;
	unsigned int i;
	t.code = ERR_T;
	for (i = 0; (i < strlen(lexeme)) && (i < ERR_LEN); i++)
		t.attribute.err_lex[i] = lexeme[i];
	t.attribute.err_lex[i] = '\0';
	return t;
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
Token aa_func13(char *lexeme) {
	return aa_table[ES](lexeme);
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
int iskeyword(char * kw_lexeme) {
	unsigned int i;

	for (i = 0; i < KWT_SIZE; i++) {
		if (!strcmp(kw_lexeme, kw_table[i]))
			return i;
	}

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
static long atool(char * lexeme) {
	return strtol(lexeme, NULL, 8);
}