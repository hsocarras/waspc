#include "interpreter/values.h"
#include <math.h>

StackValue SinF64(uint32_t argc,StackValue *args)
{
    StackValue result;
    StackValue x = args[0];
    if(x.type != WAS_VAL_TYPE_F64)
    {
        // Handle error for unsupported types
        result.type = WAS_EX_VAL_TYPE_NULL; // or some error code
        result.value.f64 = 0.0; // or some default value
        return result;
    }
    result.type = WAS_VAL_TYPE_F64;
    result.value.f64 = sin(x.value.f64);
        
    return result;
}

StackValue SinF32(uint32_t argc, StackValue *args)
{
    StackValue result;
    StackValue x = args[0];
    if(x.type != WAS_VAL_TYPE_F32)
    {
        // Handle error for unsupported types
        result.type = WAS_EX_VAL_TYPE_NULL; // or some error code
        result.value.f32 = 0.0f; // or some default value
        return result;
    }
    result.type = WAS_VAL_TYPE_F32;
    result.value.f32 = sinf(x.value.f32);
        
    return result;
}