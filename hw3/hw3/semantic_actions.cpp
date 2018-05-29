//
//  semantic_actions.cpp
//  hw3
//
//  Created by Bilal Tamish on 27/05/2018.
//  Copyright © 2018 ShafikNassar. All rights reserved.
//

#include "semantic_actions.hpp"
#include <iostream>

using namespace output;
using std::stoi;
using std::to_string;
using std::cout;
using std::endl;

/* it's ok to use those variables when pushing to a vector
 * since the push_back method copies the arg */
Table            cloningTable;

/*****************************************/
/* global variables */
/*****************************************/

vector<Table> tableStack;

vector<int>   offsetStack;

bool isMainDef = false;

/*****************************************/
/* 5ara 3arasak */
/*****************************************/

void printScope()
{
    Table curr_scope = tableStack.back();
    for (int i = 0; i < curr_scope.entryStack.size(); i++) {
        TableEntry *entry = (curr_scope.entryStack.at(i));
        string type(etos(entry->type));
        if (entry->type == FUNC) {
            FuncTableEntry *func_entry = (FuncTableEntry*)entry;
            vector<string> *args = func_entry->getArgs();
            type = makeFunctionType(etos(func_entry->retType->id), *args);
        } else if (isArrType(entry->type)) {
            ArrTableEntry* arr_entry = (ArrTableEntry*)entry;
            TypeId arr_type = convertFromArrType(arr_entry->type);
            type = makeArrayType(etos(arr_type), arr_entry->size);
        }
        printID(entry->name, entry->offset, type);
    }
}

void openScope()
{
    tableStack.push_back(*new Table());
    tableStack.back().isFunc  = false;
    tableStack.back().isWhile = false;
    tableStack.back().retType = tableStack.back().retType;
    offsetStack.push_back(offsetStack.back());
}

void closeScope()
{
    endScope();
    printScope();
    tableStack.pop_back();
    offsetStack.pop_back();
}

