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


using std::string;
using std::stringstream;
using std::stoi;
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
    BOOL_ARR
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

struct NumVal : public Node {
    int val;
    NumVal(int i) : val(i) {}
    NumVal(string i) : val(stoi(i)) {}
};

struct Id : public Node {
    string id;
    TypeId type;
    Id(string id) : id(id) {}
};

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



struct RelOp : public Expr {
    Node *left, *right;
    RelOpId id;
    RelOp(Node *l, Node *r, RelOpId id) :
    left(l), right(r), id(id), Expr(BOOL) {}
};

/***************************************/
/* Node structs for terminals */
/***************************************/

TypeId convertFromArrType(TypeId arr_t) {
    switch (arr_t) {
        case INT_ARR:
            return INT;
            break;
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
            break;
        case BYTE:
            return BYTE_ARR;
        case BOOL:
            return BOOL_ARR;
        default:
            break;
    }
    return ERROR;
}



#endif /* attributes_hpp */


