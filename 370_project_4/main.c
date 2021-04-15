#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#define NUMMEMORY 65536 /* maximum number of words in memory */
#define NUMREGS 8 /* number of machine registers */
#define MAXLINELENGTH 1000
#define MAX_CACHE_SIZE 256
#define MAX_BLOCK_SIZE 256


// read in command line

/*
 initialize cache with command line
  How to handel LRU? is it the total count????
  evict if LRU = count = # blocks per set ?????
 need to initalize block array to correct size??????
*/

/*
 break down addresss
 for fetch addr = PC
 for lw/sw addr = reg A = offset
 
calc # bits for tag, set, offset based on cache struct vars
 # offset bits = log blocksize
 # set index bits = log # sts
 # tag bits = addr - set bits - offset bits // how big is the addr????/
extract these bits from address
can only be 1,2,4,8
 if(off = 1) & 1
 if(off = 2) & 3
 if(off = 4) & 7
 if(off = 8) & 15
 // not sure of should shift or just use diff mask numers for tag and set bits  ?????????
 */

//LOAD
//dont need to do any thing for executing load bc done in P1????? just need to go thru cache???????
// prolly a call to getAddr
/*
 getAddr // func call
 check if in cache block array
 int i = setIndex * blocksPerSet
 while (i < setIndex + 1 * blocksPerSet){
    if (blocks[i].tag = addr tag ){ // HIT
        // update LRU
        return blocks.[i].data[ addr offset];
 }
 }
 // MISS
 //create and initalize new block
 i = setIndex * blocksPerSet;
 // check if any empty spots
 while ( i < setIndex + 1 * blocksPerSet){
    if (blocks[i].valid == 0){
        blocks[i] = new block;
        // update LRU
        return blocks.[i].data[addr offset];
 }
 }
 // need to evict LRU
 i = setIndex * blocksPerSet;
 while( i < setIndex + 1 * blocksPerSet){
    if (block.[i].LRU == 0){  // is LRU
        if (blocks[i].isDirty == true){
            //write back
        }
        blocks[i] = new block;
        // update LRU
        return blocks.[i].data[addr offset];
 }
 }
*/

// SET LRU
/*
// all LRU orginally set to 0???? , need to be for my implementaiton to work
 setLRU(block[i], target offset){
    j = 0;
    while (j < blockSize){ // or MAX_BLOCK_SIZE
        if (block[i].data[j].LRU > block[i].data[target].LRU){   // og LRU of target addr
            block[i].data[j].LRU--
        }
    }
 block[i].data[target].LRU = blockSize - 1;   // Set target LRU to MRU value
 }
 */

// STORE
// best way to combine LOAD and STORE funcs ???????
/*
 getAddr
 
 check if in cache
 // HIT
 modify block data at offset = regB   // is regB also need to be read into cache or is it just gonna be the actual number/line ?????????
 change dirty bit to 1   // only SW can make a block dirty??????
 update LRU
 

 // MISS
 creat block initalized to addres regA + offset
 // avaiavble way
 check if any valid bits = 0
 bring from memory to cache
 modify  // data = regB???????
 update dirty bit
 up date LRU
 
 // MISS & EVICT
 find LRU, write back, evict
 check if any valid bits = 0
 bring from memory to cache
 modify  // data = regB
 update dirty bit
 up dateLRU

*/


/*
// OH Qs
How to Implement WRITE BACK
how to CREATE BLOCK
    take addr from memory and create block
how to WRITE TO MEM
comand line
what is enum action
when to call print
using CAEN
 */



// proj 4 structs
enum actionType
{
  cacheToProcessor,
  processorToCache,
  memoryToCache,
  cacheToMemory,
  cacheToNowhere
};

typedef struct blockStruct
{
  int data[MAX_BLOCK_SIZE];
    // number ways in block
  bool isDirty;
  int lruLabel;
  int set;
  int tag;
    // want to add valid bit
} blockStruct;

typedef struct cacheStruct
{
  blockStruct blocks[MAX_CACHE_SIZE];
    // numer total blocks
  int blocksPerSet;
  int blockSize;
  int lru; // what iis this used for
  int numSets;
} cacheStruct;

/* Global Cache variable */
cacheStruct cache;

// proj 1 state struct
typedef struct stateStruct {
    int pc;
    int mem[NUMMEMORY];
    int reg[NUMREGS];
    int numMemory;
} stateType;

void printState(stateType *);
int convertNum(int);
void printAction(int address, int size, enum actionType type);
void printCache();
int loadFromMem(int addr); // Properly simulates the cache for a load from
                    // memory address “addr”. Returns the loaded value.

void storeToMem(int addr, int data); // Properly simulates the cache for a store
                                // to memory address “addr”. Returns nothing.


int
main(int argc, char *argv[])
{
    char line[MAXLINELENGTH];
    stateType state;
    FILE *filePtr;

    if (argc != 2) {   // change to 4 or 5 ??????????
        printf("error: usage: %s <machine-code file>\n", argv[0]);
        exit(1);
    }
    
    // need to initalize cache with command line  IM Not sure if this is correct at all?????
    cache.blockSize = atoi(argv[2]);
    cache.numSets = atoi(argv[3]);
    cache.blocksPerSet = atoi(argv[4]);

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

// PROJ 4 HELPER FUNCS

/*
 * Log the specifics of each cache action.
 *
 * address is the starting word address of the range of data being transferred.
 * size is the size of the range of data being transferred.
 * type specifies the source and destination of the data being transferred.
 *  -    cacheToProcessor: reading data from the cache to the processor
 *  -    processorToCache: writing data from the processor to the cache
 *  -    memoryToCache: reading data from the memory to the cache
 *  -    cacheToMemory: evicting cache data and writing it to the memory
 *  -    cacheToNowhere: evicting cache data and throwing it away
 */
void printAction(int address, int size, enum actionType type)
{
    printf("@@@ transferring word [%d-%d] ", address, address + size - 1);

    if (type == cacheToProcessor) {
        printf("from the cache to the processor\n");
    }
    else if (type == processorToCache) {
        printf("from the processor to the cache\n");
    }
    else if (type == memoryToCache) {
        printf("from the memory to the cache\n");
    }
    else if (type == cacheToMemory) {
        printf("from the cache to the memory\n");
    }
    else if (type == cacheToNowhere) {
        printf("from the cache to nowhere\n");
    }
}

/*
 * Prints the cache based on the configurations of the struct
 */
void printCache()
{
  printf("\n@@@\ncache:\n");

  for (int set = 0; set < cache.numSets; ++set) {
    printf("\tset %i:\n", set);
    for (int block = 0; block < cache.blocksPerSet; ++block) {
      printf("\t\t[ %i ]: {", block);
      for (int index = 0; index < cache.blockSize; ++index) {
        printf(" %i", cache.blocks[set * cache.blocksPerSet + block].data[index]);
      }
      printf(" }\n");
    }
  }

  printf("end cache\n");
}


// PROJ 1S HELPER FUNCS


void printState(stateType *statePtr)
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

int convertNum(int num)
{
    /* convert a 16-bit number into a 32-bit Linux integer */
    if (num & (1<<15) ) {
        num -= (1<<16);
    }
    return(num);
}


