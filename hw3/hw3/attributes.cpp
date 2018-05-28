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
            return INT;
        case BYTE_ARR:
            return BYTE;
        case BOOL_ARR:
            return BOOL;
        default:
            break;
    }
    return ERROR;
}

TypeId convertToArrType(TypeId arr_t) {
    switch (arr_t) {
        case INT:
            return INT_ARR;
        case BYTE:
            return BYTE_ARR;
        case BOOL:
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
