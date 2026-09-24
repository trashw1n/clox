#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include "common.h"
#include "chunk.h"
#include "debug.h"
#include "vm.h"

static void repl(){
    char line[1024];
    for(;;){
        printf("> ");
        if(!fgets(line, sizeof(line), stdin)){
            printf("\n");
            break;
        }
        interpret(line);
    }
}

static char* readFile(const char* path){
    FILE* file = fopen(path, "rb");
    //file does not exist
    if(file == NULL){
        fprintf(stderr, "Could not open file \"%s\".\n", path);
        exit(74);
    }
    //move to the end of file and calculate size of the file using bytes moved.
    fseek(file, 0L, SEEK_END);
    size_t fileSize = ftell(file);
    rewind(file);
    char* buf = (char*)malloc(fileSize + 1);
    //not able to allocate enough memory 
    if(buf == NULL){
        fprintf(stderr, "Not enough memory to read \"%s\".\n", path);
        exit(74);        
    }
    size_t bytesRead = fread(buf, sizeof(char), fileSize, file);
    buf[bytesRead] = '\0';
    fclose(file);
    return buf;
}

static void runFile(const char* path){
    char* src = readFile(path);
    InterpretResult result = interpret(src);
    free(src);
    if(result == INTERPRET_COMPILE_ERROR) exit(65);
    if(result == INTERPRET_RUNTIME_ERROR) exit(70);
}

int main(int argc, const char* argv[]){
    initVM();
    if(argc == 1) repl();
    else if(argc == 2) runFile(argv[1]);
    else{
        fprintf(stderr, "Usage: clox [path]\n");
        exit(64);
    }
    freeVM();
    return 0;
}