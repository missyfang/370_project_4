#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define NUMMEMORY 65536 /* maximum number of words in memory */
#define NUMREGS 8 /* number of machine registers */
#define MAXLINELENGTH 1000
// intialize to 0
// set nummemory to lines
// loop while less than nummemory
// extract bits
// noop - do nothing?
// halt - stop?
// add
// lw loads regs from memory
// sw will modify memory
// beq - pc changes but how do you actually move around.
// when to print, not every interation ??


typedef struct stateStruct {
    int pc;
    int mem[NUMMEMORY];
    int reg[NUMREGS];
    int numMemory;
} stateType;

void printState(stateType *);
int convertNum(int);

int
main(int argc, char *argv[])
{
    char line[MAXLINELENGTH];
    stateType state;
    FILE *filePtr;

    if (argc != 2) {
        printf("error: usage: %s <machine-code file>\n", argv[0]);
        exit(1);
    }

    filePtr = fopen(argv[1], "r");
    if (filePtr == NULL) {
        printf("error: can't open file %s", argv[1]);
        perror("fopen");
        exit(1);
    }

    /* read the entire machine-code file into memory */   // add a count here to count the number of lines in memory
    int count = 0; // count the number of lines in memory
    for (state.numMemory = 0; fgets(line, MAXLINELENGTH, filePtr) != NULL;
            state.numMemory++) {
        if (sscanf(line, "%d", state.mem+state.numMemory) != 1) {
            printf("error in reading address %d\n", state.numMemory);
            exit(1);
        }
        printf("memory[%d]=%d\n", state.numMemory, state.mem[state.numMemory]);
        count++;
    }
 
    // initalize registars and pc to 0
    state.pc = 0;
    for( int i = 0 ; i  < NUMREGS ; i++ ){
        state.reg[i] = 0;
    }
    // useful variables
    state.numMemory = count;  // number of lines in memory
    int opcode = 0;
    int regA = 0;
    int regB = 0;
    int dest_reg = 0;
    int offset = 0;
    // intruction count
    int j = 0;
    while (state.pc < state.numMemory + 1) {
        // GONNNA ADD LOAD
        printState(&state);
        // extract opcode
        int decimal = state.mem[state.pc] >> 22;
        opcode = decimal & 7;
        // extract regA
        decimal = state.mem[state.pc] >> 19;
        regA = decimal & 7;
        // extract regB
        decimal = state.mem[state.pc] >> 16;
        regB = decimal & 7;
        // extraxt dest for nor and add, not used anywhere else.
        dest_reg =  state.mem[state.pc] & 7;
        // opcode == add
        if (opcode == 0){ // what if something like add 1 2 five  is this possible??????????
            state.reg[dest_reg] = state.reg[regA] + state.reg[regB];  // get a content ????????
            state.pc++;
        }
        // opcode == nor
        if (opcode == 1){
            state.reg[dest_reg] = ~(state.reg[regA] | state.reg[regB]);
            state.pc++;
        }
        // opcode == lw
        if ( opcode == 2){
            offset = convertNum(state.mem[state.pc] & 0xffff); // keeps only last 16 bits of decimal
            state.reg[regB] = state.mem[state.reg[regA] + offset];
            state.pc++;
        }
        // opcode = sw only time modify memory
        if ( opcode == 3){
            offset = convertNum(state.mem[state.pc] & 0xffff); // keeps only last 16 bits of decimal
            state.mem[offset + state.reg[regA]] = state.reg[regB]; // at memory location offset + content of A =  content of B
            state.pc++;
        }
        // opcode == beq
        if (opcode == 4) {
            offset = convertNum(state.mem[state.pc] & 0xffff); // keeps only last 16 bits of decimal
            if( state.reg[regA] == state.reg[regB]){
                state.pc = state.pc + offset + 1;
            }
            else {
                state.pc++;
            }
        }
        // if opcode == jalr
        if (opcode == 5){
            state.reg[regB] = state.pc + 1;
            state.pc = state.reg[regA];
        }
        // should there be an if statment here bc halt and noop have nothing or is it fine bc theyll just be zeros??
       // if (opcode == 6){ } // nothing bc noop does nothing, just increment pc ??????????????
        // opcode == halt;
        if(opcode == 6){
            j++;
            state.pc++;
            break;
        }
        // opcode == noop
        if (opcode == 7){ state.pc++; }
        j++;
    }
    printf("machine halted\n");
    printf("total of %d instructions executed\n" , j);
    printf("final state of machine:\n");
    printState(&state);

    return(0);
}

void
printState(stateType *statePtr)
{
    int i;
    printf("\n@@@\nstate:\n");
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

int
convertNum(int num)
{
    /* convert a 16-bit number into a 32-bit Linux integer */
    if (num & (1<<15) ) {
        num -= (1<<16);
    }
    return(num);
}


