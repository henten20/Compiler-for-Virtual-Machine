// ===================================
//	COP 3402 - SUMMER 2017
//	Author: Henry Ton
//	PARSER FOR VIRTUAL MACHINE
// ===================================

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "parser.h"

void parse(FILE *input)
{
	char buffer[TOKEN_MAX];
	int i;
 
  if(input == NULL)
  {
    printf("Error in opening the input file. \n");
    exit(1);
  }

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

	program();
}

void program()
{
	getNextToken();

	block();

	if (atoi(token) != periodsym)
		error(9);

	emit(SIO, 0, 3);

	printf("No errors, program is syntactically correct.\n\n");

	print_parser_output();
}

void block()
{
	int val, jump, idx, space = 4;
	char variable[12];
 
  level++;
  jump = cx;

	emit(JMP, 0, 0);

	// const decleration
	if (atoi(token) == constsym)
	{
		while (1)
		{
			getNextToken();

			if (atoi(token) != identsym)
				error(4);  // identifier expected

			getNextToken();
			strcpy(variable, token);
			getNextToken();

			if (atoi(token) == becomessym)
				error(1);	// use = instead

			if (atoi(token) != eqlsym)
				error(3);	// must be followed by =

			getNextToken();

			if (atoi(token) != numbersym)
				error(2);	// must be followed by a number

			getNextToken();
			val = atoi(token);

			if (val > 99999)
				error(25);	// number is too large

			add_to_symbol_table(1, variable, val, -1, -1);

			getNextToken();

			if (atoi(token) != commasym)
				break;
		}
		if (atoi(token) != semicolonsym)
			error(5);

		getNextToken();
	}

	// variable decleration
	if (atoi(token) == varsym)
	{
		while (1)
		{
      space++;
			getNextToken();

			if (atoi(token) != identsym)
				error(4);

			getNextToken();
	
			strcpy(variable, token);
			add_to_symbol_table(2, variable, 0, level, space);
			
			getNextToken();

			if (atoi(token) != commasym)
				break;
		}

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

			if (atoi(token) != identsym)
				error(4);	// identifier expected
      
			getNextToken(); 
		
			strcpy(variable, token);
   	  add_to_symbol_table(3, variable, -1, level, cx + 1);
			getNextToken();

			if (atoi(token) != semicolonsym)
				error(17);
      
			getNextToken();
      
			// recursive call to block
			block();

			if (atoi(token) != semicolonsym)
				error(10);

			getNextToken();
		}
	}

	code[jump].m = cx;
	emit(INC, 0, space);
	statement();
 
  if (level != 0)
    emit(OPR, 0, 0);
  
  // delete all symbols after exiting the level
  delete_symbols();
  level--;
}

void statement()
{
	int used = 0, kind, idx;

	// indent
	if (atoi(token) == identsym)
	{
		// variable name
		getNextToken();

		idx = search_symbol_table();
		kind = symbol_table[idx].kind;

		if (kind == 1 || kind == 3)
			error(12);	// assignment to procedure or constant is not allowed

		getNextToken();

		if (atoi(token) != becomessym)
			error(13); // assignment operatior expected

		getNextToken();
		expression();
		emit(STO, level - symbol_table[idx].level, symbol_table[idx].addr - 1);
	}

	// call
	else if (atoi(token) == callsym)
	{
		getNextToken();

		if (atoi(token) != identsym)
			error(14);	// call must be followed by an identifier

		getNextToken();

		idx = search_symbol_table();
    kind = symbol_table[idx].kind;

		if (symbol_table[idx].kind == 3)
			emit(CAL, level - symbol_table[idx].level, symbol_table[idx].addr);

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
		int ctemp = cx;
		emit(JPC, level, 0);
		statement();
		
    if (atoi(token) == elsesym)
    {
      code[ctemp].m = cx + 1;
      getNextToken();
      int ctemp2 = cx;
      emit(JMP, 0, 0);
      statement();
      code[ctemp2].m = cx;
    }
    else
      code[ctemp].m = cx;

	}
 
	// whilesym
	else if (atoi(token) == whilesym)
	{
		int cx1 = cx;
		getNextToken();
		condition();
		int cx2 = cx;
		emit(JPC, level, 0);

		if (atoi(token) != dosym)
			error(18);	// do expected

		getNextToken();
		statement();
		emit(JMP, level, cx1);
		code[cx2].m = cx;
	}

	// read ident
	else if (atoi(token) == readsym)
	{
		getNextToken();

		if (atoi(token) != identsym)
			error(28);

		getNextToken();

		idx = search_symbol_table();
		kind = symbol_table[idx].kind;

		// Store user input
		emit(SIO, level, 2);

		if (kind == 2)
			emit(STO, level - symbol_table[idx].level, symbol_table[idx].addr - 1);

		else if (kind == 1 || kind == 3)
			error(12); // assignment to procedure or constant not allowed

		getNextToken();
	}

	// write identifier
	else if (atoi(token) == writesym)
	{
		getNextToken();
    expression();
    emit(SIO, level, 1);
	}
}

