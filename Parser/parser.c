/*******************************************************************************
* File Name:		parser.c
* Compiler:			MS Visual Studio 2013
* Author:			Skye Turriff
* Course:			CST 8152 - Compilers, Lab Section:	011
* Assignment:		4
* Date:				7 December 2015
* Professor:		Sv. Ranev
* Purpose:			Function definition for protypes declared in parser.h
* Function list:	parser(), match(), syn_eh(), syn_printe(), gen_incode(),
*					program(), opt_statements(), statements(), statements_p(),
*					statement(), assignment_statement(), assignment_expression()
*					selection_statement(), iteration_statement(),
*					input_statement(), variable_list(), variable_list_p(),
*					variable_identifier(), output_statement(), 
*					output_statement_p(), arithmetic_expression(), 
*					unary_arithmetic_exp(), additive_arithmetic_exp(), 
*					additive_arithmetic_exp_p(), multiplicative_arithmetic_exp()
*					multiplicative_arithmetic_exp_p(), primary_arithmetic_exp(),
*					string_expression(), string_expression_p(),
*					primary_string_expression(), conditional_expression(),
*					logical_or_expression(), logical_or_expression_p(), 
*					logical_and_expression(), logical_and_expression_p(),
*					relational_expression(), relational_operator(), 
*					primary_a_rel_exp(), primary_s_rel_exp()
*******************************************************************************/
#include <stdlib.h>
#include "buffer.h"
#include "token.h"
#include "stable.h"
#include "parser.h"

/*******************************************************************************
* Purpose:			Initialize input buffer storing the input to be parsed, get
*					first token from input, and begin parsing at production for
*					start symbol of the grammar
* Author:			Skye Turriff
* History:			Version 1, 7 December 2015
* Called functions:	mlwarpar_next_token(), program(), match(), gen_incode()
* Parameters:		Buffer* the input buffer 
* Return value:		None
*******************************************************************************/
void parser(Buffer* in_buf) {
	sc_buf = in_buf;
	lookahead = mlwpar_next_token(sc_buf);
	program(); match(SEOF_T, NO_ATTR);
	gen_incode("PLATY: Source file parsed");
}

/*******************************************************************************
* Purpose:			Match the current input token (lookahead) with the token
*					the parser expects, according to the grammar
* Author:			Skye Turriff
* History:			Version 1, 7 December 2015
* Called functions: syn_eh(), mlwpar_next_token(), syn_printe()
* Parameters:		The token code expects by the parser, and (if applicable)
*					the corresponding token attribute expected by the parser
* Return value:		None
* Algorithm:		If the required token code does not match the lookahead,
*					call error handler and return. Else, the token codes match.
*					If the token requires a matching attribute code (ART_OP_T,
*					REL_OP_T, LOG_OP_T, and KW_T), check that the current
*					token's attribute matches the required attribute. If not,
*					call error handler and return. Else, match is fully 
*					successful, advance to next lookahead. If next lookahead is
*					an error token, print it, advance the lookahead once more, 
*					and increment the record of syntax errors.
*******************************************************************************/
void match(int pr_token_code, int pr_token_attribute) {
	/* Call error handler if no match */
	if (lookahead.code != pr_token_code) {
		syn_eh(pr_token_code); 
		return;
	}

	/* If match to SEOF_T, no more tokens, so return */
	if (pr_token_code == SEOF_T) return;

	/* Match attribute if token has one */
	switch (pr_token_code) {
	case ART_OP_T: case REL_OP_T: case LOG_OP_T: case KW_T:
		if (lookahead.attribute.get_int != pr_token_attribute) {
			syn_eh(pr_token_code); 
			return;
		}
	}

	/* Successful match, advance input */
	lookahead = mlwpar_next_token(sc_buf);
	if (lookahead.code == ERR_T) {
		syn_printe();
		lookahead = mlwpar_next_token(sc_buf);
		++synerrno;
	}
}

/*******************************************************************************
* Purpose:			Implement a simple panic mode error recover when the parser
*					encounters an incorrect (not expected by grammar) token
* Author:			Skye Turriff
* History:			Version 1, 7 December 2015
* Called functions: syn_printe(), exit(), mlwpar_next_token()
* Parameters:		The token code required by the grammar
* Return value:		None
* Algorithm:		Print the error description of the current lookahead (the
*					error token). Then, advance the lookahead until a token
*					matching the one required by the grammar is found. If the
*					end of the input is reached before a matching token found,
*					the program exits. Else a matching token is found, and if
*					the match was not SEOF_T, the lookahead is advanced again
*					so that parsing can resume.
*******************************************************************************/
void syn_eh(int sync_token_code) {
	syn_printe();
	++synerrno;

	/* Advance input token until match to sync_token_code or SEOF_T */
	while (lookahead.code != sync_token_code) {
		if (lookahead.code == SEOF_T)
			exit(synerrno);
		lookahead = mlwpar_next_token(sc_buf);
	}

	/* If matched token was other than SEOF_T, advance input token */
	if (sync_token_code != SEOF_T)
		lookahead = mlwpar_next_token(sc_buf);
}

