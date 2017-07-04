// ===================================
//	COP 3402 - SUMMER 2017
//	Author: Henry Ton
//	COMPILER FOR VIRTUAL MACHINE
// ===================================

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Prototype functions for parser.c and PM_0.c
void parse(FILE *);
void VM();
void print_lexemes();
void print_assembly_code();

// all print statements will be done to the screen
int main(int argc, char **argv)
{
	int arguments = 2;

	while (1)
	{
		if (argc < 2)
		{
			printf("Error: not enough arguments at command line.\n");
			printf("Proper format: ./compiler file.txt -l -a - v.\n");
			continue;
		}
		else if (argc > 5)
		{
			printf("Too many arguments at command line.\n");
			printf("Proper format: ./compiler file.txt -l -a - v.\n");
			continue;
		}
		break;
	}

	if (argv[1] == NULL)
	{
		printf("Error: No file input detected. \n");
		printf("Proper format: ./compiler file.txt -l -a - v.\n");
		exit(1);
	}

	FILE *input = fopen(argv[1], "r");

	if (!input)
	{
		printf("Error: file could not be opened. Please check the file for proper format. \n");
		exit(1);
	}

	// parse the code
	parse(input);
	fclose(input);

	while (arguments < argc)
	{
		// print lexemes and tokens
		if (strcmp(argv[arguments], "-l") == 0)
		{
			print_lexemes();
		}
		// print the generated assembly code
		if (strcmp(argv[arguments], "-a") == 0)
		{
			print_assembly_code();
		}
		// print the vmoutput
		if (strcmp(argv[arguments], "-v") == 0)
		{
			VM();
		}

		arguments++;
	}

	return 0;
}