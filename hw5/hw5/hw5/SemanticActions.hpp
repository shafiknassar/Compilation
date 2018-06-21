//
//  semantic_actions.hpp
//  hw3
//
//  Created by Bilal Tamish on 27/05/2018.
//  Copyright © 2018 ShafikNassar. All rights reserved.
//

#ifndef SemanticActions_hpp
#define SemanticActions_hpp

#include "includes.h"

/*****************************************/
/* External Declarations */
/*****************************************/

extern int yylex();
extern int yylineno;
extern char* yytexr;
void yyerror(const char*);


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
void rule_FuncHeader(Type *retType, Variable *var, FormList *args);
void rule_FuncDecl__RetType_ID_LPAREN_Formals_RPAREN_LBRACE_Statements_RBRACE();

FormList* rule_FormalsList__FormalDecl_COMMA_FormalsList(
                                                FormDec *fd, FormList *fl);
FormDec* rule_FormalDecl__Type_ID(Type *type, Variable *var);
FormDec* rule_FormalDecl__Type_ID_LBRACK_NUM_RBRACK(
                                                Type *type, Variable *var, Expression *num);
FormDec* rule_FormalDecl__Type_ID_LBRACK_NUMB_RBRACK(
                                                    Type *type, Variable *var, Expression *num);

void rule_Statements__Statement();
void rule_Statements__Statements_Statement();

void rule_Statement__LBRACE_Statements_RBRACE();
void rule_Statement__Type_ID_SC(Type *type, Variable *var);
void rule_Statement__Type_ID_LBRACK_NUM_RBRACK_SC(Type *type, Variable *var, Expression *num);
void rule_Statement__Type_ID_LBRACK_NUMB_RBRACK_SC(Type *type, Variable *var, Expression *num);
void rule_Statement__Type_ID_ASSIGN_Exp_SC(Type *type, Variable *var, Expression *exp);
void rule_Statement__ID_ASSIGN_Exp_SC(Variable *var, Expression *exp);
void rule_Statement__ID_LBRACK_Exp_RBRACK_ASSIGN_Exp_SC(Variable *var, Expression *exp1, Expression *exp2);
void rule_Statement__Call_SC();
void rule_Statement__RETURN_SC();
void rule_Statement__RETURN_Exp_SC(Expression *exp);
void rule_Statement__IF_LPAREN_Exp_RPAREN_Statement(Expression *exp);
void rule_Statement__IF_LPAREN_Exp_RPAREN_StatementWithElse_ELSE_Statement(Expression *exp);
void rule_Statement__WHILE_LPAREN_Exp_RPAREN_Statement(Expression *exp);
void rule_Statement__BREAK_SC();

void rule_StatementWithElse__epsilon();
void rule_StatementWithElse__IF_LPAREN_Exp_RPAREN_StatementWithElse_ELSE_StatementWithElse(Expression *exp);
Expression* rule_Call__ID_LPAREN_ExpList_RPAREN(Variable *var, ExprList *expList);
Expression* rule_Call__ID_LPAREN_RPAREN(Variable *var);

Expression* rule_Exp__ID_LBRACK_Exp_RBRACK(Variable *var, Expression *exp);
Expression* rule_Exp__ID(Variable *var);
Expression* rule_Exp__Exp_BINOP_Exp(Expression *exp1, Expression *exp2);
Expression* rule_Exp__TRUE();
Expression* rule_Exp__FALSE();
Expression* rule_Exp__NOT_Exp(Expression *exp);
Expression* rule_Exp__NUM(Expression *num);
Expression* rule_Exp__STRING(Expression *str);
Expression* rule_Exp__Exp_AND_Exp(Expression *exp1, Expression* marker, Expression *exp2);
Expression* rule_Exp__Exp_OR_Exp(Expression *exp1, Expression* marker, Expression *exp2);
Expression* rule_Exp__Exp_RELOP_Exp(Expression *exp1, Expression *exp2);
Expression* marker__M();

#endif /* SemanticActions_hpp */