/*******************************************************************************
* Purpose:			Prints an error message regarding the current lookahead
* Author:			Svillen Ranev
* History:			Version 1, 7 December 2015
* Called functions:	printf(), b_setmark()
* Parameters:		None
* Return value:		None
*******************************************************************************/
void syn_printe(void) {
	Token t = lookahead;

	printf("PLATY: Syntax error:  Line:%3d\n", line);
	printf("*****  Token code:%3d Attribute: ", t.code);

	switch (t.code) {
	case  ERR_T:		/* ERR_T    0   Error token */
		printf("%s\n", t.attribute.err_lex);
		break;
	case  SEOF_T:		/* SEOF_T   1   Source end-of-file token */
		printf("NA\n");
		break;
	case  AVID_T:		/* AVID_T   2   Arithmetic Variable identifier token */
	case  SVID_T:		/* SVID_T   3  String Variable identifier token */
		printf("%s\n", sym_table.pstvr[t.attribute.get_int].plex);
		break;
	case  FPL_T:		/* FPL_T    4  Floating point literal token */
		printf("%5.1f\n", t.attribute.flt_value);
		break;
	case INL_T:			/* INL_T    5   Integer literal token */
		printf("%d\n", t.attribute.get_int);
		break;
	case STR_T:			/* STR_T    6   String literal token */
		printf("%s\n", b_setmark(str_LTBL, t.attribute.str_offset));
		break;
	case SCC_OP_T:		/*			7   String concatenation operator token */
		printf("NA\n");
		break;
	case  ASS_OP_T:		/* ASS_OP_T 8   Assignment operator token */
		printf("NA\n");
		break;
	case  ART_OP_T:		/* ART_OP_T 9   Arithmetic operator token */
		printf("%d\n", t.attribute.get_int);
		break;
	case  REL_OP_T:		/* REL_OP_T 10   Relational operator token */
		printf("%d\n", t.attribute.get_int);
		break;
	case  LOG_OP_T:		/* LOG_OP_T	11  Logical operator token */
		printf("%d\n", t.attribute.get_int);
		break;
	case  LPR_T:		/* LPR_T	12  Left parenthesis token */
		printf("NA\n");
		break;
	case  RPR_T:		/* RPR_T	13  Right parenthesis token */
		printf("NA\n");
		break;
	case LBR_T:			/*			14   Left brace token */
		printf("NA\n");
		break;
	case RBR_T:			/*			15  Right brace token */
		printf("NA\n");
		break;
	case KW_T:			/*			16   Keyword token */
		printf("%s\n", kw_table[t.attribute.get_int]);
		break;
	case COM_T:			/*			17   Comma token */
		printf("NA\n");
		break;
	case EOS_T:			/*			18  End of statement *(semi-colon) */
		printf("NA\n");
		break;
	default:
		printf("PLATY: Scanner error: invalid token code: %d\n", t.code);
	}
}

/*******************************************************************************
* Purpose:			Prints a string message
* Author:			Skye Turriff
* History:			Version 1, 7 December 2015
* Called functions:	prinf()
* Parameters:		char* message the string to print
* Return value:		None
*******************************************************************************/
void gen_incode(char* message) {
	printf("%s\n", message);
}

/*******************************************************************************
* Purpose:			Parse Platypus program syntax
* Author:			Skye Turriff
* History:			Version 1, 10 December 2015
* Called functions:	match(), opt_statements(), gen_incode()
* Parameters:		None
* Return value:		None
*
*<program> -> PLATYPUS {<opt_statements>} SEOF
*
* FIRST(<program>) = { KW_T(PLATYPUS) }
*******************************************************************************/
void program(void) {
	match(KW_T, PLATYPUS); 
	match(LBR_T, NO_ATTR); 
	opt_statements();
	match(RBR_T, NO_ATTR);
	gen_incode("PLATY: Program parsed");
}

