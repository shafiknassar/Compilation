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
/* Functions Rules */
/*****************************************/

void rule_Program__end();

void rule_init();

void rule_FuncHeader(Type *retType, Variable *var, FormList *args);
void rule_FuncBody(Node* stat);

FormList* rule_FormalsList(Variable *fd, FormList *fl);
Variable* rule_FormalDecl__Type_ID(Type *type, Variable *var);
Variable* rule_FormalDecl__Type_ID_NUM(Type *type, Variable *var, Expression *num);
Variable* rule_FormalDecl__Type_ID_NUMB(Type *type, Variable *var, Expression *num);

/*****************************************/
/* Statement Rules */
/*****************************************/

void rule_Statements(Node* stat, Node* marker);
void rule_Statement__Type_ID_SC(Type *type, Variable *var);
void rule_Statement__Type_ID_NUM_SC(Type *type, Variable *var, Expression *num);
void rule_Statement__Type_ID_NUMB_SC(Type *type, Variable *var, Expression *num);
void rule_Statement__Type_ID_ASSIGN_Exp_SC(Type *type, Variable *var, Expression *exp);
void rule_Statement__ID_ASSIGN_Exp_SC(Variable *var, Expression *exp);
void rule_Statement__ID_Exp_ASSIGN_Exp_SC(Variable *var, Expression *exp1, Expression *exp2);
void rule_Statement__Call_SC();
void rule_Statement__RETURN_SC();
void rule_Statement__RETURN_Exp_SC(Expression *exp);
Expression* checkType(Expression* cond);
Node* rule_Statement__IF_Statement(Expression *exp, Node* marker_m, Node* stat, Node* marker_n);
Node* rule_Statement__IF_ELSE_Statement(Expression *exp, Node* marker_m1,
                                       Node* stat1, Node* marker_n,
                                       Node* marker_m2, Node* stat2);
Node* rule_Statement__WHILE_Statement(Expression *exp, Node* marker1,
                                     Node* marker2, Node* stat);
void rule_Statement__BREAK_SC();

/*****************************************/
/* Call Rules */
/*****************************************/

Expression* rule_Call__ID_ExpList(Variable *var, ExprList *expList);
Expression* rule_Call__ID(Variable *var);

/*****************************************/
/* Expression Rules */
/*****************************************/
Expression* rule_Exp__ID_Exp(Variable *var, Expression *exp);
Expression* rule_Exp__ID(Variable *var);
Expression* rule_Exp__Exp_BINOP_Exp(Expression *exp1, string binop, Expression *exp2);
Expression* rule_Exp__TRUE();
Expression* rule_Exp__FALSE();
Expression* rule_Exp__NOT_Exp(Expression *exp);
Expression* rule_Exp__NUM(Expression *num);
Expression* rule_Exp__STRING(Expression *str);
Expression* rule_Exp__Exp_AND_Exp(Expression *exp1, Node* marker, Expression *exp2);
Expression* rule_Exp__Exp_OR_Exp(Expression *exp1, Node* marker, Expression *exp2);
Expression* rule_Exp__Exp_RELOP_Exp(Expression *exp1, string relop, Expression *exp2);
Node* marker__M();
Node* marker__N();

#endif /* SemanticActions_hpp */
