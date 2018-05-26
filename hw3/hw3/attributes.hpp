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


using std::string;
using std::stringstream;
using std::stoi;
using std::vector;

/***************************************/
/* Defines */
/***************************************/

#define YYSTYPE (Node*)

#define TYPES_MATCH(exp1, exp2) \
            ((exp1)->type == (exp2)->type)

#define ARR_TYPE_MATCH(arr, exp) \
            ((convertFromArrType((arr)->type) == (exp)->type) \
            ||

#define BYTE_TO_INT_MATCH(src, dst) \
            ((src)->type == BYTE && (dst)->type == INT)

#define IS_NUM_TYPE(exp)  ((exp)->type == BYTE || (exp)->type == INT)

/***************************************/
/* Enums */
/***************************************/

typedef enum {
    ERROR,
    VOID,
    INT,
    BYTE,
    BOOL,
    STRING,
    INT_ARR,
    BYTE_ARR,
    BOOL_ARR,
    FUNC
} TypeId;

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
};

struct Id : public Node {
    string id;
    TypeId type;
    Id(string id) : id(id) {}
};

/* Expressions */

struct Expr : public Node {
    TypeId type;
    Expr(TypeId type): type(type) {}
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
    FormDec(Id *id) : id(id) {};
};

struct FormList : public Node {
    vector<TypeId> typesList;
    void add(TypeId t) { typesList.push_back(t); }
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
    TypeId retType;
    vector<TypeId> paramTypes;
    
    FuncTableEntry(string name,
                   TypeId retType,
                   vector<TypeId>& paramTypes)
        : TableEntry(name, FUNC, INT_MIN),
        retType(retType),
        paramTypes(paramTypes) {}
    
    FuncTableEntry() {}
};

struct Table {
    vector<TableEntry> entryStack;
    
    /*default C'tor*/
    
    void insert(string name, TypeId type, int offset)
    {
        TableEntry tmp;
        tmp.name   = name;
        tmp.type   = type;
        tmp.offset = offset;
        entryStack.push_back(tmp);
    }
    
    void insertFunc(string name,
                    TypeId retType,
                    vector<TypeId> paramTypes)
    {
        FuncTableEntry tmp;
        tmp.name       = name;
        tmp.type       = FUNC;
        tmp.offset     = INT_MIN;
        tmp.retType    = retType;
        /* The following parameter can be
         most easily acquired from a struct
         specificaly created to FormalDecls */
        tmp.paramTypes = paramTypes;
        entryStack.push_back(tmp);
    }
};




#endif /* attributes_hpp */


