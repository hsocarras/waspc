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
#include "runtime/runtime.h"
#include "diagnostic/error.h"
#include "loader/wasm_loader.h"
#include "memory/work_memory.h"


#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>

//config memory////////////////////////////////////////////////////////////////////////////////
#define INPUT_SIZE 256      //in bytes
#define OUTPUT_SIZE 256     //in bytes
#define MARK_SIZE 256
#define CODE_MEMORY_SIZE 65536  //64 KB


static uint8_t in[INPUT_SIZE];
static uint8_t out[OUTPUT_SIZE];
static uint8_t mark[MARK_SIZE];

static uint8_t code_mem[CODE_MEMORY_SIZE];
//////////////////////////////////////////////////////////////////////////////////////////////////

//instanciating the runtime;
static RuntimeEnv env;

/**
 * @brief Get the File Name object
 * Basic function to get the file name. Is not final version and must be inproved
 * @param path 
 * @return char* 
 */
uint32_t GetFileName(const char * path, char *name){

    
    const char *end;
    size_t len;
    size_t counter = 0;
    errno_t err;
    len = strlen(path);
    
    end = &path[len - 1];
    
    //directory
    if (*end == '/'){   
        strcpy_s(name, 1, "");    
        return 0;
    }

    while(*end != '/' && *end != 92 && counter < len){
        end--;
        counter++;
    }
    
    end ++;
    
    len = strlen(end);    
    err = strcpy_s(name, len+1, end);
    if (err != 0) {
        printf("Err %i", err);
        return 0;
    }
    
    return len;

}


 
int main(int argc, const char* argv[]) {

    assert(argc > 1);

    InitRuntime(&env);
    SetWorkMem(&env, code_mem, CODE_MEMORY_SIZE);
    
    errno_t error_code; //Declaring a variable of type errno_t to store the return value of fopen_s
    FILE *wasm;
    int32_t len;
    uint8_t *binary_file; //Declaring a variable to store the wasm file
    uint32_t bytes_read;
    //char pou_names[1024];
    //uint16_t pou_name_index[1024];
    //uint32_t pou_index_count = 0;
    char pou_name[32];
    uint32_t pou_name_len;
   
    //uint8_t error_buf [16];

    

    WpError result = CreateError(OK);            //No error

    // Opening file in reading mode
    error_code = fopen_s(&wasm, argv[1], "rb");

    if (error_code != 0) {
        printf("file can't be opened \n");
        return 1;
    }
    
    //Get pou's name logic ///////////////////////////////////////////////////////////////////////////////////
    GetFileName(argv[1], pou_name); //get name
    printf("File name %s \n", pou_name);
    //pou_name_len = strlen(pou_name);  // get name leng
    //pou_name_index[0] = 0; //index wher the string will be saved  
    //strcpy()
    
    /////////////////////////////////////////////////////////////////////////////////////////////////////////
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
        
        result = LoadWasmBuffer(&env, binary_file, len, 1);
        
        if(result.id == 0){
            
            printf("Module loaded into runtime. %i Bytes loaded \n", (env.work_mem.index - env.work_mem.code));
            printf("Module code section size %d", env.mod.codesec.size);           
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