/*******************************************************************************
* Purpose:			Parse Platypus opt_statements syntax
* Author:			Skye Turriff
* History:			Version 1, 10 December 2015
* Called functions:	statements(), gen_incode()
* Parameters:		None
* Return value:		None
*
* <opt_statements> -> <statements> | e
*
* FIRST(<opt_statements>) = { AVID_T, SVID_T, KW_T(IF), KW_T(USING), 
*	KW_T(INPUT), KW_T(OUTPUT), e }
*******************************************************************************/
void opt_statements(void) {
	switch (lookahead.code) {
	case AVID_T: case SVID_T:		/* Variable identifiers*/
		statements(); 
		break;
	case KW_T:						/* IF, USING, INPUT, or OUTPUT */
		switch (lookahead.attribute.get_int) {
		case IF:
		case USING:
		case INPUT:
		case OUTPUT:
			statements(); return;
		}
	default:						/* Empty - optional statements */
		gen_incode("PLATY: Opt_statements parsed");
	}
}

/*******************************************************************************
* Purpose:			Parse Platypus statements syntax
* Author:			Skye Turriff
* History:			Version 1, 10 December 2015
* Called functions: statement(), statements_p()
* Parameters:		None
* Return value:		None
*
* <statements> -> <statement><statements’>
*
* FIRST(<statements>) = { AVID_T, SVID_T, KW_T(IF), KW_T(USING), 
*	KW_T(INPUT), KW_T(OUTPUT) }
*******************************************************************************/
void statements(void) {
	statement(); statements_p();
}

/*******************************************************************************
* Purpose:			Parse Platypus statements_p syntax
* Author:			Skye Turriff
* History:			Version 1, 10 December 2015
* Called functions: statement(), statements_p()
* Parameters:		None
* Return value:		None
*
* <statements'> -> <statement><statements'> | e
*
* FIRST(<statements'>) = { AVID_T, SVID_T, KW_T(IF), KW_T(USING),
*	KW_T(INPUT), KW_T(OUTPUT), e}
*******************************************************************************/
void statements_p(void) {
	switch (lookahead.code) {
	case AVID_T: case SVID_T:			/* Variable identifiers*/
		statement(); statements_p(); 
		break;
	case KW_T:							/* IF, USING, INPUT, or OUTPUT */
		switch (lookahead.attribute.get_int) {
		case IF:
		case USING:
		case INPUT:
		case OUTPUT:
			statement(); statements_p(); 
		}
	}
}

/*******************************************************************************
* Purpose:			Parse Platypus statement syntax
* Author:			Skye Turriff
* History:			Version 1, 10 December 2015
* Called functions: assignment_statement(), selection_statement(),
*					input_statement(), output_statement(), iteration_statement()
*					syn_printe()
* Parameters:		None
* Return value:		None
*
* <statement> -> 
*	<assignment statement> | <selection statement> | <iteration statement> 
*	| <input statement> | <output statement>
*
* FIRST(<statement>) = { AVID_T, SVID_T, KW_T(IF), KW_T(USING), KW_T(INPUT), 
*	KW_T(OUTPUT) }
*******************************************************************************/
void statement(void) {
	switch (lookahead.code) {
	case AVID_T: case SVID_T:						/* Variable identifier */
		assignment_statement(); break;
	case KW_T:										/* If it's a keyword */
		switch (lookahead.attribute.get_int) {		/* Must be one of: */
		case IF: selection_statement(); break;		/* IF statement */
		case INPUT: input_statement(); break;		/* INPUT statement */
		case OUTPUT: output_statement(); break;		/* OUTPUT statement */
		case USING: iteration_statement(); break;	/* USING statement */
		default: syn_printe();						/* Else it's an error */
		} break;
	default:
		syn_printe();								/* None of above; error */
	}
}

/*******************************************************************************
* Purpose:			Parse Platypus assignment_statement syntax
* Author:			Skye Turriff
* History:			Version 1, 10 December 2015
* Called functions: assignment_expression(), match(), gen_incode()
* Parameters:		None
* Return value:		None
* 
* <assignment statement> -> <assignment expression>;
*
* FIRST(<assignment statement>) = { AVID_T, SVID_T }
*******************************************************************************/
void assignment_statement(void) {
	assignment_expression(); match(EOS_T, NO_ATTR);
	gen_incode("PLATY: Assignment statement parsed");
}

/*******************************************************************************
* Purpose:			Parse Platypus assignment_expression syntax
* Author:			Skye Turriff
* History:			Version 1, 10 December 2015
* Called functions: match(), arithmetic_expression(), gen_incode(),
*					string_expression(), syn_printe()
* Parameters:		None
* Return value:		None
*
* <assignment expression> -> 
*	AVID = <arithmetic expression> | SVID = <string expression>
*
* FIRST(<assignment expression>) = { AVID_T, SVID_T }
*******************************************************************************/
void assignment_expression(void) {
	switch (lookahead.code) {
	case AVID_T:	/* Arithmetic assignment */
		match(AVID_T, NO_ATTR); match(ASS_OP_T, NO_ATTR);
		arithmetic_expression(); 
		gen_incode("PLATY: Assignment expression (arithmetic) parsed");
		break;
	case SVID_T:	/* String assignment */
		match(SVID_T, NO_ATTR); match(ASS_OP_T, NO_ATTR);
		string_expression(); 
		gen_incode("PLATY: Assignment expression (string) parsed");
		break;
	default:		/* Error token */
		syn_printe();
	}
}

