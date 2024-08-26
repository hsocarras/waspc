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
#include "config.h"
#include "runtime/runtime.h"
#include "diagnostic/error.h"
#include "loader/wasm_loader.h"
#include "memory/work_memory.h"
#include "utils/hash_table.h"

////For TODO testing//////////////////////////////////////////////////////////////////////
#include "webassembly/binary/module.h"
#include "utils/hash_table.h"
//////////////////////////////////////////////////////////////////////////////////////////
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
static uint8_t work_code_mem[CODE_MEMORY_SIZE];         //See tia portal code memory
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
    printf("Booting \n");
    InitRuntime(&env);
    env.code_mem = work_code_mem;
    //SetWorkMem(&env, code_mem, CODE_MEMORY_SIZE);
    
    errno_t error_code; //Declaring a variable of type errno_t to store the return value of fopen_s
    FILE *wasm;
    int32_t len;
    uint8_t *load_ptr = work_code_mem; //Declaring a variable to store the wasm file
    uint8_t *load_base = work_code_mem; //Declaring a variable to store the wasm file
    uint32_t bytes_read;
    uint8_t *binary_file;
    //char pou_names[1024];
    //uint16_t pou_name_index[1024];
    //uint32_t pou_index_count = 0;
    char pou_name[32];
    uint32_t id;
    uint32_t pou_name_len;
   
    //uint8_t error_buf [16];

    

    WpError result = CreateError(OK, HOST, 88, 0);            //No error

    // Opening file in reading mode
    printf("Openning wasm file \n");
    error_code = fopen_s(&wasm, argv[1], "rb");

    if (error_code != 0) {
        printf("file can't be opened \n");
        return 1;
    }
    printf("Done \n");
    //Get pou's name logic ///////////////////////////////////////////////////////////////////////////////////
    GetFileName(argv[1], pou_name); //get name
    len = strlen(pou_name);
    id = fnv(pou_name, len);
    printf("File name %s has a id %u\n", pou_name, id);
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
    bytes_read = fread(load_ptr, 1, len, wasm);
    fclose(wasm);

    if(bytes_read == len){

        printf("Module Loaded. Total bytes %d \n", bytes_read);   

        //Loading process  
        //call to runtime/loader.c wasm_runtime_load
        
        result = LoadWasmBuffer(&env, load_ptr, len, id);
        
        if(result.id == OK){
            
            WasmBinModule *mod = (WasmBinModule *)HashTableGet(&env.table_wasm_bin, id);
            printf("Module loaded into runtime. %i Bytes loaded \n", (mod->bin_len));
            printf("Module code section size %d", mod->codesec.size);           
        }
        else{
            ReportError(&result);
            printf("Module loaded fail  \n");
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

/**
 * @brief Hook function for diagnostic
 * 
 * @param err 
 */
void ReportError(WpError *err){

    switch(err->detail.origin){
        case HOST:
            printf("Error from host module: \n");
            break;
        case LOADER:
            printf("Error from loader module: \n");
            break;
        case DECODER:
            printf("Error from decoder module: \n");
            break;
        default: 
            printf("Error unknow \n");
    }

    //get function name, for now line of code
    printf("At function; %u \n", err->detail.func);

    //get function place, for now line of code
    printf("in line; %u \n", err->detail.place);
    
}