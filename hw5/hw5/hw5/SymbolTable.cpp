//
//  SymbolTable.cpp
//  hw5
//
//  Created by Bilal Tamish on 19/06/2018.
//  Copyright © 2018 ShafikNassar. All rights reserved.
//

#include "includes.h"

vector<string>* FuncTableEntry::getArgs() {
    vector<string> *args = new vector<string>();
    for (int i = (int)paramTypes.size()-1; i >= 0; i--) {
        string typeStr = etos(paramTypes.at(i)->type);
        if (isArrType(paramTypes.at(i)->type)) {
            TypeId type = convertFromArrType(paramTypes.at(i)->type);
            typeStr = output::makeArrayType(etos(type), paramTypes.at(i)->size);
        }
        args->push_back(typeStr);
    }
    return args;
}

/***************************************/
/* Main Table */
/***************************************/

void Table::insert(string name, TypeId type, int offset) {
    entryStack.push_back(new TableEntry(name, type, offset));
}

void Table::insert(Variable *id, TypeId type, int offset) {
    if (isArrType(type)) {
        insertArr(id, offset, id->size);
        return;
    }
    insert(id->id, type, offset);
}

void Table::insertArr(Variable *id, int offset, int size) {
    ArrTableEntry *tmp = new ArrTableEntry(id->id, id->type, offset, size);
    entryStack.push_back(tmp);
}

void Table::insertFunc(string name, TypeId retType, vector<Variable*> &paramTypes) {
    FuncTableEntry *tmp = new FuncTableEntry();
    tmp->name       = name;
    tmp->type       = FUNC;
    tmp->offset     = 0;
    tmp->retType    = retType;
    /*
     The following parameter can be
     most easily acquired from a struct
     specificaly created to FormalDecls
     */
    for (int i = 0; i < paramTypes.size(); ++i) {
        tmp->paramTypes.push_back(new Type(paramTypes[i]->type, paramTypes[i]->size));
    }
    entryStack.push_back(tmp);
}

FuncTableEntry* Table::getFuncEntry(string name) {
    for (int i = 0; i < entryStack.size(); ++i) {
        if (entryStack[i]->type == FUNC &&
            entryStack[i]->name == name) {
            return (FuncTableEntry*)(entryStack[i]);
        }
    }
    return NULL;
}

TableEntry* Table::getEntry(string name) {
    for (int i = 0; i < entryStack.size(); ++i) {
        if (entryStack[i]->name == name) {
            return (entryStack[i]);
        }
    }
    return NULL;
}

TableEntry* Table::getEntry(Variable *id) {
    return getEntry(id->id);
}

bool Table::isDefinedInScope(Variable *id) {
    for (int i = 0; i < entryStack.size(); ++i) {
        if (entryStack[i]->name == id->id) return true;
    }
    return false;
}

/***************************************/
/* Helper functions */
/***************************************/

bool isAlreadyDefined(vector<Table> scopes, Variable *id) {
    for (int i = (int)scopes.size()-1; i >= 0; --i) {
        if (scopes[i].isDefinedInScope(id)) return true;
    }
    return false;
}

FuncTableEntry* funcLookup(vector<Table> scopes, Variable *id) {
    FuncTableEntry *res = NULL;
    for (int i = (int)scopes.size()-1; i >= 0; --i) {
        res = scopes[i].getFuncEntry(id->id);
        if (NULL != res) return res;
    }
    return NULL;
}

TableEntry *idLookup(vector<Table> scopes, Variable *id) {
    TableEntry *res = NULL;
    for (int i = (int)scopes.size()-1; i >= 0; --i) {
        res = scopes[i].getEntry(id->id);
        if (NULL != res) return res;
    }
    return NULL;
}
