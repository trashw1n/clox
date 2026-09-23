#include "common.h"
#include "chunk.h"
#include "debug.h"
#include "vm.h"

int main(int argc, const char* argv[]){
    initVM();
    Chunk chunk;
    initChunk(&chunk);
    int c1 = addConstant(&chunk, 1);
    writeChunk(&chunk, OP_CONSTANT, 123);
    writeChunk(&chunk, c1, 123);
    int c2 = addConstant(&chunk, 5.6);
    writeChunk(&chunk, OP_CONSTANT, 124);
    writeChunk(&chunk, c2, 124);    
    writeChunk(&chunk, OP_RETURN, 125);
    writeChunk(&chunk, OP_RETURN, 125);
    writeChunk(&chunk, OP_RETURN, 125);
    writeChunk(&chunk, OP_RETURN, 125);
    disassembleChunk(&chunk, "test chunk");
    interpret(&chunk);
    freeVM();
    freeChunk(&chunk);
    return 0;
}