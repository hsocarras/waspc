/**
 * @file driver.c
 * @author H. E. Socarras
 * @brief This file implement windows driver. For now to load and prepare all thing for wasm interpreter
 * @version 0.0.1
 * @date 2024-03-17
 *
 * @copyright copyright description
 */

//wasp includes
#include "vm.h"
#include "diagnostic/error.h"
#include "loader/wasm_loader.h"


#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <assert.h>


 
int main(int argc, const char* argv[]) {

    assert(argc > 1);
    
    
    errno_t error_code; //Declaring a variable of type errno_t to store the return value of fopen_s
    FILE *wasm;
    int32_t len;
    uint8_t *binary_file; //Declaring a variable to store the wasm file
    uint32_t bytes_read;
   
    //uint8_t error_buf [16];

    //instanciating the interpreter;
    Vm interpreter;
    WpError result = {OK};            //No error

    // Opening file in reading mode
    error_code = fopen_s(&wasm, argv[1], "rb");

    if (error_code != 0) {
        printf("file can't be opened \n");
        return 1;
    }

    // Positioning cursor at file end
    fseek( wasm , 0L , SEEK_END);
    // Getting file's size
    len = ftell(wasm);
    // Positioning cursor at begining
    rewind(wasm);

    // getting memory to store binary file
    binary_file = malloc(len);

    if (!binary_file){
        printf("No memory available to load wasm\n");
        return 2;
    } 

    //fread(binary_file, len, 1, wasm);
    bytes_read = fread(binary_file, 1, len, wasm);
    fclose(wasm);

    if(bytes_read == len){
        printf("Module Loaded. Total bytes %d \n", bytes_read);   

        //Loading process  
        //call to runtime/loader.c wasm_runtime_load
        result = LoadWasm(&interpreter, binary_file, len);
        if(result.id == 0){
            printf("Module loaded into interpreter. wasm version %i \n",interpreter.main_module.package_version);
        }
        else{
            printf("Module loaded fail %i \n",result.id);
        }
        free(binary_file);
        return 0;
    }
    else{
        //_fcloseall();
        printf("Error loading file content\n");
        free(binary_file);
        return 3;

    }  
    
}