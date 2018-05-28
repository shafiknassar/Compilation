//
//  semantic_actions.cpp
//  hw3
//
//  Created by Bilal Tamish on 27/05/2018.
//  Copyright © 2018 ShafikNassar. All rights reserved.
//

#include "semantic_actions.hpp"

using namespace output;
using std::stoi;
using std::reverse;

/* it's ok to use those variables when pushing to a vector
 * since the push_back method copies the arg */
Table            cloningTable;
FuncScopeTable   cloningFuncTable;

/*****************************************/
/* 5ara 3arasak */
/*****************************************/

void openScope()
{
    tableStack.push_back(cloningTable);
    offsetStack.push_back(offsetStack.back());
}

void closeScope()
{
    endScope();
    tableStack.pop_back();
    offsetStack.pop_back();
    /* TODO print scope variables */
}

Table& openFuncScope(string funcName, Type *retType)
{
    cloningFuncTable.name = funcName;
    cloningFuncTable.retType = retType;
    tableStack.push_back(cloningFuncTable);
    offsetStack.push_back(offsetStack.back());
    
    return tableStack.back();
}

void extractTypesFromFormList(FormList &src, vector<TypeId> &dst)
{
    for (int i = 0; i < src.idList.size(); ++i)
    {
        dst.push_back(src.idList[i]->type);
    }
}

void calculateArgOffsets(FormList &src, vector<int> &dst)
{
    int curr = 0;
    for (int i = 0; i < src.size(); ++i) {
        curr -= src.typeList[i]->size;
        dst.push_back(curr);
    }
}

/*****************************************/
/* Rule Functions */
/*****************************************/

void rule_init()
{
    vector<TypeId> arg;
    arg.push_back(STRING);
    tableStack.push_back(cloningTable);
    tableStack[0].insertFunc("print", VOID, arg);
    arg.pop_back();
    arg.push_back(INT);
    tableStack[0].insertFunc("printi", VOID, arg);
}

void rule_Program__Funcs();

void rule_Funcs__FuncDecl();

// TODO: FuncDecl     : RetType ID LPAREN Formals RPAREN LBRACE Statements RBRACE
//{
//    //TODO: should check if defined (in the table);
//    String func_name = (Id*)$2->id;
//    TypeId func_ret = (Type*)$1->type;
//
//
//
//}


void rule_FuncHeader(Type *retType, Id *id, FormList *args)
{
    if (!(retType->id == INT || retType->id == BYTE
          || retType->id == BOOL || retType->id == VOID))
    {
        errorMismatch(yylineno);
        exit(0);
    }
    
    if (isAlreadyDefined(tableStack, id))
    {
        errorDef(yylineno, id->id);
        exit(0);
    }
    
    /* Declare function in scope (global) */
    vector<TypeId> argTypes;
    extractTypesFromFormList(*args, argTypes);
    tableStack.back().insertFunc(id->id, retType->id, argTypes);
    
    /* create function scope - openScope */
    Table &currScope = openFuncScope(id->id, retType);
    vector<int> argOffsets;
    calculateArgOffsets(*args, argOffsets);
    for (int i = args->size()-1; i >= 0 ; i++) {
        currScope.insert(args->idList[i], argOffsets[i]);
    }
    
}

void rule_FuncDecl__RetType_ID_LPAREN_Formals_RPAREN_LBRACE_Statements_RBRACE(
            Type *retType, Id *id, FormList *args)
{
    
    
}


FormList* rule_FormalsList__FormalDecl_COMMA_FormalsList(
                                            FormDec *fd, FormList *fl)
{
    Id *decl_id = fd->id;
    if(fl->redefined(decl_id)) {
        errorDef(yylineno, decl_id->id);
        exit(0);
    }
    fl->add(fd);
    return fl;
}
FormDec* rule_FormalDecl__Type_ID(Type *type, Id *id)
{
    id->type = type->id;
    if (isAlreadyDefined(tableStack, id)) {
        errorDef(yylineno, id->id);
        exit(0);
    }
    tableStack.back().insert(id, offsetStack.back());
    offsetStack.back() += type->size;
    return new FormDec(id, type);
}

FormDec* rule_FormalDecl__Type_ID_LBRACK_NUM_RBRACK(Type *type, Id *id, NumVal *num)
{
    int arr_size = num->val;
    if (arr_size <= 0 || arr_size >= 256) {
        errorInvalidArraySize(yylineno, id->id);
        exit(0);
    }
    id->type = convertToArrType(type->id);
    if (id->type == ERROR) {
        errorMismatch(yylineno);
        exit(0);
    }
    if (isAlreadyDefined(tableStack, id)) {
        errorDef(yylineno, id->id);
        exit(0);
    }
    tableStack.back().insertArr(id, offsetStack.back(), arr_size);
    type->size *= arr_size;
    offsetStack.back() += type->size;
    
    return new FormDec(id, type);
}