/*******************************************************************************
* Purpose:			Parse Platypus selection_statement syntax
* Author:			Skye Turriff
* History:			Version 1, 10 December 2015
* Called functions: match(), conditional_expression(), opt_statements(),
*					gen_incode();
* Parameters:		None
* Return value:		None
*
* <selection statement> -> 
*	IF (<conditional expression>)  
*	THEN  <opt_statements> 
*	ELSE { <opt_statements> } ;
*
* FIRST(<selection statement>) = { KW_T(IF) }
*******************************************************************************/
void selection_statement(void) {
	match(KW_T, IF); match(LPR_T, NO_ATTR); conditional_expression();
	match(RPR_T, NO_ATTR); match(KW_T, THEN); opt_statements(); 
	match(KW_T, ELSE); match(LBR_T, NO_ATTR); opt_statements(); 
	match(RBR_T, NO_ATTR); match(EOS_T, NO_ATTR);
	gen_incode("PLATY: IF statement parsed");
}

/*******************************************************************************
* Purpose:			Parse Platypus iteration_statement syntax
* Author:			Skye Turriff
* History:			Version 1, 10 December 2015
* Called functions: match(), assignment_expression(), conditional_expression(),
*					opt_statements(), gen_incode()
* Parameters:		None
* Return value:		None
*
* <iteration statement> -> 
*	USING ( <assignment exp> , <conditional exp> , <assignment exp> )
*	REPEAT { <opt_statements> };
*
* FIRST(<iteration statement>) = { KW_T(USING) }
*******************************************************************************/
void iteration_statement(void) {
	match(KW_T, USING); match(LPR_T, NO_ATTR); assignment_expression();
	match(COM_T, NO_ATTR); conditional_expression(); match(COM_T, NO_ATTR);
	assignment_expression(); match(RPR_T, NO_ATTR); match(KW_T, REPEAT); 
	match(LBR_T, NO_ATTR); opt_statements(); match(RBR_T, NO_ATTR); 
	match(EOS_T, NO_ATTR);
	gen_incode("PLATY: USING statement parsed");
}

/*******************************************************************************
* Purpose:			Parse Platypus input_statement syntax
* Author:			Skye Turriff
* History:			Version 1, 10 December 2015
* Called functions: match(), variable_list(), gen_incode()
* Parameters:		None
* Return value:		None
*
* <input statement> -> INPUT (<variable list>);
*
* FIRST(<input statement>) = { KW_T(INPUT) }
*******************************************************************************/
void input_statement(void) {
	match(KW_T, INPUT); match(LPR_T, NO_ATTR); variable_list();
	match(RPR_T, NO_ATTR); match(EOS_T, NO_ATTR);
	gen_incode("PLATY: INPUT statement parsed");
}

/*******************************************************************************
* Purpose:			Parse Platypus variable_list syntax
* Author:			Skye Turriff
* History:			Version 1, 10 December 2015
* Called functions: variable_identifier(), variable_list_p(), gen_incode()
* Parameters:		None
* Return value:		None
*
* <variable list> -> <variable identifier><variable list’>
*
* FIRST(<variable list>) = { AVID_T, SVID_T }
*******************************************************************************/
void variable_list(void) {
	variable_identifier(); variable_list_p();
	gen_incode("PLATY: Variable list parsed");
}

/*******************************************************************************
* Purpose:			Parse Platypus variable_list_p syntax
* Author:			Skye Turriff
* History:			Version 1, 10 December 2015
* Called functions: match(), variable_identifier(), variable_list_p()
* Parameters:		None
* Return value:		None
*
* <variable list’> -> , <variable identifier><variable list’> | e
* 
* FIRST(<variable list’>) = { , e }
*******************************************************************************/
void variable_list_p(void) {

	/* If lookahead if a comma, must continue to parse list,
	else empty list or end of list and so do nothing */
	if (lookahead.code == COM_T) {
		match(COM_T, NO_ATTR); variable_identifier(); variable_list_p();
	} 
}

