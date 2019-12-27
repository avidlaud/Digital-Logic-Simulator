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

struct var* head = NULL;
int num_vars = 0;

struct line* linehead = NULL;
struct line* orderhead = NULL;

/*
 * Function: insert
 *
 * Create and insert a new var struct into the linked list
 *
 * name: String of the character name
 * 	Limited to 64 characters
 *
 * value: Logical value of the variable
 *
 */
void insert(char* name, int value) {

	/*Variable already exists*/
	if(exists(name) == 1) {
		return;
	}

	struct var* temp = malloc(sizeof(struct var));
	char* tempname = malloc(64*sizeof(char));
	strcpy(tempname, name);
	temp->name = tempname;
	temp->value = value;
	/*New variable so value has not been determined*/
	temp->done = false;
	/*Insert at head*/
	temp->next = head;
	head = temp;
	num_vars++;
	return;
}

/*
 * Function: insertLine
 *
 * Create and insert a new line struct into the linked list
 *
 * oper: The type of logic gate
 *
 * num_inputs: Number of input variables, needs to be a power of two
 *
 * num_outputs: Number of output variables
 *
 * inputs: String array of input variable names
 *
 * outputs: String array of output variable names
 *
 * multi: Multiplexer encoding; only used for multiplexers, NULL otherwise
 *
 */
void insertLine(int oper, int num_inputs, int num_outputs, char** inputs, char** outputs, char** multi) {
	struct line* temp = malloc(sizeof(struct line));
	char** in = malloc(num_inputs*sizeof(char*));
	for(int i = 0; i < num_inputs; i++) {
		in[i] = malloc(32*sizeof(char));
		strcpy(in[i], inputs[i]);
	}
	char** out = malloc(num_outputs*sizeof(char*));
	for(int i = 0; i < num_outputs; i++) {
		out[i] = malloc(32*sizeof(char));
		strcpy(out[i], outputs[i]);
	}
	if(multi != NULL) {
		char** mul = malloc((1 << num_inputs)*sizeof(char*));
		for(int i = 0; i < (1 << num_inputs); i++) {
			mul[i] = malloc(32*sizeof(char));
			strcpy(mul[i], multi[i]);
		}
		temp->multi = mul;
	}
	else {
		temp->multi = NULL;
	}
	temp->oper = oper;
	temp->num_inputs = num_inputs;
	temp->num_outputs = num_outputs;
	temp->inputs = in;
	temp->outputs = out;
	temp->next = NULL;
	if(linehead == NULL) {
		linehead = temp;
		return;
	}
	struct line* ptr = linehead;
	while(ptr->next != NULL) {
		ptr = ptr->next;
	}
	ptr->next = temp;
	return;
}

/*
 * Function: insertOrdered
 *
 * Create and insert a new line struct into the ordered linked list
 *
 * in: The line to insert
 *
 */
 void insertOrdered(struct line* in) {
	in->next = NULL;
	if(orderhead == NULL) {
		orderhead = in;
		return;
	}
	struct line* ptr = orderhead;
	while(ptr->next != NULL) {
		ptr = ptr->next;
	}
	ptr->next = in;
	return;
}

/*
 * Function: exists
 *
 * Performs a search through the linked list of variables to find if a variable exists
 *
 * name: Name of the variable*
 *
 * returns: 1 if a variable exists, 0 if not
 *
 */
int exists(char* name) {
	/*Is a 0 or 1*/
	if(strcmp(name, "0") == 0 || strcmp(name, "1") == 0) {
		return 1;
	}
	/*No variables in list*/
	if(head == NULL) {
		return 0;
	}
	struct var* ptr = head;
	while(ptr != NULL) {
		if(strcmp(ptr->name, name) == 0) {
			return 1;
		}
		ptr = ptr->next;
	}
	return 0;
}

/*
 * Function: validPowerTwo
 *
 * Determines if a number is a power of two
 *
 * num: the number to check
 *
 * returns: true if power of two, false otherwise
 *
 */
bool validPowerTwo(int num) {
        if((num & (num-1)) == 0) {
                return true;
        }
        return false;
}

/*
 * Function: log_2
 *
 * Calculates the log base 2 of a number
 * Must be power of two to accurate calculate
 *
 * num: the number of perform the logarithmic operation on
 *
 * return: log2 of the number
 */
int log_2(int num) {
        if(num == 0) {
                return -1;
        }
        if(!validPowerTwo(num)) {
                return -2;
        }
        int ret = 0;
        while(num != 1) {
                num = num >> 1;
                ret++;
        }
        return ret;
}