void rule_Statements__Statement();
void rule_Statements__Statements_Statement();

void rule_Statement__LBRACE_Statements_RBRACE()
{
    
}
void rule_Statement__FormalDecl_SC()
{
    
}
void rule_Statement__Type_ID_ASSIGN_Exp_SC(Type* type, Id *id, Expr *exp)
{
    id->type = type->id;
    if (!TYPES_MATCH(id, exp) &&
        !(id->type == INT && exp->type == BYTE)) {
        errorMismatch(yylineno);
        exit(0);
    }
    if (tableStack.back().isDefinedInScope(id))
    {
        errorDef(yylineno, id->id);
        exit(0);
    }
    tableStack.back().insert(id, offsetStack.back());
    offsetStack.back() += type->size;
    /* if needed, value of id can be assigned here */
}
void rule_Statement__ID_ASSIGN_Exp_SC(Id *id, Expr *exp)
{
    if (!TYPES_MATCH(id, (exp)) &&
        !BYTE_TO_INT_MATCH(exp, id)) {
        errorMismatch(yylineno);
        exit(0);
    }
    if (!isAlreadyDefined(tableStack, id)) {
        errorUndef(yylineno, id->id);
        exit(0);
    }
    /* if needed, value of id can be assigned here */
}

void rule_Statement__ID_LBRACK_Exp_RBRACK_ASSIGN_Exp_SC(Id *arr, Expr *exp, Expr *rval)
{
    if ((exp->type != INT && exp->type != BYTE)
        || !ARR_TYPE_MATCH(arr, rval)){
        errorMismatch(yylineno);
        exit(0);
    }
    if (!isAlreadyDefined(tableStack, arr)) {
        errorUndef(yylineno, arr->id);
        exit(0);
    }
    /* if needed, value of id can be assigned here */
}

void rule_Statement__Call_SC();
void rule_Statement__RETURN_SC();
void rule_Statement__RETURN_Exp_SC();
void rule_Statement__IF_LPAREN_Exp_RPAREN_Statement();
void rule_Statement__IF_LPAREN_Exp_RPAREN_StatementWithElse_ELSE_Statement();
void rule_Statement__WHILE_LPAREN_Exp_RPAREN_Statement();
void rule_Statement__BREAK_SC();

void rule_StatementWithElse__epsilon();
void rule_StatementWithElse__IF_LPAREN_Exp_RPAREN_StatementWithElse_ELSE_StatementWithElse();
void rule_Call__ID_LPAREN_ExpList_RPAREN(Id *id);
void rule_Call__ID_LPAREN_RPAREN(Id *id);

Expr* rule_Exp__ID_LBRACK_Exp_RBRACK(Id *id)
{
    TypeId type = convertFromArrType(id->type);
    if (type == ERROR)
    {
        errorMismatch(yylineno);
        exit(0);
    }
    return new Expr(type);
}
Expr* rule_Exp__Exp_BINOP_Exp(Expr *exp1, Expr *exp2)
{
    if (!IS_NUM_TYPE(exp2) || !IS_NUM_TYPE(exp1)) {
        errorMismatch(yylineno);
        exit(0);
    }
    if (exp2->type == INT || exp1->type == INT) {
        return new Expr(INT);
    }
    return new Expr(BYTE);
}
Expr* rule_Exp__NUMB(NumVal *num)
{
    if (num->val > 255) {
        errorByteTooLarge(yylineno, to_string(num->val));
        exit(0);
    }
    return new ByteVal(num->val);
}
Expr* rule_Exp__NOT_Exp(Expr *exp)
{
    if (exp->type != BOOL) {
        errorMismatch(yylineno);
        exit(0);
    }
    return exp;
}
Expr* rule_Exp__Exp_AND_Exp(Expr *exp1, Expr *exp2)
{
    if (exp1->type != BOOL ||
        exp2->type != BOOL) {
        errorMismatch(yylineno);
        exit(0);
    }
    return new Expr(BOOL);
}
Expr* rule_Exp__Exp_OR_Exp(Expr *exp1, Expr *exp2)
{
    if (exp1->type != BOOL ||
        exp2->type != BOOL) {
        errorMismatch(yylineno);
        exit(0);
    }
    return new Expr(BOOL);
}
Expr* rule_Exp__Exp_RELOP_Exp(Expr *exp1, Expr *exp2)
{
    if (!IS_NUM_TYPE(exp2) || !IS_NUM_TYPE(exp1)) {
        errorMismatch(yylineno);
        exit(0);
    }
    return new Expr(BOOL);
}
