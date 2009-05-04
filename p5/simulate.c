/* EECS 370 LC-2K8 Instruction-level simulator */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define NUMMEMORY 65536 /* maximum number of words in memory */
#define NUMREGS 8 /* number of machine registers */
#define MAXLINELENGTH 1000	

#define M_OPCODE 29360128
#define M_REGA 3670016
#define M_REGB 458752
#define M_DESTREG 7
#define M_OFFSETFIELD 65535

#define B_OPCODE 22
#define B_REGA 19
#define B_REGB 16
#define B_DESTREG 0
#define B_OFFSETFIELD 0

#define I_ADD	0
#define	I_NAND	1
#define I_LW	2
#define I_SW	3
#define	I_BEQ	4
#define	I_JALR	5
#define	I_HALT	6
#define	I_NOOP	7

#define MAX_CACHE_BLOCKS 256

enum actionType
        {cacheToProcessor, processorToCache, memoryToCache, cacheToMemory,
        cacheToNowhere};

typedef struct stateStruct {
    int pc;
    int mem[NUMMEMORY];
    int reg[NUMREGS];
    int numMemory;
} stateType;

void printState(stateType *);
int convertNum(int num);

void initialize(stateType *);
void simulate(stateType *);

int getOpcode(int num);
int getRegA(int num);
int getRegB(int num);
int getOffsetField(int num);

int blockSizeInWords;
int numberOfSets;
int blocksPerSet;
int cacheSize;

int
main(int argc, char *argv[])
{
    char line[MAXLINELENGTH];
    stateType state;
    FILE *filePtr;

    if (argc != 5) {
	printf("error: usage: %s simulate program.mc blockSizeInWords numberOfSets blocksPerSet\n", argv[0]);
	exit(1);
    }

    blockSizeInWords = atoi(argv[2]);
    numberOfSets = atoi(argv[3]);
    blocksPerSet = atoi(argv[4]);
    cacheSize = blockSizeInWords * numberOfSets * blocksPerSet;

    filePtr = fopen(argv[1], "r");
    if (filePtr == NULL) {
	printf("error: can't open file %s", argv[1]);
	perror("fopen");
	exit(1);
    }

    /* read in the entire machine-code file into memory */
    for (state.numMemory = 0; fgets(line, MAXLINELENGTH, filePtr) != NULL;
	state.numMemory++) {
	
		if (sscanf(line, "%d", state.mem+state.numMemory) != 1) {
		printf("error in reading address %d\n", state.numMemory);
		exit(1);
		}
	
	printf("memory[%d]=%d\n", state.numMemory, state.mem[state.numMemory]);
    }
	initialize(&state);
	simulate(&state);
    return(0);
}

void
printState(stateType *statePtr)
{
    int i;
    printf("\n~~~\nstate:\n");
    printf("\tpc %d\n", statePtr->pc);
    printf("\tmemory:\n");
	for (i=0; i<statePtr->numMemory; i++) {
	    printf("\t\tmem[ %d ] %d\n", i, statePtr->mem[i]);
	}
    printf("\tregisters:\n");
	for (i=0; i<NUMREGS; i++) {
	    printf("\t\treg[ %d ] %d\n", i, statePtr->reg[i]);
	}
    printf("end state\n");
}

void mem(int pc)
{
}

void 
simulate(stateType * state)
{
	int abort = 0;
	int inst, opcode, i_regA, i_regB, i_destReg, offsetField, v_regA, v_regB;
	int num_insts = 0;
	while(!abort)
	{
		printState(state);
		inst = state->mem[state->pc];
		opcode = (inst & M_OPCODE) >> B_OPCODE;
		i_regA = (inst & M_REGA) >> B_REGA;
		i_regB = (inst & M_REGB) >> B_REGB;
		i_destReg = (inst & M_DESTREG) >> B_DESTREG;
		offsetField = convertNum(inst & M_OFFSETFIELD) >> B_OFFSETFIELD;
		
		v_regA = state->reg[i_regA];
		v_regB = state->reg[i_regB];
		
		if (opcode == I_ADD)
		{
			state->reg[i_destReg] = v_regA + v_regB;
		}
		
		else if (opcode == I_NAND)
		{
			state->reg[i_destReg] = ~(v_regA & v_regB);
		}

		else if (opcode == I_LW)
		{
			state->reg[i_regB] = state->mem[v_regA + offsetField];
		}
		
		else if (opcode == I_SW)
		{
			state->mem[v_regA + offsetField] = v_regB;
		}
		
		else if (opcode == I_BEQ)
		{
			if (v_regA == v_regB)
				state->pc += offsetField;
		}
		
		else if (opcode == I_JALR)
		{
			state->reg[i_regB] = state->pc + 1;
			state->pc  = v_regA - 1;
		}
		
		else if (opcode == I_HALT)
		{
			abort = 1;
		}
		
		else if (opcode == I_NOOP)
		{
			//Doing nothing. Whee.
		}
		
		num_insts++;
		state->pc++;
	}
	
	printf("machine halted\n");
	printf("total of %d instructions executed\n", num_insts);
	printf("final state of machine:\n");
	
	printState(state);
}

void
initialize(stateType * state)
{
	int i = 0;
	for (i=0; i<NUMREGS; i++) 
	{
	    state->reg[i] = 0;
	}

	state->pc = 0;
}


int
convertNum(int num)
{
	/* convert a 16-bit number into a 32-bit Sun integer */
	if (num & (1<<15) ) {
		num -= (1<<16);
	}
	return(num);
}

/*
 * Log the specifics of each cache action.
 *
 * address is the starting word address of the range of data being transferred.
 * size is the size of the range of data being transferred.
 * type specifies the source and destination of the data being transferred.
 *     cacheToProcessor: reading data from the cache to the processor
 *     processorToCache: writing data from the processor to the cache
 *     memoryToCache: reading data from the memory to the cache
 *     cacheToMemory: evicting cache data by writing it to the memory
 *     cacheToNowhere: evicting cache data by throwing it away
 */
void
printAction(int address, int size, enum actionType type)
{
    printf("@@@ transferring word [%d-%d] ", address, address + size - 1);
    if (type == cacheToProcessor) {
        printf("from the cache to the processor\n");
    } else if (type == processorToCache) {
        printf("from the processor to the cache\n");
    } else if (type == memoryToCache) {
        printf("from the memory to the cache\n");
    } else if (type == cacheToMemory) {
        printf("from the cache to the memory\n");
    } else if (type == cacheToNowhere) {
        printf("from the cache to nowhere\n");
    }
}
