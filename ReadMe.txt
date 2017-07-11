========================================================================
    CONSOLE APPLICATION : Compiler Project Overview
	Author: Henry Ton
========================================================================

This file contains a summary of what you will find in each of the files that
make up the Compiler application.


compiler.c
    This is the main project c rile that contains the main() function. It will be responsible
	for executing programs depending on input from the command line. The format for compiling:

		gcc compiler.c lexicalAnalyzer.c parser.c virtualMachine.c

	NOTE: An ouput file from the lexicalAnalyzer.c program will be produced to be used by the
	parser portion of the program. The output will be in the format:

		Symbolic Lexeme List:
			lexeme lexeme ... lexeme

		Lexeme List:
			sym sym ...  sym

	This format is the default text file produced from the lexicalAnalyzer provided from Webcourses.
	To execute the compiler.c program, input the following into the command line:

		./a.out  -l -a -v

	where the the file input will be read in automatically as "in.txt". Please make sure in.txt is 
	included in the directory and has some sort of code typed in.

	At minimum, the command line arguments must have at least "./a.out
	-l, -a, -v are additional commands that will output the list of lexemes, assembly code,
	and virtual machine output.

	Some examples of acceptable command line input:

		- ./a.out 
		- ./a.out  -l 
		- ./a.out filename.txt -a -v
		- ./a.out filename.txt -l -a 
		- ./a.out filename.txt -l -v

lexicalAnalyzer.c
	This is a modified version of the provided lexical analyzer. This reads in "in.txt" from the directory and will parse
	the code out into a list of symbolic lexems and a list of symbol tokens. A file named "out.txt" will be produced
	and used by parser.c

parser.c
    This parser.c file will take in the filename.txt file and parse out the intermediate symbols in order
	to determine if the grammar of the code is syntactically correct. A correct piece of code will automatically
	display the following:

		"No errors, program is syntactically correct."

	Any error within the code will display an error code and safely exit out of the program.
	Once the parsing is completed. A list of generated assembly code will be produced and written to a file called
	"vminput.txt". 

virtualMachine.c
    This is virtual machine program created for homework 1. This program will take the "vminput.txt" file created from
	the parser.c program and execute the assembly code. Selecting "-v" as a command argument will display the 
	operation input and stack contents similar to that of homework 1. A corresponding text file will also be produced.
	Output to the screen will be displayed on the next line following any "SIO 0 1" operation. Input will also be read in
	after every "SIO 0 2" operation.

/////////////////////////////////////////////////////////////////////////////
Other standard files:

parser.h
    This header file will contain all neccessary struct definitions, enums, function prototypes, etc needed
	to properly run the parser.c program.

PM_0.h
	This header file will contain all neccessary struct definitions, parameters, function prototypes, etc needed
	to properly run the PM_0 program.

/////////////////////////////////////////////////////////////////////////////
Additional files needed:

- in.txt

/////////////////////////////////////////////////////////////////////////////
Other notes:

An "output.txt" file will be automatically produced for every instance of the program. This output file will
contain the contents for the lexems and symbols, the assembly code, and the virtual machine stack trace.

All intermediate files such as "vminput.txt", "vmoutput.txt", "lexicaloutput.txt", etc will not be kept and are
deleted at the end of the program.

/////////////////////////////////////////////////////////////////////////////
