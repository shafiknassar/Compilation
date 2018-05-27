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

void rule_Call__ID_LPAREN_ExpList_RPAREN(Id *id);
void rule_Call__ID_LPAREN_RPAREN(Id *id);

Expr rule_Exp__ID_LBRACK_Exp_RBRACK(Id *id);
Expr rule_Exp__Exp_BINOP_Exp(Expr *exp1, Expr *exp2);
Expr rule_Exp__ID(Id *id);
Expr rule_Exp__NUMB(NumVal *num);
Expr rule_Exp__NOT_Exp(Expr *exp);
Expr rule_Exp__Exp_AND_Exp(Expr *exp1, Expr *exp2);
Expr rule_Exp__Exp_OR_Exp(Expr *exp1, Expr *exp2);
Expr rule_Exp__Exp_RELOP_Exp(Expr *exp1, Expr *exp2);
#endif /* semantic_actions_hpp */
