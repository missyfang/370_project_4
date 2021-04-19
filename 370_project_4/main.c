#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>

#define NUMMEMORY 65536 /* maximum number of words in memory */
#define NUMREGS 8 /* number of machine registers */
#define MAXLINELENGTH 1000
#define MAX_CACHE_SIZE 256
#define MAX_BLOCK_SIZE 256


// read in command line

/*
 initialize cache with command line
 set LRU in cache to 0.
*/

/*
 break down addresss
 for fetch addr = PC
 for lw/sw addr = reg A + offset
 
calc # bits for tag, set, offset based on cache struct vars
 # offset bits = log blocksize
 # set index bits = log # sts
 # tag bits = addr - set bits - offset bits // how big is the addr????/
extract these bits from address]
 
 address & log blocksize
 address shift by offset bits
        * log # sets
 addres shift by # set bits
        &  addr - set bits - offset bits
can only be 1,2,4,8
 */

//LOAD
//dont need to do any thing for executing load bc done in P1????? just need to go thru cache???????
// prolly a call to getAddr
/*
 getAddr // func call
 check if in cache block array
 int i = setIndex * blocksPerSet
 while (i < setIndex + 1 * blocksPerSet){
    if (blocks[i].tag = addr tag && valid = false){ // was told to add valid = false but not sure that is correct since the data ought already be in the cache i.e valid = true
        // update LRU
        return blocks.[i].data[ addr offset];
 }
 }
 // MISS
 //initalize block
 i = setIndex * blocksPerSet;
 // check if any empty spots
 while ( i < setIndex + 1 * blocksPerSet){
    if (blocks[i].valid == 0){
        initalize blocks[i] ;
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
      intialize  blocks[i];
        // update LRU
        return blocks.[i].data[addr offset];
 }
 }
*/

// combine return spec block
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
 modify block data at block offset = regB
 change dirty bit to 1   // only SW can make a block dirty
 update LRU
 

 // MISS
 initalized block to addres regA + offset
 // avaiavble way
 check if any valid bits = 0
 bring from memory to cache i.e initialize block
 modify  // data = regB
 update dirty bit
 up date LRU
 
 // MISS & EVICT
 find LRU, write back, evict
 bring from memory to cache
 modify  // data = regB
 update dirty bit
 up dateLRU

*/

// prolly have a write back function
// ought have func for loops return which spec block then initalze/return data for/from the spec block in load and store funcs
// each case sep func that loops thru and returns spec block
    // could have func loops to see if hit
    // could have func that loops to find any valid bit = 0
    // could have func to find LRU
// could have a initalize block func

/*
// OH Qs
when intalized block need to bring in entire block from ememory so mem[i] to mem[i + blocksize - 1]?
how to WRITE back // need to write entire block to MEM mem[i] = sw stuff
inserting print action
still confused aboutintializing block[i].data[block offset] // where is the data found in memory ??? just set to regB???

 address in 32B?
 */

// even tho cache global wouldnt carry changes from init back???????
// check write back addr imp
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
  bool isDirty;
  int lruLabel;
  int set;
  int tag;
  int valid;
    // want to add valid bit
} blockStruct;

typedef struct cacheStruct
{
  blockStruct blocks[MAX_CACHE_SIZE]; // important for LRU implementation need to looop thru LRU block[].lrulabel = 0 for all in cache
  int blocksPerSet;
  int blockSize;
  int lru; // what iis this used for???????
  int numSets;
} cacheStruct;



// proj 1 state struct
typedef struct stateStruct {
    int pc;
    int mem[NUMMEMORY];
    int reg[NUMREGS];
    int numMemory;
} stateType;

/* Global Cache variable */
cacheStruct cache;
struct stateStruct state;

void printState(stateType *);
int convertNum(int);
void printAction(int address, int size, enum actionType type);
void printCache();
int getOffset(int addr, cacheStruct* cache);
int getSetIndex(int addr, cacheStruct* cache);
int getTag(int addr, int setBits, int offsetBits, cacheStruct* cache);
void updateLRU(cacheStruct* cache, int target, int set);
int search_cache_set( int start, int end, int tag, cacheStruct* cache);
int check_valid (int start, int end, cacheStruct* cache);
int evict (int start, int end, cacheStruct* cache, struct stateStruct* state);
int write_back_addr( cacheStruct* cache, struct stateStruct* state, int tag, int set, int offset);
void write_back( cacheStruct* cache, struct stateStruct* state, int addr, int block);
void init_block( cacheStruct* cache, struct stateStruct* state, int addr, int tag, int set, int offset, int blockIndex);