/*
 * Function: setValue
 *
 * Changes the value of a provided variable
 * Assumes that the variable exists
 *
 * name: The variable name
 *
 * value: The new value of the variable
 *
 */
void setValue(char* name, int value) {
	struct var* ptr = head;
	while(strcmp(ptr->name, name) != 0) {
		ptr = ptr->next;
	}
	ptr->value = value;
	return;
}

/*
 * Function: getValue
 *
 * Fetches the value of the variable
 * Assumes that the variable exists
 *
 * name: The name of the variable
 *
 * returns: The value of the variable (0 or 1) or
 * 	0 or 1 if the input string is "0" or "1" respectively
 */
int getValue(char* name) {
	if(strcmp(name, "0") == 0) {
		return 0;
	}
	if(strcmp(name, "1") == 0) {
		return 1;
	}
	struct var* ptr = head;
	while(strcmp(ptr->name, name) != 0) {
		ptr = ptr->next;
	}
	return ptr->value;
}

/*
 * Function: setDone
 *
 * Searches for variable and sets its done field to true
 * Assumes that the variable exists
 *
 * name: The variable name
 *
 */
void setDone(char* name) {
	if(strcmp(name, "0") == 0 || strcmp(name, "1") == 0) {
		return;
	}
	struct var* ptr = head;
	while(strcmp(ptr->name, name) != 0) {
		ptr = ptr->next;
	}
	ptr->done = true;
}

/*
 * Function: getDone
 *
 * Searches for variable and retrieves its done field value
 * Assumes that the variable exists
 *
 * name: The variable name
 *
 * returns: True if the variable is done, false otherwise
 */
bool getDone(char* name) {
	if(strcmp(name, "0") == 0 || strcmp(name, "1") == 0) {
		return true;
	}
	struct var* ptr = head;
	while(strcmp(ptr->name, name) != 0) {
		ptr = ptr->next;
	}
	return ptr->done;
}

/*
 * Functions: Basic logic gates
 *
 * Performs basic logical operations, using bitwise operations where appropriate
 *
 * in1: The first input
 *
 * in2: The second input
 *
 * returns: The value of the logic operation applied to the input(s)
 */

 int myNot(int in) {
 	return (in == 0);
 }

 int myAnd(int in1, int in2) {
 	return in1 & in2;
 }

 int myOr(int in1, int in2) {
 	return in1 | in2;
 }

 int myNand(int in1, int in2) {
 	return myNot(myAnd(in1, in2));
 }

 int myNor(int in1, int in2) {
 	return myNot(myOr(in1, in2));
 }

 int myXor(int in1, int in2) {
 	return in1 ^ in2;
 }

 int myXnor(int in1, int in2) {
 	return myNot(myXor(in1, in2));
 }

 /*
  * Function: myDecoder
  *
  * Simulates a decoder logic gate that takes n inputs and 2^n outputs
  * Outputs a "1" to the selected output in gray code order
  *
  * in: Array of inputs
  *
  * num_inputs: The number of inputs
  *
  * returns: An int with the corresponding outputs in binary
  */
int myDecoder(int* in, int num_inputs) {
	int code = 0;
	for(int i = 0; i < num_inputs; i++) {
		code = code << 1;
		code = code + in[i];
	}
	return grayToBinary(code);
}

/*
 * Function: myMultiplexer
 *
 * Simulates a multiplexer logic gate that takes n inputs, 2^n encoding inputs, and 1 output
 * Outputs a the value of the encoding input based on the inputs
 * The encoding inputs are in gray code
 *
 * encoding: Encoding array
 *
 * in: Array of inputs
 *
 * num_inputs: The number of inputs
 *
 * returns: The value of the selected input
 */
int myMultiplexer(int* encoding, int* in, int num_inputs) {
	int code = 0;
	for(int i = 0; i < num_inputs; i++) {
		code = code << 1;
		code = code + in[i];
	}
	return encoding[grayToBinary(code)];
}

/*
 * Function: grayToBinary
 *
 * Converts gray code stored as an int to binary stored as a int
 *
 * in: Gray code stored as an int
 *
 * returns: Binary stored as an int
 */
int grayToBinary(int in) {
	int temp = in;
	while(in >>= 1) temp ^= in;
	return temp;
}

/*
 * Function: binaryToGray
 *
 * Converts binary stored as an in to gray code stored as an int
 *
 * in: Binary stored as an int
 *
 * returns: Gray code stored as an int
 */
