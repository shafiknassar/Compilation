//
//  attributes.cpp
//  hw3
//
//  Created by Shafik Nassar on 23/05/2018.
//  Copyright © 2018 ShafikNassar. All rights reserved.
//

#include "attributes.hpp"

TypeId convertFromArrType(TypeId arr_t) {
    switch (arr_t) {
        case INT_ARR:
            return M_INT;
        case BYTE_ARR:
            return M_BYTE;
        case BOOL_ARR:
            return M_BOOL;
        default:
            break;
    }
    return ERROR;
}

TypeId convertToArrType(TypeId arr_t) {
    switch (arr_t) {
        case M_INT:
            return INT_ARR;
        case M_BYTE:
            return BYTE_ARR;
        case M_BOOL:
            return BOOL_ARR;
        default:
            break;
    }
    return ERROR;
}

bool isAlreadyDefined(vector<Table> scopes, Id *id) {
    for (int i = (int)scopes.size()-1; i >= 0; --i) {
        if (scopes[i].isDefinedInScope(id)) return true;
    }
    return false;
}

FuncTableEntry* funcLookup(vector<Table> scopes, Id *id)
{
    FuncTableEntry *res = NULL;
    for (int i = (int)scopes.size()-1; i >= 0; --i) {
        res = scopes[i].getFuncEntry(id->id);
        if (NULL != res) return res;
    }
    return NULL;
}

TableEntry *idLookup(vector<Table> scopes, Id *id) {
    TableEntry *res = NULL;
    for (int i = (int)scopes.size()-1; i >= 0; --i) {
        res = scopes[i].getEntry(id->id);
        if (NULL != res) return res;
    }
    return NULL;
}

int typeSize(TypeId id)
{
    switch (id) {
        case M_INT:
            return 1;
        case M_VOID:
            return 0;
        case M_STRING: ;
        default: ;
    }
    return 1;
}

string etos(TypeId type)
{
    switch (type) {
        case M_INT: return string("INT");
        case M_VOID: return string("VOID");
        case M_BOOL: return string("BOOL");
        case M_STRING: return string("STRING");
        case M_BYTE: return string("BYTE");
        case INT_ARR: return string("IARR");
        case BYTE_ARR: return string("CARR");
        case BOOL_ARR: return string("BARR");
        case ERROR: return string("error");
            
            
            
        default: return string("ERROR");
    }
}

bool isArrType(TypeId t) {
    switch (t) {
        case INT_ARR:
        case BYTE_ARR:
        case BOOL_ARR:
            return true;
        default:
            break;
    }
    return false;
}
