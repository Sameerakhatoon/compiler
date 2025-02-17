#include <stdio.h>
#include "compiler.h"

int main(int argc, char** argv){
    printf("entering main\n");
    int result = compile_file("./test/testMain.c", "./test/testMain", 0);
    if(result==COMPILER_SUCCESS){
        printf("Compilation successful\n");
    }
    else if(result==COMPILER_FAILED_WITH_ERRORS){
        printf("Compilation failed with errors\n");
    }
    else{
        printf("unknown response for compile file\n");
    }
    return 0;
}