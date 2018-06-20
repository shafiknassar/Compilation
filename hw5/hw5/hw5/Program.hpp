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
    NONE
};

/***************************************/
/* Node structs for terminals */
/***************************************/
struct Node {
    virtual ~Node();
};

struct Type : public Node {
    TypeId id;
    int size;
    Type(TypeId id, int size) : id(id), size(size) {};
    Type(Type const &t)       : id(t.id), size(t.size) {};
    Type()                    : id(NONE), size(-1) {}
};

struct Variable : public Node {
    string id;
    TypeId type;
    int size;
    Variable(string id) : id(id) {}
    Variable(string id, TypeId type) : id(id), type(type) {}
};

struct Function : public Node {
    Function(){};
};

struct Programe : public Node {
    Programe(){};
};

struct Expression : public Node {
    TypeId type;
    string place;
    string quad;
    vector<int> trueList;
    vector<int> falseList;
    int size;
    Expression(TypeId type) : type(type), size(typeSize(type)) {}
    Expression(Type t)      : type(t.id), size(t.size) {}
    Expression(TypeId type, int size) : type(type), size(size) {}
};

struct ExprList : public Node {
    vector<Expression*> v;
};

struct NumVal : public Expression {
    int val;
    string sVal;
    NumVal(int i, string sVal) : val(i), Expression(M_INT), sVal(sVal) {}
};

struct Constant : public Expression {
    int val;
    string sVal;
    Constant(int i, string sVal, Type type) : val(i), Expression(type), sVal(sVal) {}
};

/* Formal Declaration */

struct FormDec : public Node {
    Variable *id;
    Type *type;
    FormDec(Variable *id, Type *t) : id(id), type(t) {};
};

struct FormList : public Node {
    vector<Variable*> idList;
    vector<Type*> typeList;
    
    void add(Variable *id, Type *t);
    void add(FormDec *fd);
    bool redefined(Variable* id);
    int size();
    
};


/***************************************/
/* Enums & Helper functions */
/***************************************/

int typeSize(TypeId id);
bool isArrType(TypeId t);
string etos(TypeId type);
TypeId convertFromArrType(TypeId arr_t);
TypeId convertToArrType(TypeId arr_t);

#endif /* program_hpp */


