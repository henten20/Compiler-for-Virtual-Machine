#ifndef parser
#define parser

// Parser 
#define MAX_SYMBOL_TABLE_SIZE 100
#define MAX_CODE_SIZE 500
#define TOKEN_MAX 256  

typedef struct Symbol
{

	int kind;		// const = 1, var = 2, proc = 3
	char name[10];	// name up to 11 chars
	int val;		// number (ASCII value) 
	int level;		// L level
	int addr;		// M address

} Symbol;

typedef struct Code
{
	int op; // opcode
	int r;  // register
	int l;  // lexicographical level
	int m;  // offset

} Code;

//Internal representation stuff
typedef enum
{
	nulsym = 1, identsym = 2, numbersym = 3, plussym = 4,
	minussym = 5, multsym = 6, slashsym = 7, oddsym = 8, eqlsym = 9,
	neqsym = 10, lessym = 11, leqsym = 12, gtrsym = 13, geqsym = 14,
	lparentsym = 15, rparentsym = 16, commasym = 17, semicolonsym = 18,
	periodsym = 19, becomessym = 20, beginsym = 21, endsym = 22, ifsym = 23,
	thensym = 24, whilesym = 25, dosym = 26, callsym = 27, constsym = 28,
	varsym = 29, procsym = 30, writesym = 31, readsym = 32, elseym = 33

} sym;

typedef enum
{
	LIT = 1, OPR = 2, LOD = 3, STO = 4, CAL = 5, INC = 6, JMP = 7, JPC = 8, SIO = 9

} opr;


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
int len = 0;

// list of symbols input from scanner
char lexemes[MAX_CODE_SIZE][TOKEN_MAX];
char tokens[MAX_CODE_SIZE][TOKEN_MAX];
char *token;

// function prototypes

void program();
void block();
void statement();
void condition();
void expression();
void term();
void factor();
void getNextToken();
void check_identifier(char *);
int relational_op(char *);
void emit(int, int, int);
void add_to_symbol_table(int, char*, int, int, int);
int search_symbol_table();
void print_parser_output();
void error(int);

#endif