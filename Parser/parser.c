/*******************************************************************************
* File Name:		parser.c
* Compiler:			MS Visual Studio 2013
* Author:			Skye Turriff
* Course:			CST 8152 - Compilers, Lab Section:	011
* Assignment:		4
* Date:				7 December 2015
* Professor:		Sv. Ranev
* Purpose:			Function definition for protypes declared in parser.h
* Function list:	
*******************************************************************************/
#include "buffer.h"
#include "token.h"
#include "stable.h"
#include "parser.h"

/*******************************************************************************
* Purpose:			
* Author:			Skye Turriff
* History:			Version 1, 7 December 2015
* Called functions:
* Parameters:		
* Return value:		
* Algorithm:		
*******************************************************************************/
void parser(Buffer* in_buf) {
	sc_buf = in_buf;
	lookahead = mlwpar_next_token(sc_buf);
	match(ART_OP_T, PLUS);
	match(ART_OP_T, MULT);
	match(KW_T, INPUT);
	match(SEOF_T, NO_ATTR);
	//program(); match(SEOF_T, NO_ATTR);
	gen_incode("PLATY: Source file parsed");
}

/*******************************************************************************
* Purpose:
* Author:			Skye Turriff
* History:			Version 1, 7 December 2015
* Called functions:
* Parameters:
* Return value:
* Algorithm:
*******************************************************************************/
void match(int pr_token_code, int pr_token_attribute) {
	printf("lookahead token:\t%d\tattribute: %d\n", lookahead.code, 
		lookahead.attribute);
	printf("required token code:\t%d\tattribute: %d\n", pr_token_code,
		pr_token_attribute);
	
	/* Call error handler if no match */
	if (lookahead.code != pr_token_code) {
		syn_eh(pr_token_code); return;
	}

	/* Match to SEOF_T, return */
	if (pr_token_code == SEOF_T)
		return;

	/* Match those tokens with attribute codes */
	switch (pr_token_code) {
	case  ART_OP_T:
		if (lookahead.attribute.arr_op != pr_token_attribute) {
			syn_eh(pr_token_code); return;
		}
		break;
	case  REL_OP_T:
		if (lookahead.attribute.rel_op != pr_token_attribute) {
			syn_eh(pr_token_code); return;
		}
		break;
	case  LOG_OP_T:
		if (lookahead.attribute.log_op != pr_token_attribute) {
			syn_eh(pr_token_code); return;
		}
		break;
	case KW_T:
		if (lookahead.attribute.kwt_idx != pr_token_attribute) {
			syn_eh(pr_token_code); return;
		}
		break;
	}

	lookahead = mlwpar_next_token(sc_buf);

	while (lookahead.code == ERR_T) {
		syn_printe();
		lookahead = mlwpar_next_token(sc_buf);
		++synerrno;
	}
}

/*******************************************************************************
* Purpose:
* Author:			Skye Turriff
* History:			Version 1, 7 December 2015
* Called functions:
* Parameters:
* Return value:
* Algorithm:
*******************************************************************************/
void syn_eh(int sync_token_code) {
	syn_printe();
	++scerrnum;

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
* Purpose:
* Author:			Skye Turriff
* History:			Version 1, 7 December 2015
* Called functions:
* Parameters:
* Return value:
* Algorithm:
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
* Purpose:
* Author:			Skye Turriff
* History:			Version 1, 7 December 2015
* Called functions:
* Parameters:
* Return value:
* Algorithm:
*******************************************************************************/
void gen_incode(char* message) {
	printf("%s\n", message);
}

/*******************************************************************************
*
*	PRODUCTIONS
*
*******************************************************************************/
void program(void) {
	match(KW_T, PLATYPUS); match(LBR_T, NO_ATTR); opt_statement();
	match(RBR_T, NO_ATTR);
	gen_incode("PLATY: Program parsed");
}

void opt_statement(void) {

}

void statements(void) {

}

void statements_p(void) {

}

void statement(void) {

}

void assignment_statement(void) {

}

void assignment_expression(void) {

}

void selection_statement(void) {

}

void iteration_statement(void) {

}

void input_statement(void) {

}

void variable_list(void) {

}

void variable_list_p(void) {

}

void variable_identifier(void) {

}

void output_statement(void) {

}

void output_statement_p(void) {

}

void arithmetic_expression(void) {

}

void unary_arithmetic_expression(void) {

}

void additive_arithmetic_expression(void) {

}

void additive_arithmetic_expression_p(void) {

}

void multiplicative_arithmetic_expression(void) {

}

void multiplicative_arithmetic_expression_p(void) {

}

void primary_arithmetic_expression(void) {

}

void string_expression(void) {

}

void string_expression_p(void) {

}

void primary_string_expression(void) {

}

void conditional_expression(void) {

}

void logical_or_expression(void) {

}

void logical_or_expression_p(void) {

}

void logical_and_expression(void) {

}

void logical_and_expression_p(void) {

}

void relation_expression(void) {

}

void relation_operator(void) {

}

void primary_a_relational_expression(void) {

}

void primary_s_relational_expression(void) {

}