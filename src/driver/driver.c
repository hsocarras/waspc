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
#include "objects/error.h"

////For TODO testing//////////////////////////////////////////////////////////////////////
#include "webassembly/binary/module.h"
#include "webassembly/structure/module.h"
#include "utils/hash_table.h"
#include "utils/names.h"
//////////////////////////////////////////////////////////////////////////////////////////

//Standars Includes
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>

//config memory////////////////////////////////////////////////////////////////////////////////
#define INPUT_SIZE 256      //in bytes
#define OUTPUT_SIZE 256     //in bytes
#define MARK_SIZE 256       //in bytes
#define CODE_MEMORY_SIZE 65536  //64 KB


static uint8_t in[INPUT_SIZE];
static uint8_t out[OUTPUT_SIZE];
static uint8_t mark[MARK_SIZE];
static uint8_t work_code_mem[CODE_MEMORY_SIZE];         //See tia portal code memory
// Initialize the hash table for modules
HtEntry table[10]; // Allocate memory for the hash table
//////////////////////////////////////////////////////////////////////////////////////////////////

void ReportError(WpError *err);  //function prototype for diagnostic

//instanciating the runtime;
//static RuntimeEnv env;

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

    printf("Starting \n");

    //Init Runtime State    
    WpRuntimeState runtime;    
    WpRuntimeInit(&runtime);
    WpRuntimeCodeMemInit(&runtime, work_code_mem, CODE_MEMORY_SIZE);    
    WpRuntimetableInit(&runtime, table, 10);  // Initialize the hash table with 10 capacity
    // Opening file in reading mode/////////////////////////////////////////////////////////////////////////
    FILE *wasm;   
    errno_t error_code;                             //Declaring a variable of type errno_t to store the return value of fopen_s
    printf("Openning wasm file \n");
    error_code = fopen_s(&wasm, argv[1], "rb");

    if (error_code != 0) {
        printf("file can't be opened \n");
        return 1;
    }
    printf("Wasm module readed \n");
        
    /////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Positioning cursor at file end
    fseek( wasm , 0L , SEEK_END);
    // Getting file's size
    int32_t len;
    len = ftell(wasm);
    // Positioning cursor at begining
    rewind(wasm);     

    //Reading file into runtime code memory
    uint8_t *load_ptr = (uint8_t *)malloc(len);              //Declaring a variable to store the wasm file    
    if (load_ptr == NULL) {
        printf("Memory allocation failed\n");
        fclose(wasm);
        return 1;
    }
    uint32_t bytes_read;
    bytes_read = fread(load_ptr, 1, len, wasm);
    Name mod_name;
    char name[6] = "main1";
    mod_name.name = name;
    mod_name.lenght = 6;
    // Read module from file into runtime memory ///////////////////////////////////////////////////////////////////////////////
    WpObject *result = WpRuntimeReadModule(&runtime, mod_name, load_ptr, len);
    fclose(wasm);
    free(load_ptr);
    
    // Check if the module was read successfully
    if(result->type == WP_OBJECT_ERROR){
        printf("Error loading file content\n");        
        WpError *err = (WpError *)result;        
        ReportError(err);
        return 3;
    }    
    assert(result->type == WP_OBJECT_MODULE_STATE);
    
    // Read ok, now validate the module //////////////////////////////////////////////////////////////////////////////////////
    WpModuleState *mod = (WpModuleState *)result;
    // validate the module    
    result = WpRuntimeValidateModule(&runtime, mod);
    if(result->type == WP_OBJECT_ERROR){
        printf("Error validating module\n");        
        WpError *err = (WpError *)result;        
        ReportError(err);
        return 3;
    }   
    assert(result->type == WP_OBJECT_MODULE_STATE);
    
    // Module valid.
    mod = (WpModuleState *)result;
    printf("Module name: %s\n", WasNameToString(mod->name));
    printf("Module status: %d\n", mod->status);
    printf("Module version: %d\n", mod->version);      
    printf("Module types count: %d\n", mod->was->types.lenght);   

    // Now we can instanciate the module////////////////////////////////////////////////////////////////////////////////
    printf("Instanciating module\n");
    // Create a module instance
    // Note: For now we are not using external values, so we pass NULL and 0
    result = WpRuntimeInstanciateModule(&runtime, mod, NULL, 0);
    if(result->type == WP_OBJECT_ERROR){
                printf("Error instanciating module\n");        
                WpError *err = (WpError *)result;        
                ReportError(err);
                return 3;
    }
    assert(result->type == WP_OBJECT_MODULE_INSTANCE);

    // Module instance created, now we can invoke the module   ////////////////////////////////////////////////////////// 
    WpModuleInstance *instance = (WpModuleInstance *)result;

    printf("Module instance created\n");
    result = WpRuntimeInvocateProgram(&runtime, instance);                
    if(result->type == WP_OBJECT_ERROR){
        printf("Error invoking module\n");        
        WpError *err = (WpError *)result;        
        ReportError(err);
        return 3;
    }
                
    return 0;
     
    
}

/**
 * @brief Hook function for diagnostic
 * 
 * @param err 
 */
void ReportError(WpError *err){

    printf("Error id: %u \n", err->id);
    #if WASPC_CONFIG_DEV_FLAG == 1
    //get file name, for now line of code
    printf("At file: %s ;\n", err->file); 
    printf("At function: %s ;\n", err->func);
    #endif      
}