
#ifndef AOS_VM_RUNTIME_H
#define AOS_VM_RUNTIME_H

#ifdef __cplusplus
    extern "C" {
#endif

typedef enum {
    MODULE,
    FUNCTION,
    TABLE,

}InstanceType;

typedef struct {    
    InstanceType type;
}Instance;

typedef struct {    
    Instance _instance;
}ModuleInstance;

typedef struct {    
    Instance _instance;
}FunctionInstance;

typedef struct {    
    Instance _instance;
}TableInstance;

typedef struct {    
    Instance _instance;
}MemoryInstance;

typedef struct {    
    Instance _instance;
}GlobalInstance;

typedef struct {    
    Instance _instance;
}ElementInstance;

typedef struct {    
    Instance _instance;
}DataInstance;

typedef struct {
    FunctionInstance *funcs;
    size_t funcs_len;
    TableInstance *tables;
    size_t tables_len;
    MemoryInstance *mems;
    size_t mems_len;
    GlobalInstance *globals;
    size_t globals_len;
    ElementInstance * elems;
    size_t elems_len;
    DataInstance *datas;
    size_t datas_len;

}Store;

#ifdef __cplusplus
    }
#endif

#endif
