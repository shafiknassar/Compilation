//
//  hw2.cpp
//  hw2
//
//  Created by Shafik Nassar on 29/04/2018.
//  Copyright © 2018 ShafikNassar. All rights reserved.
//

#include <stdio.h>
#include "hw2.h"
#include "grammar.h"

/*****************************************************/
/* Defines */
/*****************************************************/
#define IS_NONTERMINAL(var) ((var) < NONTERMINAL_ENUM_SIZE)
#define IS_TERMINAL(var)    (!IS_NONTERMINAL(var))
#define TO_NT(i)            (static_cast<nonterminal>(i))

/*****************************************************/
/* Static functions */
/*****************************************************/

using std::vector;

typedef vector <grammar_rule> grammar_rules;

static vector <grammar_rules*> divideGrammarRulesByLhs()
{
    std::vector <grammar_rules*> res;
    for (int i = 0; i < NONTERMINAL_ENUM_SIZE; ++i) {
        res.push_back(new grammar_rules);
    }
    for (int i = 0; i < grammar.size(); ++i) {
        grammar_rule gr = grammar[i];
        nonterminal lhs = gr.lhs;
        res[lhs]->push_back(gr);
    }
    return res;
}

static void freeVecOfGrammarRules(vector <grammar_rules*>* v)
{
    for (int i = 0; i < v->size(); ++i) {
        delete (*v)[i];
    }
    delete v;
}

vector <grammar_rules*> mapNonterminalsToRules = divideGrammarRulesByLhs();

static bool isNullable(nonterminal nt)
{
    for (int i = 0; i < mapNonterminalsToRules[nt]->size(); ++i) {
        grammar_rule rule = (*(mapNonterminalsToRules[nt]))[i];
        bool isRuleNullable = true;
        for (int j = 0; rule.rhs.size(); ++j) {
            int var = rule.rhs[j];
            /* TODO: what happens if var == nt? infinite loop - 
             for now, breaking the loop in this case */
            if (var == nt || IS_TERMINAL(var) || !isNullable(TO_NT(var))) {
                isRuleNullable = false;
                break;
            }
        }
        if (isRuleNullable) return true;
    }
    return false;
}

/*****************************************************/
/*****************************************************/

/**
 * determines which variables are nullable, i.e. can produce an empty word
 * calls print_nullable when finished
 */
void compute_nullable(){
    vector<bool> nullableBitMap;
    for (int i = 0; i < NONTERMINAL_ENUM_SIZE; ++i) {
        nullableBitMap.push_back(isNullable(TO_NT(i)));
    }
    print_nullable(nullableBitMap);
}

/**
 * computes first for all nonterminal (see nonterminal enum in grammar.h)
 * calls print_first when finished
 */
void compute_first(){
    
}

/**
 * computes follow for all nonterminal (see nonterminal enum in grammar.h)
 * calls print_follow when finished
 */
void compute_follow(){
    
}

/**
 * computes select for all grammar rules (see grammar global variable in grammar.h)
 * calls print_select when finished
 */
void compute_select(){
    
}

/**
 * implements an LL(1) parser for the grammar using yylex()
 */
void parser(){
    
}
/*
 * implemented in lex.yy.c (generated from lexer.lex)
 */
/*int yylex(){
    return 0;
}*/

