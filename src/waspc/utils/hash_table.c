
#include "utils/hash_table.h"

void InitHashTable(HashTable *self){

    self->capacity = 0;
    self->usage = 0;
    self->items = NULL;
}

/*
void FreeHashTable(HashTable *self){

    self->capacity = 0;
    self->usage = 0;
    self->items = NULL;
}*//