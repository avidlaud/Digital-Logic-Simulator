#include<stdlib.h>
#include<stdio.h>
#include<string.h>
#include<stdbool.h>

/*
 * Structure: var
 *
 * Linked list node to store the name and values of the input, temporary, and output variables.
 *
 * name: String of variable name
 *
 * value: Logical value of variable: 0 or 1
 *
 * done: True if the value of the variable has been already determined
 *
 */
struct var {
	char* name;
	int value;
	struct var* next;
	bool done;
};

/*
 * Structure: line
 *
 * Linked list node to store components of the circuit
 *
 * oper: The type of logic gate
 *	0:NOT
 *	1:AND
 *	2:OR
 *	3:NAND
 *	4:NOR
 *	5:XOR
 *	6:XNOR
 *	7:DECODER
 *	8:MULTIPLEXER
 *
 * num_inputs: Number of input variables, needs to be a power of two
 *
 * num_outputs: Number of output variables
 *
 * inputs: String array of input variable names, used to find the var structs
 *
 * outputs: String array of output variable names, used to find the var structs
 *
 * multi: Only used for multiplexers (oper=8)
 *	Store the encoding of the the multiplexer
 *
 */
struct line {
	int oper;
	int num_inputs;
	int num_outputs;
	char** inputs; //For multiplexers: Store the selects
	char** outputs;
	char** multi; //For multiplexers: Store the encoding
	struct line* next;
};

void insert(char*, int);
void insertLine(int, int, int, char**, char**, char**);
void insertOrdered(struct line*);
int exists(char*);
bool validPowerTwo(int);
int log_2(int);
void setValue(char*, int);
int getValue(char*);
void setDone(char*);
bool getDone(char*);
int myNot(int);
int myAnd(int, int);
int myOr(int, int);
int myNand(int, int);
int myNor(int, int);
int myXor(int, int);
int myXnor(int, int);
int myDecoder(int*, int);
int myMultiplexer(int*, int*, int);
int grayToBinary(int);
int binaryToGray(int);
int* genGrayCode(int);
void makeNot(FILE*);
void makeAnd(FILE*);
void makeOr(FILE*);
void makeNand(FILE*);
void makeNor(FILE*);
void makeXor(FILE*);
void makeXnor(FILE*);
void makeDecoder(FILE*);
void makeMultiplexer(FILE*);