Table& openFuncScope(string funcName, Type *retType)
{
    tableStack.push_back(*new Table());
    tableStack.back().retType = retType;
    tableStack.back().isFunc  = true;
    tableStack.back().isWhile = false;
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
/* TODO: delete and fix usage */
void calculateArgOffsets(FormList &src, vector<int> &dst)
{
    int curr = 0;
    for (int i = 0; i < src.size(); ++i) {
        curr -= src.typeList[i]->size;
        dst.push_back(curr);
    }
}

void openWhileScope()  { openScope(); tableStack.back().isWhile = true; }
void closeWhileScope() { closeScope(); tableStack.back().isWhile = false; }

/*****************************************/
/* Rule Functions */
/*****************************************/

void rule_Program__end() {
    if (!isMainDef) {
        errorMainMissing();
        exit(0);
    }
    endScope();
    printScope();
}

void rule_init()
{
    vector<Type*> *args = new vector<Type*>();
    args->push_back(new Type(M_STRING, 0));
    tableStack.push_back(*new Table());
    offsetStack.push_back(0);
    tableStack[0].insertFunc("print", new Type(M_VOID, 0), *args);
    args = new vector<Type*>();
    args->push_back(new Type(M_INT, 4));
    tableStack[0].insertFunc("printi", new Type(M_VOID, 0), *args);
}

void rule_Funcs__FuncDecl();

void rule_FuncHeader(Type *retType, Id *id, FormList *args)
{
    if (!(retType->id == M_INT || retType->id == M_BYTE
          || retType->id == M_BOOL || retType->id == M_VOID))
    {
        errorMismatch(yylineno);
        exit(0);
    }
    
    if (id->id == "main") {
        if (isMainDef) {
            errorDef(yylineno, id->id);
            exit(0);
        } else if (retType->id != M_VOID || args->size() != 0) {
            errorMismatch(yylineno);
            exit(0);
        } else
            isMainDef = true;
    }
    
    if (isAlreadyDefined(tableStack, id)) {
        errorDef(yylineno, id->id);
        exit(0);
    }
    
    
    /* Declare function in scope (global) */
    //vector<TypeId> argTypes;
    //extractTypesFromFormList(*args, argTypes);
    tableStack.back().insertFunc(id->id, retType, args->typeList);
    
    /* create function scope - openScope */
    Table &currScope = openFuncScope(id->id, retType);
    vector<int> argOffsets;
    calculateArgOffsets(*args, argOffsets);
    for (int i = args->size()-1; i >= 0 ; i--) {
        currScope.insert(args->idList[i], args->typeList[i]->id, argOffsets[i]);
    }
    
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
    id->size = typeSize(id->type);
    if (isAlreadyDefined(tableStack, id)) {
        errorDef(yylineno, id->id);
        exit(0);
    }
    //tableStack.back().insert(id, offsetStack.back());
    //offsetStack.back() += type->size;
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
    id->size = typeSize(id->type) * num->val;
    if (id->type == ERROR) {
        errorMismatch(yylineno);
        exit(0);
    }
    if (isAlreadyDefined(tableStack, id)) {
        errorDef(yylineno, id->id);
        exit(0);
    }
    //tableStack.back().insertArr(id, offsetStack.back(), arr_size);
    //type->size *= arr_size;
    //offsetStack.back() += type->size;
    
    return new FormDec(id, type);
}

void rule_Statement__Type_ID_SC(Type* type, Id* id)
{
    if (isAlreadyDefined(tableStack, id)) {
        errorDef(yylineno, id->id);
        exit(0);
    }
    tableStack.back().insert(id, type->id, offsetStack.back());
    offsetStack.back() += type->size;
}
void rule_Statement__Type_ID_LBRACK_NUM_RBRACK_SC(Type* type, Id* id, NumVal* num)
{
    int arr_size = num->val;
    if (arr_size <= 0 || arr_size >= 256) {
        errorInvalidArraySize(yylineno, id->id);
        exit(0);
    }
    id->type = convertToArrType(type->id);
    id->size = typeSize(id->type) * num->val;
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
}
void rule_Statement__Type_ID_LBRACK_NUMB_RBRACK_SC(Type* type, Id* id, NumVal* num)
{
    int arr_size = num->val;
    if (arr_size > 255) {
        long long int n = num->val;
        errorByteTooLarge(yylineno, to_string(n));
        exit(0);
    }
    rule_Statement__Type_ID_LBRACK_NUM_RBRACK_SC(type, id, num);
}

void rule_Statement__Type_ID_ASSIGN_Exp_SC(Type* type, Id *id, Expr *exp)
{
    id->type = type->id;
    id->size = exp->size;
    if (!TYPES_MATCH(id, exp) &&
        !(id->type == M_INT && exp->type == M_BYTE)) {
        errorMismatch(yylineno);
        exit(0);
    }
    if (isAlreadyDefined(tableStack, id))
    {
        errorDef(yylineno, id->id);
        exit(0);
    }
    tableStack.back().insert(id, type->id, offsetStack.back());
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
    TableEntry *entry = idLookup(tableStack, arr);
    if (NULL == entry) {
        errorUndef(yylineno, arr->id);
        exit(0);
    }
    if ((exp->type != M_INT && exp->type != M_BYTE)
        || !ARR_TYPE_MATCH(entry, rval)){
        errorMismatch(yylineno);
        exit(0);
    }
    /* if needed, value of id can be assigned here */
}

void rule_Statement__RETURN_SC()
{
    if (tableStack.back().retType->id != M_VOID)
    {
        errorMismatch(yylineno);
        exit(0);
    }
}

void rule_Statement__RETURN_Exp_SC(Expr *exp)
{
    if (tableStack.back().retType->id == M_INT && exp->type == M_BYTE)
        return;
    
    if (tableStack.back().retType->id != exp->type &&
            tableStack.back().retType->size != exp->size)
    {
        errorMismatch(yylineno);
        exit(0);
    }
}
void rule_Statement__IF_LPAREN_Exp_RPAREN_Statement(Expr *exp) {
    if (exp->type != M_BOOL) {
        errorMismatch(yylineno);
        exit(0);
    }
}
/*
void rule_Statement__IF_LPAREN_Exp_RPAREN_StatementWithElse_ELSE_Statement(Expr *exp)
{
    if (exp->type != BOOL) {
        errorMismatch(yylineno);
        exit(0);
    }
}
 */
void rule_Statement__WHILE_LPAREN_Exp_RPAREN_Statement(Expr *exp)
{
    if(exp->type != M_BOOL) {
        errorMismatch(yylineno);
        exit(0);
    }
}
void rule_Statement__BREAK_SC()
{
    if (!tableStack.back().isWhile) {
        errorUnexpectedBreak(yylineno);
        exit(0);
    }
}

void rule_StatementWithElse__epsilon();
void rule_StatementWithElse__IF_LPAREN_Exp_RPAREN_StatementWithElse_ELSE_StatementWithElse();

bool paramMatchExpected(FuncTableEntry *funcData, ExprList *expList) {
    vector<Type*> &expected = funcData->paramTypes;
    vector<Expr*> &actual   = expList->v;
    if (actual.size() != expected.size()) {
        return false;
    }
    for (int i = 0; i < actual.size(); ++i) {
        if (expected[i]->id != actual[i]->type ||
                expected[i]->size != actual[i]->size)
        {
            return false;
        }
    }
    return true;
}

Expr* rule_Call__ID_LPAREN_ExpList_RPAREN(Id *id, ExprList *expList)
{
    FuncTableEntry *funcData = funcLookup(tableStack, id);
    if (NULL == funcData) {
        errorUndefFunc(yylineno, id->id);
        exit(0);
    }
    if (!paramMatchExpected(funcData, expList)) {
        //errorPrototypeMismatch(yylineno, id->id, /* TODO strings of the input */);
        exit(0);
    }
    return new Expr(*(funcData->retType));
}
Expr* rule_Call__ID_LPAREN_RPAREN(Id *id) {
    FuncTableEntry *funcData = funcLookup(tableStack, id);
    if (NULL == funcData) {
        errorUndefFunc(yylineno, id->id);
        exit(0);
    }
    return new Expr(*(funcData->retType));
}

Expr* rule_Exp__ID_LBRACK_Exp_RBRACK(Id *id)
{
    TableEntry *entry = idLookup(tableStack, id);
    if (NULL == entry) {
        errorUndef(yylineno, id->id);
        exit(0);
    }
    TypeId type = convertFromArrType(entry->type);
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
    if (exp2->type == M_INT || exp1->type == M_INT) {
        return new Expr(M_INT);
    }
    return new Expr(M_BYTE);
}
Expr* rule_Exp__NUMB(NumVal *num)
{
    if (num->val > 255) {
        long long int n = num->val;
        errorByteTooLarge(yylineno, to_string(n));
        exit(0);
    }
    return new ByteVal(num->val);
}

Expr* rule_Exp__NOT_Exp(Expr *exp)
{
    if (exp->type != M_BOOL) {
        errorMismatch(yylineno);
        exit(0);
    }
    return exp;
}
Expr* rule_Exp__Exp_AND_Exp(Expr *exp1, Expr *exp2)
{
    if (exp1->type != M_BOOL ||
        exp2->type != M_BOOL) {
        errorMismatch(yylineno);
        exit(0);
    }
    return new Expr(M_BOOL);
}
Expr* rule_Exp__Exp_OR_Exp(Expr *exp1, Expr *exp2)
{
    if (exp1->type != M_BOOL ||
        exp2->type != M_BOOL) {
        errorMismatch(yylineno);
        exit(0);
    }
    return new Expr(M_BOOL);
}
Expr* rule_Exp__Exp_RELOP_Exp(Expr *exp1, Expr *exp2)
{
    if (!IS_NUM_TYPE(exp2) || !IS_NUM_TYPE(exp1)) {
        errorMismatch(yylineno);
        exit(0);
    }
    return new Expr(M_BOOL);
}
