// ===================================
//	COP 3402 - SUMMER 2017
//	Author: Henry Ton
//	PARSER FOR VIRTUAL MACHINE
// ===================================

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "parser.h"

// symbol table
Symbol symbol_table[MAX_SYMBOL_TABLE_SIZE];

// registers 
Code code[MAX_CODE_SIZE];
int token_index = 0;
int cx = 0;
int sp = 1;
int symbol_index = 0;
int lexographical_level = 1;
int level = 0;
int reg_ptr = 0;


// list of symbols input from scanner
char lexemes[MAX_CODE_SIZE][TOKEN_MAX];
char tokens[MAX_CODE_SIZE][TOKEN_MAX];
int len = 0;
char *token;


void parse(FILE *input)
{
	char buffer[TOKEN_MAX];
	int i;

	// moves to the first part of the lexeme list
	while (fscanf(input, "%s ", buffer) != EOF)
	{
		if (strcmp(buffer, "List:") == 0)
			break;
	}
	// save into lexeme list
	for (i = 0; fscanf(input, "%s ", buffer) != EOF; i++)
	{
		if (strcmp(buffer, "Lexeme") == 0)
			break;
		strcpy(lexemes[i], buffer);
		len++;
	}
	// moves to the second part of the lexeme list
	while (fscanf(input, "%s ", buffer) != EOF)
	{
		if (strcmp(buffer, "List:") == 0)
			break;
	}
	// save into token list
	for (i = 0; fscanf(input, "%s ", buffer) != EOF; i++)
	{
		strcpy(tokens[i], buffer);
	}
	// close the input
	fclose(input);

	// initialize the code array
	for (int i = 0; i < MAX_CODE_SIZE; i++)
	{
		code[i].op = 0;
		code[i].r = 0;
		code[i].l = 0;
		code[i].m = 0;
	}

	// PROGRAM PROCEDURE
	program();
	//print_lexemes();

}

void program()
{
	getNextToken();

	block();

	if (atoi(token) != periodsym)
		error(9);

	emit(9, level, 3);

	printf("No errors, program is syntactically correct.\n");

	// prints the generated assembly code to file
	print_parser_output();

	return;
}

void block()
{
	int jump = cx, val;
	int space = 0;
	int procedure_index;
	char variable[12];

	emit(7, level, 0);

	// const decleration

	if (atoi(token) == constsym)
	{
		while (1)
		{
			// ident
			getNextToken();
			if (atoi(token) != identsym)
				error(4);
			// variable
			getNextToken();
			// check to see if the identifier is gramatically correct
			check_identifier(token);
			// save variable
			strcpy(variable, token);
			// eqlsym
			getNextToken();
			if (atoi(token) == becomessym)
				error(1);	// use = instead
			if (atoi(token) != eqlsym)
				error(3);	// must be followed by =
							// number literal
			getNextToken();
			if (atoi(token) != numbersym)
				error(2);	// must be followed by a number
							// integer value
			getNextToken();
			// save integer
			val = atoi(token); // might need &
							   // add to symbol table now
							   //add_symbol_table(variable, 1, val, 0);
			add_to_symbol_table(1, variable, val, -1, -1);
			getNextToken();
			space++;
			// comma
			if (atoi(token) != commasym)
				break;
		}
		// semicolon
		if (atoi(token) != semicolonsym)
			error(5);
		getNextToken();
	}
	// variable decleration
	if (atoi(token) == varsym)
	{
		while (1)
		{
			// ident
			getNextToken();
			if (atoi(token) != identsym)
				error(4);
			// variable name
			getNextToken();
			// check to see if the identifier is gramatically correct
			check_identifier(token);
			// save variable name
			strcpy(variable, token);
			// save to symbolt able
			//add_symbol_table(variable, 2, 0, sp);
			add_to_symbol_table(2, variable, 0, level, sp);
			//symbol_table[symbol_index - 1].level = level;		// redundant?
			space++;
			sp++;
			getNextToken();

			// comma
			if (atoi(token) != commasym)
				break;
		}
		// semicolon
		if (atoi(token) != semicolonsym)
			error(7);	// semicolon expected
		getNextToken();
	}
	// procedure
	if (atoi(token) == procsym)
	{
		while (atoi(token) == procsym)
		{

			getNextToken();
			// ident
			if (atoi(token) != identsym)
				error(4);
			// identifier for procedure
			getNextToken();
			// save variable
			strcpy(variable, token);
			// add to symbol table
			add_symbol_table(variable, 3, 0, 0);
			procedure_index = symbol_index - 1;
			symbol_table[procedure_index].level = level;
			symbol_table[procedure_index].addr = jump + 1;


			getNextToken();
			// semicolon
			if (atoi(token) != semicolonsym)
				error(17);

			getNextToken();
			space++;
			level++;

			// recursive call 
			block();
			// At this point, the token should be a ";" right when the proc ends
			if (atoi(token) != semicolonsym)
				error(10);
			getNextToken();
		}
		level--;
	}

	code[jump].m = cx;
	emit(6, level, space);
	statement();

}

