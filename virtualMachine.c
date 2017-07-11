// ===================================
//	COP 3402 - SUMMER 2017
//	Author: Henry Ton
//	PM-0 VIrtual-MACHINE
// ===================================

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "virtualMachine.h"

void VM()
{
	FILE *input = fopen("vminput.txt", "r");
	FILE *output = fopen("vmoutput.txt", "w");

	Instruction *IR = malloc(sizeof(Instruction) * MAX_CODE_LENGTH);
	IR->inst_length = 0;

	if (!input)
	{
		printf("Error in opening file\n");
		exit(0);
	}

	// parse into inst reg: op, l, and m fields until EOF 
	while (fscanf(input, "%d %d %d", &IR[IR->inst_length].op, &IR[IR->inst_length].l, &IR[IR->inst_length].m) != EOF)
		IR->inst_length++;

	// fetch the string literals for op and store into the struct field
	parse_string(IR);
	// print out the interpreted assembly language
	print_instruction(IR, output);
	// execute the instruction and output to the display
	instruction_execute(IR, output);

	// Clean and close everything
	free(IR);
	fclose(input);
  fclose(output);
}

// ===============================================================================================
//	This function takes in an instruction register and returns a set of string literals to be
//	used for print statements
//
void parse_string(Instruction *IR)
{
	for (int i = 0; i < IR->inst_length; i++)
	{
		switch (IR[i].op)
		{
		case 1:
			strcpy(IR[i].op_str, "lit");
			break;
		case 2:
			strcpy(IR[i].op_str, "opr");
			break;
		case 3:
			strcpy(IR[i].op_str, "lod");
			break;
		case 4:
			strcpy(IR[i].op_str, "sto");
			break;
		case 5:
			strcpy(IR[i].op_str, "cal");
			break;
		case 6:
			strcpy(IR[i].op_str, "inc");
			break;
		case 7:
			strcpy(IR[i].op_str, "jmp");
			break;
		case 8:
			strcpy(IR[i].op_str, "jpc");
			break;
		case 9:
			strcpy(IR[i].op_str, "sio");
			break;
		}
	}
}