/*******************************************************************************
* Purpose:			Parse Platypus variable_identitifer syntax
* Author:			Skye Turriff
* History:			Version 1, 10 December 2015
* Called functions: match(), syn_printe() 
* Parameters:		None
* Return value:		None
*
* <variable identifier> ->  AVID | SVID
*
* FIRST(<variable identifier>) = { AVID_T, SVID_T }
*******************************************************************************/
void variable_identifier(void) {
	switch (lookahead.code) {
	case AVID_T:
		match(AVID_T, NO_ATTR); break;
	case SVID_T:
		match(SVID_T, NO_ATTR); break;
	default:
		syn_printe();
	}
}

/*******************************************************************************
* Purpose:			Parse Platypus output_statement syntax
* Author:			Skye Turriff
* History:			Version 1, 10 December 2015
* Called functions: match(), output_statement_p(), gen_incode()
* Parameters:		None
* Return value:		None
*
* <output statement> -> OUTPUT( <output statement’> );
*
* FIRST(<output statement>) = { KW_T(OUTPUT) }
*******************************************************************************/
void output_statement(void) {
	match(KW_T, OUTPUT); match(LPR_T, NO_ATTR); output_statement_p();
	match(RPR_T, NO_ATTR); match(EOS_T, NO_ATTR);
	gen_incode("PLATY: OUTPUT statement parsed");
}

/*******************************************************************************
* Purpose:			Parse Platypus output_statement_p syntax
* Author:			Skye Turriff
* History:			Version 1, 10 December 2015
* Called functions: variable_list(), match(), gen_incode()
* Parameters:		None
* Return value:		None
*
* <output statement’> -> <variable list> | STR_T | e
*
* FIRST(<output statement’>) = { AVID_T, SVID_T, STR_T, e }
*******************************************************************************/
void output_statement_p(void) {
	switch (lookahead.code) {
	case AVID_T: case SVID_T:		/* Output variable list */
		variable_list();  break;
	case STR_T:						/* Output string literal */
		match(STR_T, NO_ATTR);
		gen_incode("PLATY: Output list (string literal) parsed"); 
		break;
	default:						/* Output empty string */
		gen_incode("PLATY: Output list (empty) parsed");
	}
}

/*******************************************************************************
* Purpose:			Parse Platypus arithmetic_expression syntax
* Author:			Skye Turriff
* History:			Version 1, 10 December 2015
* Called functions: unary_arithmetic_exp(), syn_printe()
*					additive_arithmetic_exp(), gen_incode()
* Parameters:		None
* Return value:		None
*
* <arithmetic expression> -> 
*	<unary arithmetic expression> | <additive arithmetic expression>
*
* FIRST(<arithmetic expression>) = { - , + , AVID_T, FPL_T, IL_T, ( }
*******************************************************************************/
void arithmetic_expression(void) {
	switch (lookahead.code) {
	case ART_OP_T:									/* If it's arithmetic op */
		switch (lookahead.attribute.get_int) {		/* Must be one of: */
		case PLUS: case MINUS:						/* + or - */
			unary_arithmetic_exp(); 
			break;
		default:									/* Else it's an error */
			syn_printe();							
			return;
		} 
		break;
	case AVID_T: case FPL_T: case INL_T: case LPR_T:	
		additive_arithmetic_exp(); 
		break;
	default:
		syn_printe();
		return;
	}

	gen_incode("PLATY: Arithmetic expression parsed");
}

/*******************************************************************************
* Purpose:			Parse Platypus unary_arithmetic_exp syntax
* Author:			Skye Turriff
* History:			Version 1, 10 December 2015
* Called functions: match(), primary_arithmetic_exp(), syn_printe(),gen_incode()
* Parameters:		None
* Return value:		None
*
* <unary arithmetic expression> -> 
*	- <primary arithmetic expression> | + <primary arithmetic expression>
*
* FIRST(<unary arithmetic exp>) = { - , + }
*******************************************************************************/
void unary_arithmetic_exp(void) {
	switch (lookahead.attribute.get_int) {
	case PLUS:		
		match(ART_OP_T, PLUS); //primary_arithmetic_exp(); 
		break;
	case MINUS:		
		match(ART_OP_T, MINUS); //primary_arithmetic_exp();
		break;
	default:
		syn_printe(); return;
	}

	primary_arithmetic_exp();
	gen_incode("PLATY: Unary arithmetic expression parsed");
}

/*******************************************************************************
* Purpose:			Parse Platypus additive_arithmetic_exp syntax
* Author:			Skye Turriff
* History:			Version 1, 10 December 2015
* Called functions: multiplicative_arithmetic_exp(), 
*					additive_arithmetic_exp_p()
* Parameters:		None
* Return value:		None
*
* <additive arithmetic expression> -> 
*	<multiplicative arithmetic expression><additive arithmetic expression’>
*
* FIRST(<additive arithmetic exp>) = { AVID_T, FPL_T, IL_T, ( }
*******************************************************************************/
void additive_arithmetic_exp(void) {
	multiplicative_arithmetic_exp(); additive_arithmetic_exp_p();
}