void statement()
{
	int used = 0;
	int identifier_index;

	// indent
	if (atoi(token) == identsym)
	{
		// variable name
		getNextToken();
		// check for already used variable names
		for (int i = symbol_index - 1; i >= 0; i--)
		{
			if (!strcmp(token, symbol_table[i].name))
			{
				if (symbol_table[i].kind == 1)
					error(12);
				else if (symbol_table[i].kind == 2)
				{
					used = 1;
					identifier_index = i;
				}
			}
		}
		if (!used)
			error(11); // undeclared identifier

		getNextToken();

		if (atoi(token) != becomessym)
			error(13); // assignment operatior expected
		getNextToken();
		expression();

		// emit
		emit(4, level, symbol_table[identifier_index].addr - 1);

	}
	// call
	else if (atoi(token) == callsym)
	{
		int used = 0;

		getNextToken();

		if (atoi(token) != identsym)
			error(14);	// call must be followed by an identifier

		getNextToken();
		// check for already used variable names
		for (int i = symbol_index - 1; i >= 0; i--)
		{
			if (!strcmp(token, symbol_table[i].name))
			{
				if (symbol_table[i].kind == 1 || symbol_table[i].kind == 2)
					error(15); // call of a constant or variable is meaningless
				identifier_index = i;
				used = 1;
			}
		}
		if (!used)
			error(11);

		if (symbol_table[identifier_index].kind == 3)
		{
			emit(5, level, symbol_table[identifier_index].addr);
			//lexographical_level++;
			//level++;
		}
		else
			error(14); // expected procedure after call

		getNextToken();
	}
	// begin
	else if (atoi(token) == beginsym)
	{
		getNextToken();
		statement();

		while (atoi(token) == semicolonsym)
		{
			getNextToken();
			statement();

		}
		if (atoi(token) != endsym)
			error(27);	// end expected  

		getNextToken();
	}
	// if
	else if (atoi(token) == ifsym)
	{
		getNextToken();
		condition();
		if (atoi(token) != thensym)
			error(16); // then expected
		getNextToken();

		// begin code gen
		int ctemp = cx;
		emit(8, level, 0);

		statement();
		code[ctemp].m = cx;
	}
	// whilesym
	else if (atoi(token) == whilesym)
	{
		int cx1 = cx;
		getNextToken();
		condition();
		int cx2 = cx;
		// gen goes here
		emit(8, level, 0);
		if (atoi(token) != dosym)
			error(18);	// do expected
		getNextToken();
		statement();
		emit(7, level, cx1);
		code[cx2].m = cx;

	}
	// read ident
	else if (atoi(token) == readsym)
	{
		getNextToken();
		if (atoi(token) != identsym)
			error(29);
		getNextToken();
		// check for already used variable names
		for (int i = symbol_index - 1; i >= 0; i--)
		{
			if (!strcmp(token, symbol_table[i].name))
			{
				identifier_index = i;
				used = 1;
			}
		}
		if (!used)
			error(11);

		// take in user input
		emit(9, level, 2);

		if (symbol_table[identifier_index].kind == 2)
			emit(4, level, symbol_table[identifier_index].addr - 1);

		else if (symbol_table[identifier_index].kind == 1)
			error(12); // cannot change constants

		getNextToken();
	}

	// write identifier
	else if (atoi(token) == writesym)
	{
		getNextToken();

		if (atoi(token) != identsym)
			error(29);
		getNextToken();
		// check if variable exists in symbol table
		for (int i = symbol_index - 1; i >= 0; i--)
		{
			if (!strcmp(token, symbol_table[i].name))
			{
				if (symbol_table[i].kind == 1 || symbol_table[i].kind == 2)
				{
					identifier_index = i;
					used = 1;
				}
			}
		}
		if (!used)
			error(11);

		// retrieve var
		if (symbol_table[identifier_index].kind == 2)
		{
			emit(3, level - symbol_table[identifier_index].level, symbol_table[identifier_index].addr - 1);
			emit(9, level, 1);
		}
		// retrieve variable
		else if (symbol_table[identifier_index].kind == 1)
		{
			emit(1, level, symbol_table[identifier_index].val);
			emit(9, level, 1);
		}

		getNextToken();
	}
}

