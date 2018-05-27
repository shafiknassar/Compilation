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

/* global variables */
vector<Table> tableStack;
vector<int>   offsetStack;
bool isMainDef = false;

void rule_Program__Funcs();

void rule_init();

void rule_Funcs__FuncDecl();

// TODO: FuncDecl     : RetType ID LPAREN Formals RPAREN LBRACE Statements RBRACE

void rule_FormalsList__Formal_Decl(FormDec *fd);

void rule_FormalDecl__Type_ID(TypeId type, Id *id);
void rule_FormalDecl__Type_ID_LBRACK_NUM_RBRACK();
void rule_FormalDecl__Type_ID_LBRACK_NUM_B_RBRACK();

void rule_Statements__Statement();
void rule_Statements__Statements_Statement();

void rule_Statement__LBRACE_Statements_RBRACE();
void rule_Statement__FormalDecl_SC();
void rule_Statement__Type_ID_ASSIGN_Exp_SC();
void rule_Statement__ID_ASSIGN_Exp_SC();
void rule_Statement__ID_LBRACK_Exp_RBRACK_ASSIGN_Exp_SC();
void rule_Statement__Call_SC();
void rule_Statement__RETURN_SC();
void rule_Statement__RETURN_Exp_SC();
void rule_Statement__IF_LPAREN_Exp_RPAREN_Statement();
void rule_Statement__IF_LPAREN_Exp_RPAREN_StatementWithElse_ELSE_Statement();
void rule_Statement__WHILE_LPAREN_Exp_RPAREN_Statement();
void rule_Statement__BREAK_SC();

void rule_StatementWithElse__epsilon();
void rule_StatementWithElse__IF_LPAREN_Exp_RPAREN_StatementWithElse_ELSE_StatementWithElse();

#endif /* semantic_actions_hpp */