/*******************************************************************************
* Purpose:			Parse Platypus additive_arithmetic_exp_p syntax
* Author:			Skye Turriff
* History:			Version 1, 11 December 2015
* Called functions: match(), multiplicative_arithmetic_exp(),
*					additive_arithmetic_exp(), gen_incode()
* Parameters:		None
* Return value:		None
*
* <additive arithmetic expression’> -> 
*	+ <multiplicative arithmetic expression><additive arithmetic expression’>
*	| - <multiplicative arithmetic expression><additive arithmetic expression’>
*	| e
*
* FIRST(<additive arithmetic exp’>) = { + , - , e }
*******************************************************************************/
void additive_arithmetic_exp_p(void) {

	if (lookahead.code == ART_OP_T && lookahead.attribute.get_int == PLUS) {
		match(ART_OP_T, PLUS); multiplicative_arithmetic_exp();
		additive_arithmetic_exp_p();
		gen_incode("PLATY: Additive arithmetic expression parsed");
	} 
	else if (lookahead.code == ART_OP_T && lookahead.attribute.get_int == MINUS) {
		match(ART_OP_T, MINUS); multiplicative_arithmetic_exp();
		additive_arithmetic_exp_p();
		gen_incode("PLATY: Additive arithmetic expression parsed");
	}
}

/*******************************************************************************
* Purpose:			Parse Platypus multiplicative_arithmetic_exp syntax
* Author:			Skye Turriff
* History:			Version 1, 11 December 2015
* Called functions: primary_arithmetic_exp(), multiplicative_arithmetic_exp_p()
* Parameters:		None
* Return value:		None
*
* <multiplicative arithmetic expression> -> 
*	<primary arithmetic expression><multiplicative arithmetic expression’>
*
* FIRST(<multiplicative arithmetic exp>) = { AVID_T, FPL_T, IL_T, ( }
*******************************************************************************/
void multiplicative_arithmetic_exp(void) {
	primary_arithmetic_exp(); multiplicative_arithmetic_exp_p();
}

/*******************************************************************************
* Purpose:			Parse Platypus multiplicative_arithmetic_exp_p syntax
* Author:			Skye Turriff
* History:			Version 1, 11 December 2015
* Called functions: match(), primary_arithmetic_exp(), 
*					multiplicative_arithmetic_exp_p(), gen_incode()
* Parameters:		None
* Return value:		None
*
* <multiplicative arithmetic expression’> ->
*	* <primary arithmetic expression><multiplicative arithmetic expression’>
*   | / <primary arithmetic expression><multiplicative arithmetic expression’>
*   | e
*
* FIRST(<multiplicative arithmetic exp>) = { * , / , e }
*******************************************************************************/
void multiplicative_arithmetic_exp_p(void) {

	if (lookahead.code == ART_OP_T && lookahead.attribute.get_int == MULT) {
		match(ART_OP_T, MULT); primary_arithmetic_exp();
		multiplicative_arithmetic_exp_p();
		gen_incode("PLATY: Multiplicative arithmetic expression parsed");
	}
	else if (lookahead.code == ART_OP_T && lookahead.attribute.get_int == DIV) {
		match(ART_OP_T, DIV); primary_arithmetic_exp();
		multiplicative_arithmetic_exp_p();
		gen_incode("PLATY: Multiplicative arithmetic expression parsed");
	}
}

/*******************************************************************************
* Purpose:			Parse Platypus primary_arithmetic_exp syntax
* Author:			Skye Turriff
* History:			Version 1, 11 December 2015
* Called functions: match(), arithmetic_expression(), syn_printe(), gen_incode()
* Parameters:		None
* Return value:		None
*
* <primary arithmetic expression> ->
*	<arithmetic variable identifier> | <floating-point literal>
*	| <integer literal> | (<arithmetic expression>)
*
* FIRST(<primary arithmetic exp>) = { AVID_T, FPL_T, IL_T, ( }
*******************************************************************************/
void primary_arithmetic_exp(void) {
	switch (lookahead.code) {
	case AVID_T:
		match(AVID_T, NO_ATTR); break;
	case FPL_T:
		match(FPL_T, NO_ATTR); break;
	case INL_T:
		match(INL_T, NO_ATTR); break;
	case LPR_T:
		match(LPR_T, NO_ATTR); arithmetic_expression(); match(RPR_T, NO_ATTR);
		break;
	default:
		syn_printe();
		return;
	}

	gen_incode("PLATY: Primary arithmetic expression parsed");
}

