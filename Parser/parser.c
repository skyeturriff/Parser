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
#include <stdlib.h>
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
	program(); match(SEOF_T, NO_ATTR);
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
	match(KW_T, PLATYPUS); match(LBR_T, NO_ATTR); opt_statements();
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
	case AVID_T: case SVID_T:
		statements(); break;
	case KW_T:
		if (lookahead.attribute.get_int == IF
				|| lookahead.attribute.get_int == USING
				|| lookahead.attribute.get_int == INPUT
				|| lookahead.attribute.get_int == OUTPUT) {
			statements(); break;
		}
	default:
		gen_incode("PLATY: Opt_statements parsed");
	}
}

/*******************************************************************************
* Purpose:			Parse Platypus statements syntax
* Author:			Skye Turriff
* History:			Version 1, 10 December 2015
* Called functions: statement(), statements_p(), gen_incode()
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
	gen_incode("PLATY: Statements parsed");
}

/*******************************************************************************
* Purpose:			Parse Platypus statements_p syntax
* Author:			Skye Turriff
* History:			Version 1, 10 December 2015
* Called functions: statement(), statements_p(), gen_incode()
* Parameters:		None
* Return value:		None
*
* <statements'> -> <statement><statements'> | e
*
* FIRST(<statements'>) = { AVID_T, SVID_T, KW_T(IF), KW_T(USING),
*	KW_T(INPUT), KW_T(OUTPUT), e}
*******************************************************************************/
void statements_p(void) {
	if (lookahead.code == AVID_T
			|| lookahead.code == SVID_T
			|| lookahead.attribute.get_int == IF
			|| lookahead.attribute.get_int == USING
			|| lookahead.attribute.get_int == INPUT
			|| lookahead.attribute.get_int == OUTPUT) {
		statement(); statements_p();
	}
	else gen_incode("PLATY: Statements_p parsed");
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
* <statement> -> <assignment statement> | <selection statement>
*	| <iteration statement> | <input statement> | <output statement>
*
* FIRST(<statement>) = { AVID_T, SVID_T, KW_T(IF), KW_T(USING), KW_T(INPUT), 
*	KW_T(OUTPUT) }
*******************************************************************************/
void statement(void) {
	switch (lookahead.code) {
	case AVID_T: case SVID_T:
		assignment_statement(); break;
	case KW_T:
		switch (lookahead.attribute.get_int) {
		case IF: selection_statement(); break;
		case INPUT: input_statement(); break;
		case OUTPUT: output_statement(); break;
		case USING: iteration_statement(); break;
		}
	default:
		syn_printe();
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
* Called functions: arithmetic_expression(), string_expression(), syn_printe()
* Parameters:		None
* Return value:		None
*
* <assignment expression> -> AVID = <arithmetic expression> 
*	| SVID = <string expression>
*
* FIRST(<assignment expression>) = { AVID_T, SVID_T }
*******************************************************************************/
void assignment_expression(void) {
	switch (lookahead.code) {
	case AVID_T:
		arithmetic_expression(); break;
	case SVID_T:
		string_expression; break;
	default:
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
* <selection statement> -> IF (<conditional expression>)  
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
	gen_incode("PLATY: Selection statement parsed");
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
* <iteration statement> -> USING (<assignment expression> 
*	<conditional expression> <assignment expression>)
*	REPEAT { <opt_statements> };
*
* FIRST(<iteration statement>) = { KW_T(USING) }
*******************************************************************************/
void iteration_statement(void) {
	match(KW_T, USING); match(LPR_T, NO_ATTR); assignment_expression();
	conditional_expression(); assignment_expression(); match(RPR_T, NO_ATTR);
	match(KW_T, REPEAT); match(LBR_T, NO_ATTR); opt_statements(); 
	match(RBR_T, NO_ATTR); match(EOS_T, NO_ATTR);
	gen_incode("PLATY: Iteration statement parsed");
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
	gen_incode("PLATY: Input statement parsed");
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
* Called functions: match(), variable_identifier(), variable_list_p(),
*					gen_incode()
* Parameters:		None
* Return value:		None
*
* <variable list’> -> , <variable identifier><variable list’> | e
* 
* FIRST(<variable list’>) = { , e }
*******************************************************************************/
void variable_list_p(void) {
	if (lookahead.code == COM_T) {
		match(COM_T, NO_ATTR); variable_identifier(); variable_list_p();
	} 
	else gen_incode("PLATY: Variable_list_p parsed");
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
	match(RBR_T, NO_ATTR); match(EOS_T, NO_ATTR);
	gen_incode("PLATY: Output statement parsed");
}

/*******************************************************************************
* Purpose:			Parse Platypus output_statement_p syntax
* Author:			Skye Turriff
* History:			Version 1, 10 December 2015
* Called functions: variable_list(), string_expression(), gen_incode()
* Parameters:		None
* Return value:		None
*
* <output statement’> -> <variable list> | STR_T | e
*
* FIRST(<output statement’>) = { AVID_T, SVID_T, STR_T, e }
*******************************************************************************/
void output_statement_p(void) {
	switch (lookahead.code) {
	case AVID_T: case SVID_T:
		variable_list();  break;
	case STR_T:
		string_expression(); break;
	default:
		gen_incode("PLATY: Output_statement_p parsed");
	}
}

/*******************************************************************************
* Purpose:			Parse Platypus arithmetic_expression syntax
* Author:			Skye Turriff
* History:			Version 1, 10 December 2015
* Called functions: 
* Parameters:		None
* Return value:		None
*
* <arithmetic expression> -> <unary arithmetic expression> 
*	| <additive arithmetic expression>
*
* FIRST(<arithmetic expression>) = { - , + , AVID_T, FPL_T, IL_T, ( }
*******************************************************************************/
void arithmetic_expression(void) {
	switch (lookahead.code) {
	case ART_OP_T:
		if (lookahead.attribute.get_int < MULT) {
			unary_arithmetic_expression(); break;
		}
	case AVID_T: case FPL_T: case INL_T: case LPR_T:
		additive_arithmetic_expression(); break;
	default:

	}
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