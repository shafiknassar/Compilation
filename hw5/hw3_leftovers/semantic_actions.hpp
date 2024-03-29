//
//  semantic_actions.hpp
//  hw3
//
//  Created by Bilal Tamish on 27/05/2018.
//  Copyright © 2018 ShafikNassar. All rights reserved.
//

#ifndef semantic_actions_hpp
#define semantic_actions_hpp

#include <stdio.h>
#include <string>
#include "attributes.hpp"
#include "output.hpp"

/*****************************************/
/* External Declarations */
/*****************************************/

extern int yylex();
extern int yylineno;
extern char* yytexr;
void yyerror(const char*);
//Table table;

/*****************************************/
/* 5ara 3arasak */
/*****************************************/

void openScope();

void closeScope();

void printScope();

void openWhileScope();

void closeWhileScope();

/*****************************************/
/* Rule Functions */
/*****************************************/

void rule_Program__end();

void rule_init();

void rule_Funcs__FuncDecl();
void rule_FuncHeader(Type *retType, Id *id, FormList *args);
void rule_FuncDecl__RetType_ID_LPAREN_Formals_RPAREN_LBRACE_Statements_RBRACE();

FormList* rule_FormalsList__FormalDecl_COMMA_FormalsList(
                                                FormDec *fd, FormList *fl);
FormDec* rule_FormalDecl__Type_ID(Type *type, Id *id);
FormDec* rule_FormalDecl__Type_ID_LBRACK_NUM_RBRACK(
                                                Type *type, Id *id, NumVal *num);
FormDec* rule_FormalDecl__Type_ID_LBRACK_NUMB_RBRACK(
                                                    Type *type, Id *id, NumVal *num);

void rule_Statements__Statement();
void rule_Statements__Statements_Statement();

void rule_Statement__LBRACE_Statements_RBRACE();
void rule_Statement__Type_ID_SC(Type* type, Id* id);
void rule_Statement__Type_ID_LBRACK_NUM_RBRACK_SC(Type* type, Id* id, NumVal* num);
void rule_Statement__Type_ID_LBRACK_NUMB_RBRACK_SC(Type* type, Id* id, NumVal* num);
void rule_Statement__Type_ID_ASSIGN_Exp_SC(Type *type, Id *id, Expr *exp);
void rule_Statement__ID_ASSIGN_Exp_SC(Id *id, Expr *exp);
void rule_Statement__ID_LBRACK_Exp_RBRACK_ASSIGN_Exp_SC(Id *id, Expr *exp1, Expr *exp2);
void rule_Statement__Call_SC();
void rule_Statement__RETURN_SC();
void rule_Statement__RETURN_Exp_SC(Expr *exp);
void rule_Statement__IF_LPAREN_Exp_RPAREN_Statement(Expr *exp);
void rule_Statement__IF_LPAREN_Exp_RPAREN_StatementWithElse_ELSE_Statement(Expr *exp);
void rule_Statement__WHILE_LPAREN_Exp_RPAREN_Statement(Expr *exp);
void rule_Statement__BREAK_SC();

void rule_StatementWithElse__epsilon();
void rule_StatementWithElse__IF_LPAREN_Exp_RPAREN_StatementWithElse_ELSE_StatementWithElse(Expr *exp);
Expr* rule_Call__ID_LPAREN_ExpList_RPAREN(Id *id, ExprList *expList);
Expr* rule_Call__ID_LPAREN_RPAREN(Id *id);

Expr* rule_Exp__ID_LBRACK_Exp_RBRACK(Id *id);
Expr* rule_Exp__ID(Id *id);
Expr* rule_Exp__Exp_BINOP_Exp(Expr *exp1, Expr *exp2);
Expr* rule_Exp__NUMB(NumVal *num);
Expr* rule_Exp__NOT_Exp(Expr *exp);
Expr* rule_Exp__Exp_AND_Exp(Expr *exp1, Expr *exp2);
Expr* rule_Exp__Exp_OR_Exp(Expr *exp1, Expr *exp2);
Expr* rule_Exp__Exp_RELOP_Exp(Expr *exp1, Expr *exp2);

#endif /* semantic_actions_hpp */
