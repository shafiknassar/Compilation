//
//  attributes.hpp
//  hw3
//
//  Created by Shafik Nassar on 23/05/2018.
//  Copyright © 2018 ShafikNassar. All rights reserved.
//

#ifndef attributes_hpp
#define attributes_hpp

#include <stdio.h>
#include <sstream>
#include <stdlib.h>
#include <algorithm>
#include <vector>
#include <cassert>
#include "TypeId.h"
#include "output.hpp"
#include <iostream>

using namespace std;
using namespace output;
using std::string;
using std::stringstream;
using std::vector;
using std::find;
using std::cout;

/***************************************/
/* Defines */
/***************************************/

#define YYSTYPE Node*

#define TYPES_MATCH(exp1, exp2) \
            ((exp1)->type == (exp2)->type)

#define ARR_TYPE_MATCH(arr, exp)                              \
            ((convertFromArrType((arr)->type) == (exp)->type) \
            || (arr)->type == INT_ARR && (exp)->type == M_BYTE)

#define BYTE_TO_INT_MATCH(src, dst) \
            ((src)->type == M_BYTE && (dst)->type == M_INT)

#define IS_NUM_TYPE(exp)  ((exp)->type == M_BYTE || (exp)->type == M_INT)

/***************************************/
/* Enums */
/***************************************/

int typeSize(TypeId id);
string etos(TypeId type);

bool isArrType(TypeId t);


/***************************************/
/* Node structs for terminals */
/***************************************/
struct Node {
};

struct Type : public Node {
    TypeId id;
    int size;
    Type(TypeId id, int size) : id(id), size(size) {};
    Type(Type const &t)       : id(t.id), size(t.size) {};
    Type()                    : id(NONE), size(-1) {}
};

struct Id : public Node {
    string id;
    TypeId type;
    int size;
    Id(string id) : id(id) {}
};

/* Expressions */

struct Expr : public Node {
    TypeId type;
    int size;
    Expr(TypeId type) : type(type), size(typeSize(type)) {}
    Expr(Type t)      : type(t.id), size(t.size) {}
    Expr(TypeId type, int size) : type(type), size(size) {}
};

struct ExprList : public Node {
    vector<Expr*> v;
};

struct StringVal : public Expr {
    string val;
    StringVal(string s) : val(s), Expr(M_STRING) {}
};

struct BoolVal : public Expr {
    bool val;
    BoolVal(bool b) : val(b), Expr(M_BOOL) {}
};

struct NumVal : public Expr {
    int val;
    NumVal(int i) : val(i), Expr(M_INT) {}
};

struct ByteVal : public Expr {
    int val;
    ByteVal(int i) : val(i), Expr(M_BYTE) {}
};

/* Formal Declaration */

struct FormDec : public Node {
    Id *id;
    Type *type;
    FormDec(Id *id, Type *t) : id(id), type(t) {};
};

struct FormList : public Node {
    vector<Id*> idList;
    vector<Type*> typeList;

    void add(Id *id, Type *t) { idList.push_back(id); typeList.push_back(t); }
    void add(FormDec *fd)     { idList.push_back(fd->id); typeList.push_back(fd->type); }
    bool redefined(Id* id) {
        for (int i = 0; i < idList.size(); ++i) {
            if (id->id == idList[i]->id) return true;
        }
        return false;
    }
    
    int size() { return (int)idList.size(); }
    
};

/***************************************/
/* Helper functions */
/***************************************/

TypeId convertFromArrType(TypeId arr_t);

TypeId convertToArrType(TypeId arr_t);


/***************************************/
/* Symbols Tables */
/***************************************/

struct TableEntry {
    string name;
    TypeId type;
    int offset;
    
    TableEntry(string name, TypeId type, int offset) :
        name(name), type(type), offset(offset) {}
    TableEntry() {};
};

struct FuncTableEntry : public TableEntry {
    Type *retType;
    vector<Type*> paramTypes;
    
    
    FuncTableEntry(string name,
                   Type *retType,
                   vector<Type*>& paramTypes)
        : TableEntry(name, FUNC, 0),
        retType(),
        paramTypes(paramTypes) {
            this->retType = new Type(*retType);
        }
    
    FuncTableEntry() : retType(new Type()) {}
    vector<string>* getArgs() {
        vector<string> *args = new vector<string>();
        for (int i = (int)paramTypes.size()-1; i >= 0; i--) {
            string typeStr = etos(paramTypes.at(i)->id);
            if (isArrType(paramTypes.at(i)->id)) {
                TypeId type = convertFromArrType(paramTypes.at(i)->id);
                typeStr = makeArrayType(etos(type), paramTypes.at(i)->size);
            }
            args->push_back(typeStr);
        }
        return args;
    }
};

struct ArrTableEntry : public TableEntry {
    int size;
    
    ArrTableEntry(string name, TypeId type, int offset, int size) :
    TableEntry(name, type, offset), size(size) {}
    ArrTableEntry() {};
};

struct Table {
    vector<TableEntry*> entryStack;
    bool isWhile;
    bool isFunc;
    Type *retType;
    
    /* Virtual D'tor */
    virtual ~Table() {};
    
    /*default C'tor*/
    Table() : isWhile(false), isFunc(false), retType(new Type()) {}
    
    void insert(string name, TypeId type, int offset)
    {
        //cout << "Table::insert " << name << " " <<  etos(type) << endl;
        entryStack.push_back(new TableEntry(name, type, offset));
    }
    
    void insertArr(Id *id, int offset, int size) {
        //cout << "Table::insertArr " << " " << id->id << " "<< size << endl;
        ArrTableEntry *tmp = new ArrTableEntry(id->id, id->type, offset, size);
        entryStack.push_back(tmp);
    }
    
    FuncTableEntry* getFuncEntry(string name) {
        for (int i = 0; i < entryStack.size(); ++i) {
            if (entryStack[i]->type == FUNC &&
                    entryStack[i]->name == name) {
                return (FuncTableEntry*)(entryStack[i]);
            }
        }
        return NULL;
    }
    
    TableEntry* getEntry(string name) {
        for (int i = 0; i < entryStack.size(); ++i) {
            if (entryStack[i]->name == name) {
                return (entryStack[i]);
            }
        }
        return NULL;
    }
    
    TableEntry* getEntry(Id *id) { return getEntry(id->id); }

    
    void insertFunc(string name,
                    Type *retType,
                    vector<Type*> &paramTypes)
    {
        //cout << "Table::insertFunc " << name << " " << etos(retType->id) << endl;
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
        tmp->paramTypes = paramTypes;
        entryStack.push_back(tmp);
    }
    
    void insert(Id *id, TypeId type, int offset) {
        if (isArrType(type)) {
            insertArr(id, offset, id->size);
            return;
        }
        insert(id->id, type, offset);
    }
    
    bool isDefinedInScope(Id *id) {
        for (int i = 0; i < entryStack.size(); ++i) {
            if (entryStack[i]->name == id->id) return true;
        }
        return false;
    }
};

struct FuncScopeTable : public Table {
};

bool isAlreadyDefined(vector<Table> scopes, Id *id);

FuncTableEntry* funcLookup(vector<Table> scopes, Id *id);

TableEntry *idLookup(vector<Table> scopes, Id *id);


#endif /* attributes_hpp */