int binaryToGray(int in) {
	return in ^ (in-1);
}

/*
 * Function: genGrayCode
 *
 * Generates a sequence of gray code
 *
 * bits: Number of bits
 * 	Generates 2^bits gray code numbers
 *
 * returns: An array of ints, with the sequence of gray code
 */
int* genGrayCode(int bits) {
	int n = 2 << (bits-1);
	int* ret = malloc(n*sizeof(int));
	for(int i = 0; i < n; i++) {
		ret[i] = i ^ (i>>1);
	}
	return ret;
}

/*
 * Function: makeNot
 *
 * Creates a NOT logic gate
 *
 * datafile: The file in which the circuit is defined
 * 	The file pointer has been set to the start of the NOT gate arguments
 *
 */
void makeNot(FILE* datafile) {
	char** in = malloc(sizeof(char*));
	in[0] = malloc(32*sizeof(char));
	char** out = malloc(sizeof(char*));
	out[0] = malloc(32*sizeof(char));
	fscanf(datafile, " %s", in[0]);
	insert(in[0], 0);
	fscanf(datafile, " %s", out[0]);
	insert(out[0], 0);
	insertLine(0, 1, 1, in, out, NULL);
	free(in[0]);
	free(in);
	free(out[0]);
	free(out);
}

/*
 * Function: makeAnd
 *
 * Creates a AND logic gate
 *
 * datafile: The file in which the circuit is defined
 * 	The file pointer has been set to the start of the AND gate arguments
 *
 */
void makeAnd(FILE* datafile) {
	char** in = malloc(2*sizeof(char*));
	in[0] = malloc(32*sizeof(char));
	in[1] = malloc(32*sizeof(char));
	char** out = malloc(sizeof(char*));
	out[0] = malloc(32*sizeof(char));
	fscanf(datafile, " %s", in[0]);
	insert(in[0], 0);
	fscanf(datafile, " %s", in[1]);
	insert(in[1], 0);
	fscanf(datafile, " %s", out[0]);
	insert(out[0], 0);
	insertLine(1, 2, 1, in, out, NULL);
	free(in[0]);
	free(in[1]);
	free(in);
	free(out[0]);
	free(out);
}

/*
 * Function: makeOr
 *
 * Creates a OR logic gate
 *
 * datafile: The file in which the circuit is defined
 * 	The file pointer has been set to the start of the OR gate arguments
 *
 */
void makeOr(FILE* datafile) {
	char** in = malloc(2*sizeof(char*));
	in[0] = malloc(32*sizeof(char));
	in[1] = malloc(32*sizeof(char));
	char** out = malloc(sizeof(char*));
	out[0] = malloc(32*sizeof(char));
	fscanf(datafile, " %s", in[0]);
	insert(in[0], 0);
	fscanf(datafile, " %s", in[1]);
	insert(in[1], 0);
	fscanf(datafile, " %s", out[0]);
	insert(out[0], 0);
	insertLine(2, 2, 1, in, out, NULL);
	free(in[0]);
	free(in[1]);
	free(in);
	free(out[0]);
	free(out);
}

/*
 * Function: makeNand
 *
 * Creates a NAND logic gate
 *
 * datafile: The file in which the circuit is defined
 * 	The file pointer has been set to the start of the NAND gate arguments
 *
 */
void makeNand(FILE* datafile) {
	char** in = malloc(2*sizeof(char*));
	in[0] = malloc(32*sizeof(char));
	in[1] = malloc(32*sizeof(char));
	char** out = malloc(sizeof(char*));
	out[0] = malloc(32*sizeof(char));
	fscanf(datafile, " %s", in[0]);
	insert(in[0], 0);
	fscanf(datafile, " %s", in[1]);
	insert(in[1], 0);
	fscanf(datafile, " %s", out[0]);
	insert(out[0], 0);
	insertLine(3, 2, 1, in, out, NULL);
	free(in[0]);
	free(in[1]);
	free(in);
	free(out[0]);
	free(out);
}

/*
 * Function: makeNor
 *
 * Creates a NOR logic gate
 *
 * datafile: The file in which the circuit is defined
 * 	The file pointer has been set to the start of the NOR gate arguments
 *
 */