void condition()
{
	// odd
	if (atoi(token) == oddsym)
	{
		getNextToken();
		expression();
		emit(OPR, level, 6);
	}
	else
	{
		expression();
		// tests if the token is a relation operator
		int op = relational_op(token);

		if (op == 0)
			error(20);	// relational operator expected

		getNextToken();
		expression();
		emit(OPR, level, op);
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
			emit(OPR, level, 0); // changed 12 to 2, maybe change L to 0
	}
	else
		term();

	while (atoi(token) == plussym || atoi(token) == minussym)
	{
		strcpy(op, token);
		getNextToken();
		term();

		if (strcmp(op, "4") == 0)
			emit(OPR, level, 2); // OPR ADD

		else
			emit(OPR, level, 3); // OPR Sub
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
			emit(OPR, level, 4); // OPR mul

		else
			emit(OPR, level, 5); // OPR div
	}
}

void factor()
{
	int idx = 0, kind;

	if (atoi(token) == identsym)
	{
		getNextToken();
		// check for variable names

		idx = search_symbol_table();
		kind = symbol_table[idx].kind;

		if (kind == 3)
			error(21);	// expression must not contain a procedure identifier

		if (kind == 2)
			emit(LOD, level - symbol_table[idx].level, symbol_table[idx].addr - 1);

		else if (kind == 1)
			emit(LIT, 0, symbol_table[idx].val);
      
		getNextToken();
	}
	// NUMERAL
	else if (atoi(token) == numbersym)
	{
		getNextToken();
    //int val = token;
    
		emit(LIT, 0, atoi(token));
		getNextToken();
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
		error(24);	// an expression cannot begin with this symbol

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
		error(30);	// Exceeded the maximum code length

	else
	{
		code[cx].op = op;
		code[cx].l = l;
		code[cx].m = m;
		cx++;
	}
}

void add_to_symbol_table(int kind, char *name, int val, int level, int addr)
{
	int used = 0, idx;

	// check for already declared identifiers
	for (int i = symbol_index - 1; i >= 0; i--)
	{
		if (!strcmp(name, symbol_table[i].name))
		{
      // exit when we search out of the current lexigraphical level
      if (symbol_table[i].level != level)
        break;
        
			used = 1;
			idx = i;
		}
	}
	if (used)
		error(29); // identifier already in use

	if (kind == 1)
	{
		symbol_table[symbol_index].kind = kind;
		strcpy(symbol_table[symbol_index].name, name);
		symbol_table[symbol_index].val = val;
		symbol_table[symbol_index].level = -1;
		symbol_table[symbol_index].addr = -1;
	}
	else if (kind == 2 || kind == 3)
	{
		symbol_table[symbol_index].kind = kind;
		strcpy(symbol_table[symbol_index].name, name);
		symbol_table[symbol_index].val = -1;
		symbol_table[symbol_index].level = level;
		symbol_table[symbol_index].addr = addr;
	}
 
	symbol_index++;
}

int search_symbol_table()
{
	int idx = 0, used = 0;

	// check for variable names
	for (int i = symbol_index - 1; i >= 0; i--)
	{
		if (!strcmp(token, symbol_table[i].name))
		{
			used = 1;
			idx = i;
		}
	}
	if (!used)
		error(11); // Undeclared identifier

	return idx;
}

void print_parser_output()
{
	FILE *output = fopen("vminput.txt", "w");
 
	for (int i = 0; i < cx; i++)
	{
		fprintf(output, "%d %d %d\n", code[i].op, code[i].l, code[i].m);
	}

	fclose(output);
}

void getNextToken()
{
	if (tokens[token_index] == NULL)
		error(0);

	token = tokens[token_index++];
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
		printf("Const, var, procedure must be followed by identifier.\n"); break;
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
	case 27:
		printf("End expected.\n"); break;
	case 28:
		printf("Identifier expected. \n"); break;
	case 29:
		printf("identifier already in use. \n"); break;
	case 30:
		printf("Exceeded the maximum code size. \n"); break;
	case 0:
		printf("ERROR\n"); break;
	}
	exit(1);
}

void print_symbol_table()
{
  printf("\nKind\tName\tValue\tLevel\tAddr\t\n");
  
  for(int i = 0; i < symbol_index; i++)
    printf("%d\t%s\t%d\t%d\t%d\t\n", symbol_table[i].kind, symbol_table[i].name, symbol_table[i].val, symbol_table[i].level, symbol_table[i].addr);
}

void delete_symbols()
{
  for (int i = symbol_index - 1; i >= 0; i--)
	{
    if (level == 0)
      break;
      
		if (symbol_table[i].level == level)
		{
     // only delete variable symbols
      if (symbol_table[i].kind != 2)
        break;
        
      strcpy(symbol_table[i].name, "[Del]");
      symbol_table[i].kind = -1;
      symbol_table[i].val = -1;
      symbol_table[i].level = -1;
      symbol_table[i].addr = -1;
      
      symbol_index--;
		}
	}
}

// ===============================================================================
// Notes on changes:
//
//  - Deleted check_identifier functionality since lexicalAnalyzer does it already
//  - Deleted case 26
//
//  - Added option to print symbol table for debugging purposes
//  - Took out the initialization of the code array
//  - Deleted proc_idx and respaced variables, brackets