void condition()
{
	// odd
	if (atoi(token) == oddsym)
	{
		getNextToken();
		expression();
		emit(2, level, 6);
	}
	else
	{
		expression();
		// tests if the token is a relation operator
		int op = relational_op(token);
		if (op == 0)
			error(20);
		getNextToken();
		expression();

		emit(2, level, op);
		reg_ptr--;
	}
}

void expression()
{
	char op[3];

	if (atoi(token) == plussym || atoi(token) == minussym)
	{
		strcpy(op, token);
		getNextToken();
		term();

		if (strcmp(op, "5") == 0)
			emit(2, level, 0); // changed 12 to 2, maybe change L to 0
	}
	else
		term();

	while (atoi(token) == plussym || atoi(token) == minussym)
	{
		strcpy(op, token);
		// indicates number
		getNextToken();
		term();

		if (strcmp(op, "4") == 0)
		{
			emit(2, level, 2); // OPR ADD

		}
		else
		{
			emit(2, level, 3);

		}
	}
}

void term()
{
	char op[3];

	factor();

	while (atoi(token) == multsym || atoi(token) == slashsym)
	{
		strcpy(op, token);
		getNextToken();
		factor();

		if (strcmp(op, "6") == 0)
		{
			emit(2, level, 4); // OPR mul

		}
		else
		{
			emit(2, level, 5); // OPR div

		}
	}
}

void factor()
{
	int used = 0, identifier_index = 0;

	if (atoi(token) == identsym)
	{
		getNextToken();
		// check for already used variable names
		for (int i = symbol_index - 1; i >= 0; i--)
		{
			if (!strcmp(token, symbol_table[i].name))
			{
				// error if identifier is a procedure identifier
				if (symbol_table[i].kind == 3)
					error(21);

				identifier_index = i;
				used = 1;
			}
		}
		if (!used)
			error(11); // undeclared identifier

		if (symbol_table[identifier_index].kind == 2)
			emit(3, level, symbol_table[identifier_index].addr - 1);

		else if (symbol_table[identifier_index].kind == 1)
			emit(1, level, symbol_table[identifier_index].val);


		// Variable
		getNextToken();
		// Save variable
		//token = tokens[index++];
	}
	// NUMERAL
	else if (atoi(token) == numbersym)
	{
		getNextToken();
		emit(1, level, atoi(token));


		// numeral assigned i.e. x = 56
		getNextToken();
		// variable assigned now. Move to next token
		//token = tokens[index++];
	}
	else if (atoi(token) == lparentsym)
	{
		getNextToken();
		expression();
		if (atoi(token) != rparentsym)
			error(22);	// right parenthesis missing
		getNextToken();
	}
	else
		error(7);
	return;
}

int relational_op(char *token)
{
	if (strcmp(token, "9") == 0) // eql
		return 8;
	else if (strcmp(token, "10") == 0) // neq
		return 9;
	else if (strcmp(token, "11") == 0) // les
		return 10;
	else if (strcmp(token, "12") == 0) // leq
		return 11;
	else if (strcmp(token, "13") == 0) // gtr
		return 12;
	else if (strcmp(token, "14") == 0) // geq
		return 13;
	else
		return 0;
}

void emit(int op, int l, int m)
{
	if (cx > MAX_CODE_SIZE)
		error(25);	// 

	else
	{
		code[cx].op = op;
		code[cx].l = l;
		code[cx].m = m;
		cx++;
	}
}