/*******************************************************************************
* Purpose:			Parse Platypus string_expression syntax
* Author:			Skye Turriff
* History:			Version 1, 11 December 2015
* Called functions: primary_string_expression(), string_expression_p(),
*					gen_incode()
* Parameters:		None
* Return value:		None
*
* <string expression> -> <primary string expression><string expression’>
*
* FIRST(<string expression>) = { SVID_T, STR_T }
*******************************************************************************/
void string_expression(void) {
	primary_string_expression(); string_expression_p();
	gen_incode("PLATY: String expression parsed");
}

/*******************************************************************************
* Purpose:			Parse Platypus string_expression_p syntax
* Author:			Skye Turriff
* History:			Version 1, 11 December 2015
* Called functions: match(), primary_string_expression(), string_expression_p()
* Parameters:		None
* Return value:		None
*
* <string expression’> -> 
*	# <primary string expression><string expression’> | e
*
* FIRST(<string expression’>) = { # , e }
*******************************************************************************/
void string_expression_p(void) {
	/* If token is #, continue to catenate string */
	if (lookahead.code == SCC_OP_T) {
		match(SCC_OP_T, NO_ATTR); primary_string_expression();
		string_expression_p();
	}
}

/*******************************************************************************
* Purpose:			Parse Platypus primary_string_expression syntax
* Author:			Skye Turriff
* History:			Version 1, 11 December 2015
* Called functions: match(), syn_printe(), gen_incode()
* Parameters:		None
* Return value:		None
*
* <primary string expression> ->
*	<string variable identifier> | <string literal>
*
* FIRST(<primary string expression>) = { SVID_T, STR_T } 
*******************************************************************************/
void primary_string_expression(void) {
	switch (lookahead.code) {
	case SVID_T:
		match(SVID_T, NO_ATTR); break;
	case STR_T:
		match(STR_T, NO_ATTR); break;
	default:
		syn_printe();
		return;
	}

	gen_incode("PLATY: Primary string expression parsed");
}

/*******************************************************************************
* Purpose:			Parse Platypus conditional_expression syntax
* Author:			Skye Turriff
* History:			Version 1, 11 December 2015
* Called functions: logical_or_expression(), gen_incode()
* Parameters:		None
* Return value:		None
*
* <conditional expression> -> <logical OR expression>
*
* FIRST(<conditional expression>) = { FPL_T, IL_T, AVID_T, STR_T, SVID_T }
*******************************************************************************/
void conditional_expression(void) {
	logical_or_expression(); 
	gen_incode("PLATY: Conditional expression parsed");
}

/*******************************************************************************
* Purpose:			Parse Platypus logical_or_expression syntax
* Author:			Skye Turriff
* History:			Version 1, 11 December 2015
* Called functions: logical_and_expression(), logical_or_expression_p()
* Parameters:		None
* Return value:		None
*
* <logical OR expression> -> <logical AND expression><logical OR expression’>
* 
* FIRST(<logical or exp>) = { FPL_T, IL_T, AVID_T, STR_T, SVID_T }
*******************************************************************************/
void logical_or_expression(void) {
	logical_and_expression(); logical_or_expression_p();
}

/*******************************************************************************
* Purpose:			Parse Platypus logical_or_expression_p syntax
* Author:			Skye Turriff
* History:			Version 1, 11 December 2015
* Called functions: match(), logical_and_expression(), logical_or_expression_p()
*					gen_incode()
* Parameters:		None
* Return value:		None
*
* <logical OR expression’> ->
*	.OR. <logical AND expression><logical OR expression’> | e
*
* FIRST(<logical or exp’>) = { LOG_OP_T(.OR.), e }
*******************************************************************************/
void logical_or_expression_p(void) {
	/* Parse .OR. expression (recursively) if there is one, else do nothing */
	if (lookahead.code == LOG_OP_T && lookahead.attribute.get_int == OR) {
		match(LOG_OP_T, OR); logical_and_expression();
		logical_or_expression_p();
		gen_incode("PLATY: Logical OR expression parsed");
	}
}

/*******************************************************************************
* Purpose:			Parse Platypus logical_and_expression syntax
* Author:			Skye Turriff
* History:			Version 1, 11 December 2015
* Called functions: relational_expression(), logical_and_expression_p()
* Parameters:		None
* Return value:		None
*
* <logical AND expression> -> <relational expression><logical AND expression’>
*
* FIRST(<logical and exp>) = { FPL_T, IL_T, AVID_T, STR_T, SVID_T }
*******************************************************************************/
void logical_and_expression(void) {
	relational_expression(); logical_and_expression_p();
}