// ===============================================================================================
//	Manipulates the stack after reading in the op bit, l represents the lexicographical
//	bit and m represents the offset. 
//
void instruction_execute(Instruction *IR, FILE *output)
{
	// create space for the registers and initialize values for pc, bp, and sp
	Register *reg = malloc(sizeof(Register));
	// run is set to 1 until a halt statement is encountered
	int run = 1, temp_bp = 0;
	// initialize sp, bp, and pc
	reg->sp = 0; reg->pc = 0; reg->bp = 1;
	// initialize array to keep track of base pointers
	reg->index = 0; reg->AR[0] = reg->bp; reg->cal = 0;

	for (int i = 1; i < MAX_AR_SIZE; i++)
		reg->AR[i] = -1;

	for (int i = 0; i < MAX_STACK_HEIGHT; i++)
		reg->stack[i] = 0;


  fprintf(output, "\t\t\t\t\tpc\tbp\tsp\tstack\n");
  fprintf(output, "Initial values\t\t\t\t%d\t%d\t%d\t\n", reg->pc, reg->bp, reg->sp);


	// Executing the instructions here. Go into op function if value is 01
	while (run)
	{
		print_initial_values(output, IR, reg->pc);

		switch (IR[reg->pc].op)
		{
			// =============================== LIT =======================================
			//	Push the operand stored in m into the top of the stack
			//
		case 1:
			reg->stack[reg->sp++] = IR[reg->pc].m;
			reg->pc++;
			break;

			// =============================== OPR =======================================
			//	Parse operation according to m operand of the instruction set
			//
		case 2:
			perform_operation(IR, reg);
			reg->pc++;
			break;

			// =============================== LOD =======================================
			//	Load value at top of the stack from the offset
			//
		case 3:
			//reg->stack[reg->sp++] = reg->stack[base(IR, reg, reg->bp) + IR[reg->pc].m];

			temp_bp = base(IR, reg, reg->bp) + IR[reg->pc].m;
			reg->stack[reg->sp++] = reg->stack[temp_bp - 1];
			reg->pc++;
			break;

			// =============================== STO =======================================
			//	Store top stack value l levels down with an offset of m
			//
		case 4:
			reg->stack[base(IR, reg, reg->bp) + IR[reg->pc].m - 1] = reg->stack[reg->sp - 1];
			reg->stack[--reg->sp] = 0;
			reg->pc++;
			break;

			// =============================== CAL =======================================
			//	Create a new activation record. Set cal bit to 1
			//	
		case 5:
			reg->cal = 1;
			reg->stack[reg->sp] = 0;

			// static link (SL)
			reg->stack[reg->sp + 1] = base(IR, reg, reg->bp);

			// dynamic link (DL)
			reg->stack[reg->sp + 2] = reg->bp;

			// return address (RA)
			reg->stack[reg->sp + 3] = reg->pc + 1;

			reg->AR[++reg->index] = reg->sp + 1;
			reg->bp = reg->sp + 1;
			reg->pc = IR[reg->pc].m;
			break;

			// =============================== INC =======================================
			//	Create new locations in the stacy by an increment of sp. 
			//	
		case 6:
			reg->inc = 1;
			reg->sp = reg->sp + IR[reg->pc].m;
			reg->pc++;
			break;

			// =============================== JMP =======================================
			//	Jump to instruction indicated by the value of m
			//
		case 7:
			reg->pc = IR[reg->pc].m;
			break;

			// =============================== JPC =======================================
			//	Jump to m only if the value at the top of the stack is 0
			//
		case 8:
			reg->sp--;
			if (reg->stack[reg->sp] == 0)
				reg->pc = IR[reg->pc].m;
			else
				reg->pc++;

			break;

			// =============================== SIO =======================================
			//	case: 0, 1	Pop the top of the stack and output to screen
			//	case: 0, 2	Store input from user into the stack
			//  case: 0, 3	Return and obliterate everything - HALT
			//
		case 9:
			if (IR[reg->pc].m == 1)
			{
			  printf("%d\n", reg->stack[--reg->sp]);
        fprintf(output, "\n");
				reg->stack[reg->sp] = 0;
				reg->pc++;
				continue;
			}
			else if (IR[reg->pc].m == 2)
			{
				scanf("%d", &reg->stack[reg->sp++]);
				reg->pc++;
        fprintf(output, "\n");
				continue;
			}
			else if (IR[reg->pc].m == 3)
			{
				run = 0; reg->pc = 0; reg->sp = 0; reg->bp = 0;
				break;
			}
			break;
		}
		print_stack(reg, output);

	}
  fprintf(output, "\n");

	free(reg);
}

