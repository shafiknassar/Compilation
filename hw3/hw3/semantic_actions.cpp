//
//  semantic_actions.cpp
//  hw3
//
//  Created by Bilal Tamish on 27/05/2018.
//  Copyright © 2018 ShafikNassar. All rights reserved.
//

#include "semantic_actions.hpp"

using namespace output;

void rule_init() {
    
}

void rule_Program__Funcs();

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
void rule_Call__ID_LPAREN_ExpList_RPAREN(Id *id);
void rule_Call__ID_LPAREN_RPAREN(Id *id);

Expr rule_Exp__ID_LBRACK_Exp_RBRACK(Id *id);
Expr rule_Exp__Exp_BINOP_Exp(Expr *exp1, Expr *exp2);
Expr rule_Exp__ID(Id *id);
Expr rule_Exp__NUMB(NumVal *num);
Expr rule_Exp__NOT_Exp(Expr *exp);
Expr rule_Exp__Exp_AND_Exp(Expr *exp1, Expr *exp2);
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
