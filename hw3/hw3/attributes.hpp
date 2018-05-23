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

/***************************************/
/* Enums */
/***************************************/

typedef enum {
    VOID,
    INT,
    BYTE,
    B,
    BOOL
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
    Type(TypeId id) : id(id) {};
};

struct BinOp : public Node {
    Node *left, *right;
    BinOpId id;
    
    BinOp(Node *l, Node *r, BinOpId id) :
        left(l), right(r), id(id) {}
    
};

struct LogOp : public Node {
    Node *left, *right;
    LogOpId id;
    
    LogOp(Node *l, Node *r, LogOpId id) :
    left(l), right(r), id(id) {}
};

struct StringVal : public Node {
    string val;
    StringVal(string s) : val(s) {}
};

struct BoolVal : public Node {
    bool val;
    BoolVal(bool b) : val(b) {}
};

struct NumVal : public Node {
    int val;
    NumVal(int i) : val(i) {}
};


struct RelOp : public Node {
    Node *left, *right;
    RelOpId id;
    RelOp(Node *l, Node *r, RelOpId id) :
    left(l), right(r), id(id) {}
};


#endif /* attributes_hpp */