int loadFromMem(int addr, cacheStruct* cache, struct stateStruct * state); // Properly simulates the cache for a load from
                    // memory address “addr”. Returns the loaded value.

void storeToMem(int addr, int data, cacheStruct* cache, struct stateStruct* state); // Properly simulates the cache for a store
                                // to memory address “addr”. Returns nothing.

int
main(int argc, char *argv[])
{
    char line[MAXLINELENGTH];
    FILE *filePtr;

    if (argc != 5) {
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
    
    // need to initalize cache with command line
    cache.blockSize = atoi(argv[2]);
    cache.numSets = atoi(argv[3]);
    cache.blocksPerSet = atoi(argv[4]);

    // initalizes all LRU to be 0
    for(int i = 0 ; i< MAX_CACHE_SIZE; i++){         // do i need to initialize other stuff to 0????????/
        cache.blocks[i].lruLabel = 0;
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
        //int decimal = state.mem[state.pc] >> 22;
        int decimal = loadFromMem(state.pc, &cache, &state) >> 22;
        opcode = decimal & 7;
        // extract regA
        decimal = loadFromMem(state.pc, &cache, &state) >> 19;
        regA = decimal & 7;
        // extract regB
        decimal = loadFromMem(state.pc, &cache, &state) >> 16;
        regB = decimal & 7;
        // extraxt dest for nor and add, not used anywhere else.
        dest_reg =  loadFromMem(state.pc, &cache, &state) & 7;
        // opcode == add
        if (opcode == 0){
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
           // offset = convertNum(state.mem[state.pc] & 0xffff); // keeps only last 16 bits of decimal
            //state.reg[regB] = state.mem[state.reg[regA] + offset]; // = load[state.reg[regA] + offset}
            offset = convertNum(loadFromMem(state.pc, &cache, &state) & 0xffff);
            int trash = state.reg[regA] + offset;
            state.reg[regB] = loadFromMem(trash, &cache, &state);
            state.pc++;
        }
        // opcode = sw only time modify memory
        if ( opcode == 3){
           // offset = convertNum(state.mem[state.pc] & 0xffff); // keeps only last 16 bits of decimal
            // state.mem[offset + state.reg[regA]] = state.reg[regB]; // at memory location offset + content of A =  content of B
            offset = convertNum(loadFromMem(state.pc, &cache, &state) & 0xffff);
            int shit = offset + state.reg[regA];
            int data = state.reg[regB];
            storeToMem(shit, data,&cache, &state);
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
// initalize cach

// return valur of addr offset
int getOffset(int addr, cacheStruct* cache){
    int bits = log2(cache->blockSize);
    int offset =  addr & bits;                      // mask
    return offset;
}
// return value of addr set index
int getSetIndex(int addr, cacheStruct* cache){
    int shift = log2(cache->blockSize);              // shift by # offset bits
    int bits = log2(cache->numSets);
    int temp = addr >> shift;
    int index = (temp & bits);             // mask and shift
    return index;
}
// return value of addr tag
int getTag(int addr, int setBits, int offsetBits, cacheStruct* cache){
    int shift_set = log2(cache->numSets);
    int shift_off = log2(cache->blockSize);
    int shift = shift_off + shift_set;
    int tag = addr >> shift;             // mask and shift by # set bits
    return tag;
}
// load function
int loadFromMem (int addr, cacheStruct* cache, struct stateStruct* state){
    int offset = getOffset(addr, cache);
    int setIndex = getSetIndex(addr, cache);
    int tag = getTag(addr, setIndex , offset, cache);
    
    // check for cache hit
    int setStart = setIndex * cache->blocksPerSet; // index into set
    int setEnd = (setIndex + 1) * cache->blocksPerSet;  // index of start of next set
    int hit = search_cache_set(setStart, setEnd, tag, cache);
    if (hit != -1){                                     // HIT
        init_block(cache, state, addr, tag, setIndex, offset, hit);    // not sure if should be passing *state?????????
        // call LRU here or in init func?????????????
        return cache->blocks[hit].data[offset];  }     // return data found in cache
        
    // MISS
    int emptySlot = check_valid(setStart, setEnd, cache);
    if (emptySlot != -1){
        init_block(cache, state, addr, tag, setIndex, offset, emptySlot);
        // call LRU here or in init func?????????????
        return cache->blocks[emptySlot].data[offset]; } // return data put into empty slot in cache
    
    // EVICT
    int evictionBlock = evict(setStart, setEnd, cache, state);
    if (cache->blocks[evictionBlock].isDirty == true){              // check if dirty
        int writeBackAddr = write_back_addr(cache, state, tag, setIndex, offset);
        write_back(cache, state, writeBackAddr, evictionBlock); }
    init_block(cache, state, addr, tag, setIndex, offset, evictionBlock);
    return cache->blocks[evictionBlock].data[offset];
}
void storeToMem( int addr, int data, cacheStruct* cache, struct stateStruct* state){
    int offset = getOffset(addr, cache);
    int setIndex = getSetIndex(addr, cache);
    int tag = getTag(addr, setIndex , offset, cache);
    int setStart = setIndex * cache->blocksPerSet; // index into set
    int setEnd = (setIndex + 1) * cache->blocksPerSet;  // index of start of next set
    int hit = search_cache_set(setStart, setEnd, tag, cache);
    // HIT
    if (hit != -1){
        cache->blocks[hit].data[offset] = data;
        return; }
    // MISS
    int emptySlot = check_valid(setStart, setEnd, cache);
    if (emptySlot != -1){
        init_block(cache, state, addr, tag, setIndex, offset, emptySlot);    // bring in block
        cache->blocks[emptySlot].data[offset] = data;                        // write to cache block
        return; }
    // EVICT
    int evictionBlock = evict(setStart, setEnd, cache, state);
    if (cache->blocks[evictionBlock].isDirty == true){
        int writeBackAddr = write_back_addr(cache, state, tag, setIndex, offset);
        write_back(cache, state, writeBackAddr, evictionBlock); }           // write back to memory
    init_block(cache, state, addr, tag, setIndex, offset, evictionBlock);   // bring in cache block from memory
    cache->blocks[evictionBlock].data[offset] = data;                       // write to cache block
}

// update LRU function
void updateLRU(cacheStruct* cache, int target, int set){
    int i = cache->blockSize * set;                 // index of first bloack in set
    int end =  cache->blockSize * (set + 1);        // index of start of next set
    while (i < end){
        if(target != i){                           // skip target block
            if (cache->blocks[i].lruLabel > cache->blocks[target].lruLabel){  // if LRU greater than target OG LRU
                cache->blocks[i].lruLabel--;        // decrement LRU
            }
        }
        i++;                                       // go to next block
    }
    cache->blocks[target].lruLabel = cache->blockSize - 1;     // sent MRU to greatest count
}

// search set for hit, return  index of block that hit, return -1 if miss
int search_cache_set (int start, int end, int tag, cacheStruct* cache){
    int i = start;
    while (i < end){
        if (cache->blocks[i].tag == tag && cache->blocks[i].valid == 1 ){   // is the valid thing correct ??????????
            return i;  }                            // returns index of block where hit
        i++; }
    return -1;
}
// check to see if entire set is full
int check_valid (int start, int end, cacheStruct* cache){
    int i = start;
    while (i < end){
        if (cache->blocks[i].valid == 0){
            return i; }
        i++; }
    return -1;
}
// finds which block to evict from set, check_valid called first so set should be full
// if block is dirty will write back before block is reinitalized
int evict (int start, int end, cacheStruct* cache, struct stateStruct* state){
    int i = start;
    while (i < end){
        if( cache->blocks[i].lruLabel == 0) {
            return i; }
        i++; }
    return -1;
}
// returns mem addr to write back to from cache
int write_back_addr( cacheStruct* cache, struct stateStruct* state, int tag, int set, int offset){
    int offsetBits = log2(cache->blockSize);
    int setBits = log2(cache->numSets);
    int tagShift = tag << (offsetBits + setBits);
    int setShift = set << offsetBits;
    int addr = tagShift | setShift | offset;   // I think this works?????????
    return addr;
}
// writes back data from eviction block to correct memory address
void write_back( cacheStruct* cache, struct stateStruct* state, int addr, int block){
    for( int i = 0 ; i< cache->blockSize ; i++){
        state->mem[addr] = cache->blocks[block].data[i];
        addr++;}
}
void init_block( cacheStruct* cache, struct stateStruct* state, int addr, int tag, int set, int offset, int blockIndex){
    int j = addr - offset;              // start of indecies within block I think it works???????????????
    cache->blocks[blockIndex].valid = 1;
    cache->blocks[blockIndex].tag = tag;
    cache->blocks[blockIndex].set = set;
    for ( int i = 0; j < cache->blockSize; i++){
    cache->blocks[blockIndex].data[i] = state->mem[j];
    j++; }
}




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


