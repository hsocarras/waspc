/**
 * @file wasmbinmodule.c
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2024-09-16
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#include "object/bin_module.h"


void WpBinModuleInit(WpBinModule *self){

    self->type = WP_OBJECT_BIN_MODULE;
    self->version = 0;

    self->typesec.size = 0;
    self->typesec.content = NULL;

    self->importsec.size = 0;
    self->importsec.content = NULL;

    self->functionsec.size = 0;
    self->functionsec.content = NULL;

    self->tablesec.size = 0;
    self->tablesec.content = NULL;

    self->memsec.size = 0;
    self->memsec.content = NULL;

    self->globalsec.size = 0;
    self->globalsec.content = NULL;

    self->exportsec.size = 0;
    self->exportsec.content = NULL;

    self->startsec.size = 0;
    self->startsec.content = NULL;

    self->elemsec.size = 0;
    self->elemsec.content = NULL;

    self->datacountsec.size = 0;
    self->datacountsec.content = NULL;

    self->codesec.size = 0;
    self->codesec.content = NULL;

    self->datasec.size = 0;
    self->datasec.content = NULL;
}