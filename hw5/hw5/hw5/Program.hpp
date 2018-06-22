//
//  attributes.hpp
//  hw3
//
//  Created by Shafik Nassar on 23/05/2018.
//  Copyright © 2018 ShafikNassar. All rights reserved.
//

#ifndef program_hpp
#define program_hpp

#include "includes.h"
using std::string;
using std::vector;

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

enum TypeId {
    ERROR,
    /* Original Types */
    M_VOID,
    M_INT,
    M_BYTE,
    M_BOOL,
    /* end of Original Types */
    M_STRING,
    INT_ARR,
    BYTE_ARR,
    BOOL_ARR,
    FUNC,
    MARKER,
    NONE
};

/***************************************/
/* Helper functions */
/***************************************/

int typeSize(TypeId id);
bool isArrType(TypeId t);
string etos(TypeId type);
TypeId convertFromArrType(TypeId arr_t);
TypeId convertToArrType(TypeId arr_t);

/***************************************/
/* Node structs for terminals */
/***************************************/
struct Node {
    string token;
    string quad;
    TypeId type;
    int size;
    vector<int> nextList;
    virtual ~Node() {};
    
    Node() : token(""), quad(""), nextList() {};
    Node(string s) : token(s) {}
    Node(TypeId tId) : type(tId), size(typeSize(tId)) {};
    Node(TypeId tId, int sz) : type(tId), size(sz) {};
};

struct Type : public Node {
    Type(TypeId tId) : Node(tId) {};
    Type(TypeId tId, int size) : Node(tId, size) {};
    Type(Type const &t)       : Node(t.type, t.size) {};
    Type()                    : Node(NONE) {}
};

struct Variable : public Node {
    string id;
    Variable(TypeId t) : Node(t), id("none") {}
    Variable(string id) : id(id) {}
    Variable(string id, TypeId type) : id(id), Node(type) {}
};

struct Function : public Node {
    Function(){};
};

struct Programe : public Node {
    Programe(){};
};

struct Expression : public Node {
    string value;
    string place;
    vector<int> trueList;
    vector<int> falseList;
    
    Expression(TypeId type) : Node(type) {}
    Expression(TypeId type, int size) : Node(type, size) {}
    Expression(TypeId type, string value) : Node(type), value(value) {}
};

struct ExprList : public Node {
    vector<Expression*> v;
};

/* Formal Declaration */

struct FormList : public Node {
    vector<Variable*> idList;
    
    void add(Variable *id, Type *t);
    void add(Variable *id);
    bool redefined(Variable* id);
    int size();
    
};



#endif /* program_hpp */