// ===============================================================================================
//	Executes a set of operations depending on the instruction given by M
//	OP use the two topmost values of the stack.
//
void perform_operation(Instruction *IR, Register * reg)
{
	switch (IR[reg->pc].m)
	{
		// =============================== RET =======================================
		//
	case 0:
		reg->AR[reg->index--] = -1;
		reg->sp = reg->bp - 1;
		reg->pc = reg->stack[reg->sp + 3] - 1;
		reg->bp = reg->stack[reg->sp + 2];
		break;

		// =============================== NEG =======================================
		//
	case 1:
		reg->stack[reg->sp - 1] = 0 - reg->stack[reg->sp - 1];
		break;

		// =============================== ADD =======================================
		//
	case 2:
		reg->sp--;
		reg->stack[reg->sp - 1] = reg->stack[reg->sp - 1] + reg->stack[reg->sp];
		reg->stack[reg->sp] = 0;
		break;

		// =============================== SUB =======================================
		//
	case 3:
		reg->sp--;
		reg->stack[reg->sp - 1] = reg->stack[reg->sp - 1] - reg->stack[reg->sp];
		reg->stack[reg->sp] = 0;
		break;

		// =============================== MUL =======================================
		//
	case 4:
		reg->sp--;
		reg->stack[reg->sp - 1] = reg->stack[reg->sp - 1] * reg->stack[reg->sp];
		reg->stack[reg->sp] = 0;
		break;

		// =============================== DIV =======================================
		//
	case 5:
		reg->sp--;
		reg->stack[reg->sp - 1] = reg->stack[reg->sp - 1] / reg->stack[reg->sp];
		reg->stack[reg->sp] = 0;
		break;

		// =============================== ODD =======================================
		//
	case 6:
		reg->stack[reg->sp - 1] = reg->stack[reg->sp - 1] % 2;
		break;

		// =============================== MOD =======================================
		//
	case 7:
		reg->sp--;
		reg->stack[reg->sp - 1] = reg->stack[reg->sp - 1] % reg->stack[reg->sp];
		reg->stack[reg->sp] = 0;
		break;

		// =============================== EQL =======================================
		//
	case 8:
		reg->sp--;
		reg->stack[reg->sp - 1] = (reg->stack[reg->sp - 1] == reg->stack[reg->sp]) ? 1 : 0;
		reg->stack[reg->sp] = 0;
		break;

		// =============================== NEQ =======================================
		//
	case 9:
		reg->sp--;
		reg->stack[reg->sp - 1] = (reg->stack[reg->sp - 1] != reg->stack[reg->sp]) ? 1 : 0;
		reg->stack[reg->sp] = 0;
		break;

		// =============================== LSS =======================================
		//
	case 10:
		reg->sp--;
		reg->stack[reg->sp - 1] = (reg->stack[reg->sp - 1] < reg->stack[reg->sp]) ? 1 : 0;
		reg->stack[reg->sp] = 0;
		break;

		// =============================== LEQ =======================================
		//
	case 11:
		reg->sp--;
		reg->stack[reg->sp - 1] = (reg->stack[reg->sp - 1] <= reg->stack[reg->sp]) ? 1 : 0;
		reg->stack[reg->sp] = 0;
		break;

		// =============================== GTR =======================================
		//
	case 12:
		reg->sp--;
		reg->stack[reg->sp - 1] = (reg->stack[reg->sp - 1] > reg->stack[reg->sp]) ? 1 : 0;
		reg->stack[reg->sp] = 0;
		break;

		// =============================== GEQ =======================================
		//
	case 13:
		reg->sp--;
		reg->stack[reg->sp - 1] = (reg->stack[reg->sp - 1] >= reg->stack[reg->sp]) ? 1 : 0;
		reg->stack[reg->sp] = 0;
		break;
	}
}

// ===============================================================================================
//	Calculates the base pointer at l levels down
//
int base(Instruction *IR, Register * reg, int temp_bp)
{
	for (int i = 0; i < IR[reg->pc].l; i++)
		temp_bp = reg->stack[temp_bp];

	return temp_bp;
}

// ===============================================================================================
//	Prints the contents of the instruction register in order
//
void print_instruction(Instruction *p, FILE *output)
{

fprintf(output, "Line\tOP\tL\tM\n");

	for (int i = 0; i < p->inst_length; i++)
	{
	  fprintf(output, "%d\t%s\t%d\t%d\n", i, p[i].op_str, p[i].l, p[i].m);
	}
	fprintf(output, "\n");
}

// ===============================================================================================
//	Prints the contents labeled in the inital values column
//
void print_initial_values(FILE *output, Instruction *IR, int pc)
{
  fprintf(output, "%d\t%s\t%d\t%d\t", pc, IR[pc].op_str, IR[pc].l, IR[pc].m);
}

// ===============================================================================================
//	Prints the register operands and the current contents of the stack
//
void print_stack(Register *reg, FILE *output)
{
	int end = reg->sp, index = 1;

  fprintf(output, "\t%d\t%d\t%d\t", reg->pc, reg->bp, reg->sp);

	// checks if instruction was a call, print an extra 4 indices
	if (reg->inc == 1)
	{
		if (reg->cal == 1)
		{
			reg->cal = 0;
		}
	}
	if (reg->cal == 1)
	{
		end += 4;
	}

	for (int i = 0; i < end; i++)
	{
		fprintf(output, "%d ", reg->stack[i]);

		if (i == reg->AR[index] - 2)
		{
		  fprintf(output, "| ");
			index++;
		}
	}
	fprintf(output, "\n");

	reg->inc = 0;
}

