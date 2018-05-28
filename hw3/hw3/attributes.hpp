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
#include <vector>
#include <cassert>


using std::string;
using std::stringstream;
using std::stoi;
using std::vector;
using std::find;

/***************************************/
/* Defines */
/***************************************/

#define YYSTYPE (Node*)

#define TYPES_MATCH(exp1, exp2) \
            ((exp1)->type == (exp2)->type)

#define ARR_TYPE_MATCH(arr, exp)                              \
            ((convertFromArrType((arr)->type) == (exp)->type) \
            || (arr)->type == INT_ARR && (exp)->type == BYTE)

#define BYTE_TO_INT_MATCH(src, dst) \
            ((src)->type == BYTE && (dst)->type == INT)

#define IS_NUM_TYPE(exp)  ((exp)->type == BYTE || (exp)->type == INT)

/***************************************/
/* Enums */
/***************************************/

typedef enum {
    ERROR,
    /* Original Types */
    VOID,
    INT,
    BYTE,
    BOOL,
    /* end of Original Types */
    STRING,
    INT_ARR,
    BYTE_ARR,
    BOOL_ARR,
    FUNC,
    NONE
} TypeId;

int typeSize(TypeId id)
{
    switch (id) {
        case INT:
            return 4;
        case VOID:
            return 0;
        case STRING: ;
        default: ;
    }
    return 1;
}

typedef enum {
    PLUS,
    MINUS,
    MULT,
    DIV
} BinOpId;

typedef enum {
    AND,
    OR,
    NOT
} LogOpId;

typedef enum {
    IF,
    ELSE,
    WHILE,
    BREAK,
    RETURN
} CtrlFlowId;


typedef enum {
    EQ,
    NEQ,
    GE,
    GT,
    LE,
    LT
} RelOpId;

/***************************************/
/* Node structs for terminals */
/***************************************/
struct Node {
    //virtual string print() = 0;
};

struct Type : public Node {
    TypeId id;
    int size;
    Type(TypeId id, int size) : id(id), size(size) {};
    Type(Type const &t)       : id(t.id), size(t.size) {};
    Type()                    : id(ERROR), size(-1) {}
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
};

struct ExprList : public Node {
    vector<Expr*> v;
};

struct BinOp : public Expr {
    Node *left, *right;
    BinOpId id;
    
    BinOp(Node *l, Node *r, BinOpId id) :
        left(l), right(r), id(id), Expr(INT) {}
    
};

struct LogOp : public Expr {
    Node *left, *right;
    LogOpId id;
    
    LogOp(Node *l, Node *r, LogOpId id) :
    left(l), right(r), id(id), Expr(BOOL) {}
};

struct StringVal : public Expr {
    string val;
    StringVal(string s) : val(s), Expr(STRING) {}
};

struct BoolVal : public Expr {
    bool val;
    BoolVal(bool b) : val(b), Expr(BOOL) {}
};

struct NumVal : public Expr {
    int val;
    NumVal(int i) : val(i), Expr(INT) {}
    NumVal(string i) : val(stoi(i)), Expr(INT) {}
};

struct ByteVal : public Expr {
    int val;
    ByteVal(int i) : val(i), Expr(BYTE) {}
    ByteVal(string i) : val(stoi(i)), Expr(BYTE) {}
};

struct RelOp : public Expr {
    Node *left, *right;
    RelOpId id;
    RelOp(Node *l, Node *r, RelOpId id) :
    left(l), right(r), id(id), Expr(BOOL) {}
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
        if(find(idList.begin(), idList.end(), id) != idList.end())
            return true;
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
        : TableEntry(name, FUNC, INT_MIN),
        retType(retType),
        paramTypes(paramTypes) {}
    
    FuncTableEntry() {}
};

struct ArrTableEntry : public TableEntry {
    int size;
    
    ArrTableEntry(string name, TypeId type, int offset, int size) :
    TableEntry(name, type, offset), size(size) {}
    ArrTableEntry() {};
};

struct Table {
    vector<TableEntry> entryStack;
    
    /* Virtual D'tor */
    virtual ~Table() {};
    
    /*default C'tor*/
    
    void insert(string name, TypeId type, int offset)
    {
        TableEntry tmp(name, type, offset);
        entryStack.push_back(tmp);
    }
    
    void insertArr(Id *id, int offset, int size) {
        ArrTableEntry tmp(id->id, id->type, offset, size);
        entryStack.push_back(tmp);
    }
    
    FuncTableEntry* getFuncEntry(string name) {
        for (int i = 0; i < entryStack.size(); ++i) {
            if (entryStack[i].type == FUNC &&
                    entryStack[i].name == name) {
                return (FuncTableEntry*)&(entryStack[i]);
            }
        }
        return NULL;
    }
    
    void insertFunc(string name,
                    Type *retType,
                    vector<Type*> &paramTypes)
    {
        FuncTableEntry tmp;
        tmp.name       = name;
        tmp.type       = FUNC;
        tmp.offset     = INT_MIN;
        tmp.retType    = retType;
        /* 
         The following parameter can be
         most easily acquired from a struct
         specificaly created to FormalDecls
         */
        tmp.paramTypes = paramTypes;
        entryStack.push_back(tmp);
    }
    
    void insert(Id *id, int offset) {
        insert(id->id, id->type, offset);
    }
    
    bool isDefinedInScope(Id *id) {
        for (int i = 0; i < entryStack.size(); ++i) {
            if (entryStack[i].name == id->id) return true;
        }
        return false;
    }
};

struct FuncScopeTable : public Table {
    Type *retType;
    string name;
};

bool isAlreadyDefined(vector<Table> scopes, Id *id);

FuncTableEntry* funcLookup(vector<Table> scopes, Id *id);


#endif /* attributes_hpp */


