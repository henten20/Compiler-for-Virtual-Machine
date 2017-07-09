#ifndef PL_0
#define PL_0

// VM
#define MAX_STACK_HEIGHT 2000
#define MAX_CODE_LENGTH 500 
#define MAX_AR_SIZE 500

//	Variable parameters for the instruction register architecture
typedef struct Instruction
{
	// opcode
	int op;
	// lexicographical 
	int l;
	// m offset 
	int m;
	// String representation for opcode
	char op_str[4];
	// used to read in each instruction 
	int inst_length;

}Instruction;

//	Variable parameters for the registers containing pc, sp, and bp, AR represents
//	the number of activation records and will be a pointer for dynamic links
typedef struct Register
{
	// activation record index to keep track of local base pointers
	int index;
	// register for program counter
	int pc;
	// register for the stack pointer
	int sp;
	// register for base pointer
	int bp;
	// stack with max size of 2000
	int stack[MAX_STACK_HEIGHT];
	// An array of base pointers used to print "|"
	int AR[MAX_AR_SIZE];
	// Indicates whether the instruction was a cal instruction
	int cal;
	int inc;

} Register;

//	Function Prototypes
int base(Instruction*, Register*, int);
void print_instruction(Instruction*, FILE*);
void print_initial_values(FILE*, Instruction*, int);
void print_stack(Register*, FILE*);
void parse_string(Instruction*);
void perform_operation(Instruction*, Register*);
void instruction_execute(Instruction*, FILE*);

#endif


