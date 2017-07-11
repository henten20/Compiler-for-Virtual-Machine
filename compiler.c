// ===================================
//	COP 3402 - SUMMER 2017
//	Author: Henry Ton
//	COMPILER FOR VIRTUAL MACHINE
// ===================================

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_CODE_SIZE 500

// Prototype functions for parser.c and PM_0.c
void lexi(FILE *, FILE*);
void parse(FILE *);
void VM();
void output_lexemes();
void output_assembly_code();
void output_vm();
void append_output();

int main(int argc, char **argv)
{
  FILE *input = fopen("in.txt", "r");
  FILE *output = fopen("out.txt", "w");
  FILE *parser_input;

	int arguments = 1;

  if (argc < 1 || argc > 4)
  {
			printf("ERROR: incorrect input arguments.\n");
      printf("Proper format: ./a.out -l -a - v. \n");
			exit(1);
  }

  // run the input code into the lexical analyzer
  lexi(input, output);
  
	// parse the code
  parser_input = fopen("out.txt", "r");
	parse(parser_input);
 
  // run the code and excute on the virtual machine
	VM();

	while (arguments < argc)
	{
		if (strcmp(argv[arguments], "-l") == 0)
		{
			output_lexemes();
		}
		if (strcmp(argv[arguments], "-a") == 0)
		{
			output_assembly_code();
		}
		if (strcmp(argv[arguments], "-v") == 0)
		{
			output_vm();
		}

		arguments++;
	}
 
  fclose(parser_input);
  append_output();

	return 0;
}

// outputs the lexigraphical content to screen
void output_lexemes()
{
  char line[MAX_CODE_SIZE];
  
  FILE *input = fopen("out.txt", "r");
  
  if(input == NULL)
  {
    printf("Error in outputting lexemes. \n");
    exit(1);
  }
  
  printf("\n");
  while(fgets(line, MAX_CODE_SIZE, input) != NULL)
    printf("%s", line);
  printf("\n");
  fclose(input);
}

// outputs assembly code to screen
void output_assembly_code()
{
  char line[MAX_CODE_SIZE];
  
  FILE *input = fopen("vminput.txt", "r");
  
  if(input == NULL)
  {
    printf("Error in outputting assembly code. \n");
    exit(1);
  }
  
  printf("\nAssembly code:\n");
  while(fgets(line, 3, input) != NULL)
    printf("%s", line);
  printf("\n");
  fclose(input);
}

// outputs the virtual stack to screen
void output_vm()
{
  char line[MAX_CODE_SIZE];
  
  FILE *input = fopen("vmoutput.txt", "r");
  
  if(input == NULL)
  {
    printf("Error in outputting stack. \n");
    exit(1);
  }
  
  printf("\nVirtual Machine Stack:\n");
  while(fgets(line, MAX_CODE_SIZE, input) != NULL)
    printf("%s", line);
  printf("\n");
  fclose(input);
}

// creates a single file containing all outputs
void append_output()
{
  char line[MAX_CODE_SIZE];

  FILE *input1 = fopen("out.txt", "r");
  FILE *input2 = fopen("vminput.txt", "r");
  FILE *input3 = fopen("vmoutput.txt", "r");
  FILE *output = fopen("output.txt", "w");
  
  if(input1 == NULL || input2 == NULL || input3 == NULL)
    exit(1);
    
  while(fgets(line, MAX_CODE_SIZE, input1) != NULL)
    fprintf(output, "%s", line);
    
  fprintf(output, "\n\nAssembly code:\n");
  while(fgets(line, MAX_CODE_SIZE, input2) != NULL)
    fprintf(output, "%s", line);
  fprintf(output, "\nVirtual Machine Stack: \n");
  while(fgets(line, MAX_CODE_SIZE, input3) != NULL)
    fprintf(output, "%s", line);
  
  fclose(input1);
  fclose(input2);
  fclose(input3);
  fclose(output);
  
  remove("out.txt");
  remove("vminput.txt");
  remove("vmoutput.txt");
}
