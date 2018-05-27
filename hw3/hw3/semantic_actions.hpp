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
#include "attributes.cpp"
#include "output.hpp"

/* global variables */
vector<Table> tableStack;
vector<int>   offsetStack;
bool isMainDef = false;
void rule_Program__Funcs();
extern int yylineno;

void rule_init();

void rule_Funcs__FuncDecl();

// TODO: FuncDecl     : RetType ID LPAREN Formals RPAREN LBRACE Statements RBRACE

void rule_FormalsList__Formal_Decl(FormDec *fd);

void rule_FormalDecl__Type_ID(TypeId type, Id *id);
void rule_FormalDecl__Type_ID_LBRACK_NUM_RBRACK(TypeId type, Id *id, NumVal num);
//void rule_FormalDecl__Type_ID_LBRACK_NUM_B_RBRACK(TypeId type, Id *id, NumVal num);

void rule_Statements__Statement();
void rule_Statements__Statements_Statement();

void rule_Statement__LBRACE_Statements_RBRACE();
void rule_Statement__FormalDecl_SC(FormDec *fm);
void rule_Statement__Type_ID_ASSIGN_Exp_SC(TypeId type, Id *id, Expr *exp);
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
void rule_Call__ID_LPAREN_ExpList_RPAREN(Id *id);
void rule_Call__ID_LPAREN_RPAREN(Id *id);

Expr* rule_Exp__ID_LBRACK_Exp_RBRACK(Id *id);
Expr* rule_Exp__Exp_BINOP_Exp(Expr *exp1, Expr *exp2);
Expr* rule_Exp__NUMB(NumVal *num);
Expr* rule_Exp__NOT_Exp(Expr *exp);
Expr* rule_Exp__Exp_AND_Exp(Expr *exp1, Expr *exp2);
Expr* rule_Exp__Exp_OR_Exp(Expr *exp1, Expr *exp2);
Expr* rule_Exp__Exp_RELOP_Exp(Expr *exp1, Expr *exp2);

#endif /* semantic_actions_hpp */