void add_symbol_table(char* name, int kind, int val, int address)
{
	symbol_table[symbol_index].kind = kind;
	symbol_table[symbol_index].val = val;
	symbol_table[symbol_index].addr = address;
	strcpy(symbol_table[symbol_index].name, name);

	symbol_index++;
}

void add_to_symbol_table(int kind, char *name, int val, int level, int addr)
{
	if (kind == 1)
	{
		symbol_table[symbol_index].kind = kind;
		strcpy(symbol_table[symbol_index].name, name);
		symbol_table[symbol_index].val = val;

		// not used
		symbol_table[symbol_index].level = -1;
		symbol_table[symbol_index].addr = -1;
	}
	else if (kind == 2)
	{
		symbol_table[symbol_index].kind = kind;
		strcpy(symbol_table[symbol_index].name, name);
		symbol_table[symbol_index].level = level;
		symbol_table[symbol_index].addr = addr;

		// not used
		symbol_table[symbol_index].val = val;
	}

	symbol_index++;
}

void error(int i)
{
	switch (i)
	{
	case 1:
		printf("Use = instead of :=.\n"); break;
	case 2:
		printf("= must be followed by a number.\n"); break;
	case 3:
		printf("Identifier must be followed by =.\n"); break;
	case 4:
		printf("Const, var, proceduremust be followed by identifier.\n"); break;
	case 5:
		printf("Semicolon or comma missing.\n"); break;
	case 6:
		printf("Incorrect symbol after procedure declaration.\n"); break;
	case 7:
		printf("Statement expected.\n"); break;
	case 8:
		printf("Incorrect symbol after statement part in block.\n"); break;
	case 9:
		printf("Period expected.\n"); break;
	case 10:
		printf("Semicolon between statements missing.\n"); break;
	case 11:
		printf("Undeclared identifier.\n"); break;
	case 12:
		printf("Assignment to constant or procedure is not allowed.\n"); break;
	case 13:
		printf("Assignment operator expected.\n"); break;
	case 14:
		printf("Call must be followed by an identifier.\n"); break;
	case 15:
		printf("Call of a constant or variable is meaningless.\n"); break;
	case 16:
		printf("Then expected.\n"); break;
	case 17:
		printf("Semicolon or end expected.\n"); break;
	case 18:
		printf("Do expected.\n"); break;
	case 19:
		printf("Incorrect symbol following statement.\n"); break;
	case 20:
		printf("Relational operator expected.\n"); break;
	case 21:
		printf("Expression must not contain a procedure identifier.\n"); break;
	case 22:
		printf("Right parenthesis missing.\n"); break;
	case 23:
		printf("The preceding factor cannot begin with this symbol.\n"); break;
	case 24:
		printf("An expression cannot begin with this symbol.\n"); break;
	case 25:
		printf("This number is too large.\n"); break;
	case 26:
		printf("Incorrect format for identifier.\n");
	case 27:
		printf("End expected.\n");
	case 0:
		printf("ERROR\n"); break;
	}
	exit(1);
}

void check_identifier(char *token)
{
	int i = 0;
	int length = strlen(token);

	if (token == NULL)
		return;

	if (token[i] < 65 || token[i] > 122)
		error(26);
	i++;

	while (i < length)
	{
		if (token[i] >= 65 || token[i] <= 122 || token[i] >= 30 || token[i] <= 39)
			;
		else
			error(26);
		i++;
	}
}

void print_lexemes()
{
	printf("Symbolic Lexeme List:\n");
	for (int i = 0; i < len; i++)
		printf("%s ", lexemes[i]);
	printf("\n\n");

	printf("Lexeme List:\n");
	for (int i = 0; i < len; i++)
		printf("%s ", tokens[i]);
	printf("\n\n");
}

void print_assembly_code()
{
	for (int i = 0; i < cx; i++)
	{
		printf("%d %d %d\n", code[i].op, code[i].l, code[i].m);
	}
	printf("\n\n");
}

void print_parser_output()
{
	FILE *output = fopen("vminput.txt", "w");

	for (int i = 0; i < cx; i++)
	{
		fprintf(output, "%d %d %d\n", code[i].op, code[i].l, code[i].m);
	}
	printf("\n\n");

	fclose(output);
}

void getNextToken()
{
	if (tokens[token_index] == NULL)
		error(0);

	token = tokens[token_index++];
}