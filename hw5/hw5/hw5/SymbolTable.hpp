//
//  SymbolTable.hpp
//  hw5
//
//  Created by Bilal Tamish on 19/06/2018.
//  Copyright © 2018 ShafikNassar. All rights reserved.
//

#ifndef SymbolTable_hpp
#define SymbolTable_hpp

#include "includes.h"
using std::string;
using std::vector;

/***************************************/
/* Table Entry */
/***************************************/
struct TableEntry {
    string name;
    TypeId type;
    int offset;
    
    TableEntry(string name, TypeId type, int offset) :
    name(name), type(type), offset(offset) {}
    TableEntry() {};
};

/***************************************/
/* Function Entry */
/***************************************/
struct FuncTableEntry : public TableEntry {
    TypeId retType;
    vector<Type*> paramTypes;
    
    
    FuncTableEntry(string name, TypeId retType, vector<Type*>& paramTypes)
    : TableEntry(name, FUNC, 0), retType(retType), paramTypes(paramTypes) {}
    
    FuncTableEntry() : retType() {}
    
    vector<string>* getArgs();
};

/***************************************/
/* Array Entry */
/***************************************/
struct ArrTableEntry : public TableEntry {
    int size;
    
    ArrTableEntry(string name, TypeId type, int offset, int size) :
    TableEntry(name, type, offset), size(size) {}
    ArrTableEntry() {};
};

/***************************************/
/* Main Table */
/***************************************/
struct Table {
    vector<TableEntry*> entryStack;
    bool isWhile;
    bool isFunc;
    bool haveReturn;
    TypeId retType;
    
    /* Virtual D'tor */
    virtual ~Table() {};
    
    /*default C'tor*/
    Table() : isWhile(false), isFunc(false), haveReturn(false) {}
    
    void insert(string name, TypeId type, int offset);
    void insert(Variable *id, TypeId type, int offset);
    void insertArr(Variable *id, int offset, int size);
    void insertFunc(string name, TypeId retType, vector<Variable*> &paramTypes);

    FuncTableEntry* getFuncEntry(string name);
    TableEntry* getEntry(string name);
    TableEntry* getEntry(Variable *id);
    
    bool isDefinedInScope(Variable *id);
};

/***************************************/
/* Helper functions */
/***************************************/
bool isAlreadyDefined(vector<Table> scopes, Variable *id);

FuncTableEntry* funcLookup(vector<Table> scopes, Variable *id);

TableEntry *idLookup(vector<Table> scopes, Variable *id);


#endif /* SymbolTable_hpp */