/*******************************************************************************
* Purpose:			Parse Platypus logical_and_expression_p syntax
* Author:			Skye Turriff
* History:			Version 1, 11 December 2015
* Called functions: match(), relational_expression(), logical_and_expression_p()
*					gen_incode()
* Parameters:		None
* Return value:		None
*
* <logical AND expression’> -> 
*	.AND. <relational expression><logical AND expression’> | e
*
* FIRST(<logical and exp’>)	= { LOG_OP_T(.AND.), e }
*******************************************************************************/
void logical_and_expression_p(void) {
	/* Parse .AND. expression (recursively) if there is one, else do nothing */
	if (lookahead.code == LOG_OP_T && lookahead.attribute.get_int == AND) {
		match(LOG_OP_T, AND); relational_expression();
		logical_and_expression_p();
		gen_incode("PLATY: Logical AND expression parsed");
	}
}

/*******************************************************************************
* Purpose:			Parse Platypus relational_expression syntax
* Author:			Skye Turriff
* History:			Version 1, 11 December 2015
* Called functions: primary_a_rel_exp(), relational_operator(), 
*					primary_s_rel_exp(), syn_printe(), gen_incode()
* Parameters:		None
* Return value:		None
*
* <relational expression> -> 
*	<primary a_relational expression><reational operator>
*	<primary a_relational expression>
*	| <primary s_relational expression><reational operator>
*	<primary s_relational expression>
*
* FIRST(<relational expression>) = { FPL_T, IL_T, AVID_T, STR_T, SVID_T }
*******************************************************************************/
void relational_expression(void) {
	switch (lookahead.code) {
	case AVID_T: case FPL_T: case INL_T:
		primary_a_rel_exp(); relational_operator(); primary_a_rel_exp();
		break;
	case SVID_T: case STR_T:
		primary_s_rel_exp(); relational_operator(); primary_s_rel_exp();
		break;
	default:
		syn_printe();
		return;
	}

	gen_incode("PLATY: Relational expression parsed");
}

/*******************************************************************************
* Purpose:			Parse Platypus relational_operator syntax
* Author:			Skye Turriff
* History:			Version 1, 11 December 2015
* Called functions: match(), syn_printe()
* Parameters:		None
* Return value:		None
*
* <relational operator> -> == | <> |  > |  < 
*
* FIRST(<relational operator>) = { == , <> , < , > }
*******************************************************************************/
void relational_operator(void) {
	if (lookahead.code == REL_OP_T) {
		switch (lookahead.attribute.get_int) {
		case EQ:
			match(REL_OP_T, EQ); break;
		case NE:
			match(REL_OP_T, NE); break;
		case LT:
			match(REL_OP_T, LT); break;
		case GT:
			match(REL_OP_T, GT); break;
		default:
			syn_printe();
		}
	}
}

/*******************************************************************************
* Purpose:			Parse Platypus primary_a_rel_exp syntax
* Author:			Skye Turriff
* History:			Version 1, 11 December 2015
* Called functions: match(), syn_printe(), gen_incode()
* Parameters:		None
* Return value:		None
*
* <primary a_relational expression> ->
*	<floating-point literal> | <integer literal> | <arithmetic variable id>
*
* FIRST(<primary a_relational exp>)	= { FPL_T, IL_T, AVID_T }
*******************************************************************************/
void primary_a_rel_exp(void) {
	switch (lookahead.code) {
	case FPL_T:
		match(FPL_T, NO_ATTR); break;
	case INL_T:
		match(INL_T, NO_ATTR); break;
	case AVID_T:
		match(AVID_T, NO_ATTR); break;
	default:
		syn_printe(); return;
	}

	gen_incode("PLATY: Primary a_relational expression parsed");
}

/*******************************************************************************
* Purpose:			Parse Platypus primary_s_rel_exp syntax
* Author:			Skye Turriff
* History:			Version 1, 11 December 2015
* Called functions: match(), syn_printe(), gen_incode()
* Parameters:		None
* Return value:		None
*
* <primary s_relational expression> -> <string literal> | <string variable id>
*
* FIRST(<primary s_relational exp>)	= { STR_T, SVID_T }
*******************************************************************************/
void primary_s_rel_exp(void) {
	switch (lookahead.code) {
	case STR_T:
		match(STR_T, NO_ATTR); break;
	case SVID_T:
		match(SVID_T, NO_ATTR); break;
	default:
		syn_printe(); return;
	}

	gen_incode("PLATY: Primary string expression parsed");
	gen_incode("PLATY: Primary s_relational expression parsed");
}