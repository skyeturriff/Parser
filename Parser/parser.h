/*******************************************************************************
* File Name:		parser.h
* Compiler:			MS Visual Studio 2013
* Author:			Skye Turriff
* Course:			CST 8152 - Compilers, Lab Section:	011
* Assignment:		4
* Date:				7 December 2015
* Professor:		Sv. Ranev
* Purpose:			Preprocessor directives, type declarations and prototypes
*					necessary for parser implementation as required for CST8152,
*					Assignment #4, Fall 2015.
* Function list:	
*******************************************************************************/
const int NO_ATTR = -1;
typedef enum KeywordTableIndex { 
	ELSE, IF, INPUT, OUTPUT, PLATYPUS, REPEAT, THEN, USING 
} Kwt_Idx;

/* External variables */
extern int scerrnum;			/* Run-time error number */
extern int line;				/* Current line number of the source code */
extern char* kw_table[];		/* Keyword table lookup */
extern Buffer* str_LTBL;		/* String Literal Table */

/* External procedures */
extern Token mlwpar_next_token(Buffer*);	/* Get next input token */

/* Global variables */
int synerrno;					/* Number of syntax errors reported by parser */

/* Local global variables */
static Token lookahead;			/* Current input token */
static Buffer* sc_buf;			/* Pointer to input (source) buffer */

/* Function prototypes */
void parser(Buffer*);		/* Initialize parser */
void match(int, int);		/* Match two tokens */
void syn_eh(int);			/* Handle error */
void syn_printe(void);		/* Print error message */
void gen_incode(char*);		/* Print result message */
void program(void);			/* Process program production */
void opt_statement(void);
void statements(void);
void statements_p(void);
void statement(void);
void assignment_statement(void);
void assignment_expression(void);
void selection_statement(void);
void iteration_statement(void);
void input_statement(void);
void variable_list(void);
void variable_list_p(void);
void variable_identifier(void);
void output_statement(void);
void output_statement_p(void);
void arithmetic_expression(void);
void unary_arithmetic_expression(void);
void additive_arithmetic_expression(void);
void additive_arithmetic_expression_p(void);
void multiplicative_arithmetic_expression(void);
void multiplicative_arithmetic_expression_p(void);
void primary_arithmetic_expression(void);
void string_expression(void);
void string_expression_p(void);
void primary_string_expression(void);
void conditional_expression(void);
void logical_or_expression(void);
void logical_or_expression_p(void);
void logical_and_expression(void);
void logical_and_expression_p(void);
void relation_expression(void);
void relation_operator(void);
void primary_a_relational_expression(void);
void primary_s_relational_expression(void);
