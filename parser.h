#ifndef parser
#define parser


// Parser 
#define MAX_SYMBOL_TABLE_SIZE 10000
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


//Internal representation mapping, from integer to string.
char IRMapping[34][64] = {
	"ZERO",
	"nulsym",
	"identsym",
	"numbersym",
	"plussym",
	"minussym",
	"multsym",
	"slashsym",
	"oddsym",
	"eqlsym",
	"neqsym",
	"lessym",
	"leqsym",
	"gtrsym",
	"geqsym",
	"lparentsym",
	"rparentsym",
	"commasym",
	"semicolonsym",
	"periodsym",
	"becomesym",
	"beginsym",
	"endsym",
	"ifsym",
	"thensym",
	"whilesym",
	"dosym",
	"callsym",
	"constsym",
	"varsym",
	"?",
	"writesym",
	"readsym",
	"?",

};

//List of symbols allowed
char symbols[] = { '+', '-', '*', '/', '(', ')', '=', ',', '.', '<', '>', ';', ':' };

char reserved[14][32] = {
	"const",
	"var",
	"?",
	"call",
	"begin",
	"end",
	"if",
	"then",
	"?",
	"while",
	"do",
	"read",
	"write",
	"odd"
};

// function prototypes
void emit(int, int, int);
int relational_op(char *);
void error(int);
void check_identifier(char *);


void program();
void block();
void statement();
void condition();
void expression();
void term();
void factor();
void emit(int, int, int);
void print_parser_output();
void add_symbol_table(char*, int, int, int);
void add_to_symbol_table(int, char*, int, int, int);
void getNextToken();

#endif