void makeNor(FILE* datafile) {
	char** in = malloc(2*sizeof(char*));
	in[0] = malloc(32*sizeof(char));
	in[1] = malloc(32*sizeof(char));
	char** out = malloc(sizeof(char*));
	out[0] = malloc(32*sizeof(char));
	fscanf(datafile, " %s", in[0]);
	insert(in[0], 0);
	fscanf(datafile, " %s", in[1]);
	insert(in[1], 0);
	fscanf(datafile, " %s", out[0]);
	insert(out[0], 0);
	insertLine(4, 2, 1, in, out, NULL);
	free(in[0]);
	free(in[1]);
	free(in);
	free(out[0]);
	free(out);

}

/*
 * Function: makeXor
 *
 * Creates a XOR logic gate
 *
 * datafile: The file in which the circuit is defined
 * 	The file pointer has been set to the start of the XOR gate arguments
 *
 */
void makeXor(FILE* datafile) {
	char** in = malloc(2*sizeof(char*));
	in[0] = malloc(32*sizeof(char));
	in[1] = malloc(32*sizeof(char));
	char** out = malloc(sizeof(char*));
	out[0] = malloc(32*sizeof(char));
	fscanf(datafile, " %s", in[0]);
	insert(in[0], 0);
	fscanf(datafile, " %s", in[1]);
	insert(in[1], 0);
	fscanf(datafile, " %s", out[0]);
	insert(out[0], 0);
	insertLine(5, 2, 1, in, out, NULL);
	free(in[0]);
	free(in[1]);
	free(in);
	free(out[0]);
	free(out);

}

/*
 * Function: makeXnor
 *
 * Creates a XNOR logic gate
 *
 * datafile: The file in which the circuit is defined
 * 	The file pointer has been set to the start of the XNOR gate arguments
 *
 */
void makeXnor(FILE* datafile) {
	char** in = malloc(2*sizeof(char*));
	in[0] = malloc(32*sizeof(char));
	in[1] = malloc(32*sizeof(char));
	char** out = malloc(sizeof(char*));
	out[0] = malloc(32*sizeof(char));
	fscanf(datafile, " %s", in[0]);
	insert(in[0], 0);
	fscanf(datafile, " %s", in[1]);
	insert(in[1], 0);
	fscanf(datafile, " %s", out[0]);
	insert(out[0], 0);
	insertLine(6, 2, 1, in, out, NULL);
	free(in[0]);
	free(in[1]);
	free(in);
	free(out[0]);
	free(out);
}

/*
 * Function: makeDecoder
 *
 * Creates a DECODER logic gate
 *
 * datafile: The file in which the circuit is defined
 * 	The file pointer has been set to the start of the DECODER gate arguments
 *
 */
void makeDecoder(FILE* datafile) {
	int num_inputs = 0;
	fscanf(datafile, " %d", &num_inputs);
	char** in = malloc(num_inputs*sizeof(char*));
	for(int i = 0; i < num_inputs; i++) {
		in[i] = malloc(32*sizeof(char));
	}
	int num_outputs = (1 << num_inputs);
	char** out = malloc(num_outputs*sizeof(char*));
	for(int i = 0; i < num_outputs; i++) {
		out[i] = malloc(32*sizeof(char));
	}
	for(int i = 0; i < num_inputs; i++) {
		fscanf(datafile, " %s", in[i]);
		insert(in[i], 0);
	}
	for(int i = 0; i < num_outputs; i++) {
		fscanf(datafile, " %s", out[i]);
		insert(out[i], 0);
	}
	insertLine(7, num_inputs, num_outputs, in, out, NULL);
}

/*
 * Function: makeMultiplexer
 *
 * Creates a MULTIPLEXER logic gate
 *
 * datafile: The file in which the circuit is defined
 * 	The file pointer has been set to the start of the MULTIPLEXER gate arguments
 *
 */
void makeMultiplexer(FILE* datafile) {
	int num_inputs = 0;
	fscanf(datafile, " %d", &num_inputs);
	int num_multi = num_inputs;
	num_inputs = log_2(num_inputs);;
	char** in = malloc(num_inputs*sizeof(char*));
	for(int i = 0; i < num_inputs; i++) {
		in[i] = malloc(32*sizeof(char));
	}
	char** multi = malloc(num_multi*sizeof(char*));
	for(int i = 0; i < num_multi; i++) {
		multi[i] = malloc(32*sizeof(char));
	}
	char** out = malloc(sizeof(char*));
	out[0] = malloc(32*sizeof(char));
	for(int i = 0; i < num_multi; i++) {
		fscanf(datafile, " %s", multi[i]);
		insert(multi[i], 0);
	}
	for(int i = 0; i < num_inputs; i++) {
		fscanf(datafile, " %s", in[i]);
		insert(in[i], 0);
	}
	fscanf(datafile, " %s", out[0]);
	insert(out[0], 0);
	insertLine(8, num_inputs, 1, in, out, multi);
	/*Free string arrays*/
}

int main(int argc, char** argv) {

	/*Checks for a file name argument*/
	if(argc < 2){
		printf("error");
		exit(0);
	}
	FILE* datafile = fopen(argv[1], "r");

	fpos_t filestart;
	fgetpos(datafile, &filestart);
	char* findline = malloc(512*sizeof(char));
	char* findoper = malloc(32*sizeof(char));
	bool inputfound = false;
	bool outputfound = false;
	fpos_t pos;
	int num_inputs = 0;
	int num_outputs = 0;
	char* tempname = malloc(64*sizeof(char));
	struct var** inputArray;
	struct var** outputArray;
	/*Scan once for inputs and outputs*/
	while(!inputfound || !outputfound) {
		fgetpos(datafile, &pos);
		fscanf(datafile, "%[^\n]", findline);
		sscanf(findline, "%s ", findoper);
		if(strcmp(findoper, "INPUTVAR") == 0) {
			fsetpos(datafile, &pos);
			fscanf(datafile, "INPUTVAR %d", &num_inputs);
			/*Input array to store just the input nodes*/
			inputArray = malloc(num_inputs*sizeof(struct var*));
			for(int i = 0; i < num_inputs; i++) {
				fscanf(datafile, " %s", tempname);
				insert(tempname, 0);
				inputArray[i] = head; //Insert the last inputted variable into the array
				head->done = true; //Input variables are considered "done"
			}
			inputfound = true;
		}
		else if(strcmp(findoper, "OUTPUTVAR") == 0) {
			fsetpos(datafile, &pos);
			fscanf(datafile, "OUTPUTVAR %d", &num_outputs);
			/*Output array to store just the output nodes*/
			outputArray = malloc(num_outputs*sizeof(struct var*));
			for(int i = 0; i < num_outputs; i++) {
				fscanf(datafile, " %s", tempname);
				insert(tempname, 0);
				outputArray[i] = head; //Insert the last inputted variable into the array
				head->done = true; //Should never happen
			}
			outputfound = true;
		}
		fscanf(datafile, "\n");
	}
	/*Scan the various logic gates*/
	fsetpos(datafile, &filestart);
	char* fn = malloc(32* sizeof(char));
	char* dump = malloc(256*sizeof(char));
	while(fscanf(datafile, "%s ", fn) != EOF) {
		/*Determine the logic gate*/
		if(strcmp(fn, "NOT") == 0) {
			makeNot(datafile);
		}
		else if(strcmp(fn, "AND") == 0) {
			makeAnd(datafile);
		}
		else if(strcmp(fn, "OR") == 0) {
			makeOr(datafile);
		}
		else if(strcmp(fn, "NAND") == 0) {
			makeNand(datafile);
		}
		else if(strcmp(fn, "NOR") == 0) {
			makeNor(datafile);
		}
		else if(strcmp(fn, "XOR") == 0) {
			makeXor(datafile);
		}
		else if(strcmp(fn, "XNOR") == 0) {
			makeXnor(datafile);
		}
		else if(strcmp(fn, "DECODER") == 0) {
			makeDecoder(datafile);
		}
		else if(strcmp(fn, "MULTIPLEXER") == 0) {
			makeMultiplexer(datafile);
		}
		else { //Input or output
			fscanf(datafile, "%[^\n]", dump);
		}
		fscanf(datafile, "\n");
	}

	/*Sort the operations*/
	struct line* optr = linehead;
	struct line* prev = NULL;
	int n_in = 0;
	int n_out = 0;
	bool done = true;
	int n_multi = 0;

	while(linehead != NULL) {
		while(optr != NULL) { //Iterate through the unordered list and find a node that can be solved for
			done = true;
			n_in = optr->num_inputs;
			for(int a = 0; a < n_in; a++) {
				if(!getDone(optr->inputs[a])) { //True if not done
					done = false;
				}
			}
			/*Need to check multiplexer as well for encoding*/
			if(optr->oper == 8) {
				n_multi = (1 << optr->num_inputs);
				for(int c = 0; c < n_multi; c++) {
					if(!getDone(optr->multi[c])) {
						done = false;
					}
				}
			}
			if(done) { //Can be added to the list, update the outputs
				n_out = optr->num_outputs;
				for(int b = 0; b < n_out; b++) {
					setDone(optr->outputs[b]);
				}
				if(prev == NULL) { //case: optr is linehead
					linehead = optr->next;
				}
				else {
					prev->next = optr->next;
				}
				insertOrdered(optr); //Insert at end of orderhead
			}
			prev = optr;
			optr = optr->next;
		}
		optr = linehead;
		prev = NULL;
	}

	linehead = orderhead;

	/*Generate the input combinations*/
	int* grayArray = genGrayCode(num_inputs);
	int correctBit = 0;
	for(int i = 0; i < (1 << (num_inputs)); i++) {
		/*Set the inputs*/
		for(int j = 0; j < num_inputs; j++) {
		correctBit = ((grayArray[i] >> j) & 0x1);
			inputArray[num_inputs-j-1]->value = correctBit;
		}
		/*Run through the circuit*/
		struct line* ptr = linehead;
		if(ptr == NULL) {
			exit(0);
		}
		int tempresult = 0;
		while(ptr != NULL) {
			switch(ptr->oper) {
				case 0: //NOT
					{
					tempresult = myNot(getValue(ptr->inputs[0]));
					setValue(ptr->outputs[0], tempresult);
					break;
					}
				case 1: //AND
					{
					tempresult = myAnd(getValue(ptr->inputs[0]), getValue(ptr->inputs[1]));
					setValue(ptr->outputs[0], tempresult);
					break;
					}
				case 2: //OR
					{
					tempresult = myOr(getValue(ptr->inputs[0]), getValue(ptr->inputs[1]));
					setValue(ptr->outputs[0], tempresult);
					break;
					}
				case 3: //NAND
					{
					tempresult = myNand(getValue(ptr->inputs[0]), getValue(ptr->inputs[1]));
					setValue(ptr->outputs[0], tempresult);
					break;
					}
				case 4: //NOR
					{
					tempresult = myNor(getValue(ptr->inputs[0]), getValue(ptr->inputs[1]));
					setValue(ptr->outputs[0], tempresult);
					break;
					}
				case 5: //XOR
					{
					tempresult = myXor(getValue(ptr->inputs[0]), getValue(ptr->inputs[1]));
					setValue(ptr->outputs[0], tempresult);
					break;
					}
				case 6: //XNOR
					{
					tempresult = myXnor(getValue(ptr->inputs[0]), getValue(ptr->inputs[1]));
					setValue(ptr->outputs[0], tempresult);
					break;
					}
				case 7: //DECODER
					{
					int* inputDecInt = malloc(ptr->num_inputs*sizeof(int)); //Convert inputs to int
					for(int k = 0; k < ptr->num_inputs; k++) {
						inputDecInt[k] = getValue(ptr->inputs[k]);
					}
					tempresult = myDecoder(inputDecInt, ptr->num_inputs);
					for(int l = 0; l < (1 << (ptr->num_inputs)); l++) {
						if(l == (tempresult)) {
							setValue(ptr->outputs[l], 1);
						}
						else { //The 0's
							setValue(ptr->outputs[l], 0);
						}
					}
					break;
					}
				case 8: //MULTIPLEXER
					{
					int* encodingInt = malloc((1 << (ptr->num_inputs))*sizeof(int)); //Convert encoding to int
					for(int k = 0; k < (1 << (ptr->num_inputs)); k++) {
						encodingInt[k] = getValue(ptr->multi[k]);
					}
					int* inputMultiInt = malloc(ptr->num_inputs*sizeof(int));
					for(int l = 0; l < ptr->num_inputs; l++) {
						inputMultiInt[l] = getValue(ptr->inputs[l]);
					}
					tempresult = myMultiplexer(encodingInt, inputMultiInt, ptr->num_inputs);
					setValue(ptr->outputs[0], tempresult);
					break;
					}
			}
			ptr=ptr->next; //Move to next operation
		}
		/*Print the inputted value*/
		for(int k = 0; k < num_inputs; k++) {
			printf("%d ", inputArray[k]->value);
		}
		/*Print the output*/
		for(int k = 0; k < num_outputs; k++) {
			printf("%d", outputArray[k]->value);
			if(k != (num_outputs - 1)) {
				printf(" ");
			}
		}
		printf("\n");
	}
	/*Free the linked lists*/
	struct var* currVar;
	while((currVar = head) != NULL) {
		head = head->next;
		free(currVar);
	}
	struct line* currLine;
	while((currLine = linehead) != NULL) {
		linehead = linehead->next;
		free(currLine);
	}
	return 0;
}
