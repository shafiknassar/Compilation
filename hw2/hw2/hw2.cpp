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
#define TO_TOKEN(i)         (static_cast<tokens>(i))
#define for_each_token(t)   for (t = STARTSTRUCT; t <= EF; ++t)
#define for_each_nt(nt)     for (nt; nt < NONTERMINAL_ENUM_SIZE; ++nt)
#define contains(set, e)    ((set).find(e) != (set).end())
#define IS_NULLABLE(nt)     (nullableBitMap[nt])

/*****************************************************/
/* Using */
/*****************************************************/
using std::vector;
using std::set;

/*****************************************************/
/* Data structure manipulation functions */
/*****************************************************/
/*
 * merge the 2 input sets into the dst.
 * Returns true iff dst was actually changed.
 */
template<typename T>
bool mergeSets(set<T> &dst, set<T> &src) {
    bool res = false;
    typename set<T>::iterator it;
    for (it = src.begin(); it != src.end(); ++it) {
        T t = *it;
        if (!contains(dst, t)) {
            res = true;
            dst.insert(t);
        }
    }
    return res;
}

template<typename T>
vector<T> vectorFromRange(vector<T> &v, int start, int end)
{
    vector<T> toReturn;
    if (end == -1) end = v.size()-1;
    for (int i = start; i <= end; i++) {
        toReturn.push_back(v[i]);
    }
    return toReturn;
}

/*****************************************************/
/* Static functions */
/*****************************************************/

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


vector <grammar_rules*> mapNonterminalsToRules = divideGrammarRulesByLhs();

static bool isNullable(nonterminal nt)
{
    for (int i = 0; i < mapNonterminalsToRules[nt]->size(); ++i) {
        grammar_rule rule = (*(mapNonterminalsToRules[nt]))[i];
        bool isRuleNullable = true;
        for (int j = 0; rule.rhs.size(); ++j) {
            int var = rule.rhs[j];
            /* TODO: what happens if var == nt? infinite recursion -
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

static vector<bool> compute_nullable_internal(){
    vector<bool> nullableBitMap;
    for (int i = 0; i < NONTERMINAL_ENUM_SIZE; ++i) {
        nullableBitMap.push_back(isNullable(TO_NT(i)));
    }
    return nullableBitMap;
}

vector<bool> nullableBitMap = compute_nullable_internal();

static vector<set<tokens> > first_algorithm()
{
    vector<set<tokens> > firsts;
    for (int i = 0; i < NONTERMINAL_ENUM_SIZE; ++i) {
        set<tokens> set; /*empty set*/
        firsts.push_back(set);
    }
    bool isStable;
    do {
        isStable = true;
        for (int i = 0; i < grammar.size(); ++i) {
            grammar_rule rule = grammar[i];
            vector<int> &rhs = rule.rhs;
            for (int j = 0; j < rhs.size(); ++j) {
                if (IS_TERMINAL(rhs[j])) {
                    if (!contains(firsts[rule.lhs], TO_TOKEN(rhs[j]))) {
                        isStable = false;
                        firsts[rule.lhs].insert(TO_TOKEN(rhs[j]));
                    }
                    break;
                } else { /* rhs[j] is nonterminal */
                    if (mergeSets(firsts[rule.lhs], firsts[rhs[j]])) {
                        isStable = false;
                    }
                    if (!isNullable(TO_NT(rhs[j]))) break;
                }
            }
        }
    } while (!isStable);
    return firsts;
}

vector<set<tokens> > firsts = first_algorithm();

static set<tokens> first(vector<int> &word)
{
    set<tokens> res;
    int i = 0;
    while (i < word.size()) {
        if (IS_TERMINAL(word[i])) {
            res.insert(TO_TOKEN(word[i]));
            break;
        } else { /* word[0] is nonterminal */
            mergeSets(res, firsts[word[i]]);
            if (!IS_NULLABLE(word[i])) break;
        }
        i++;
    }
    //
    return res;
}

static bool isNullable(vector<int> &v)
{
    for (int i = 0; i < v.size(); ++i) {
        if (IS_TERMINAL(v[i]) || !IS_NULLABLE(TO_NT(v[i]))) {
            return false;
        }
    }
    return true;
}

static vector<set<tokens> > follow_algorithm()
{
    vector<set<tokens> > res;
    for (int i = 0; i < NONTERMINAL_ENUM_SIZE; ++i) {
        set<tokens> set; /*empty set*/
        res.push_back(set);
    }
    res[S].insert(EF);
    
    bool isStable;
    do {
        isStable = true;
        for (int i = 0; i < grammar.size(); ++i) {
            grammar_rule rule = grammar[i];
            vector<int> &rhs = rule.rhs;
            for (int j = 0; j < rhs.size(); ++j) {
                if (IS_TERMINAL(rhs[j])) continue;
                vector<int> after = vectorFromRange(rhs, j+1, -1);
                set<tokens> s = first(after);
                if (mergeSets(res[rhs[j]], s)) isStable = false;
                if (isNullable(after)) {
                    if (mergeSets(res[rhs[j]], res[rule.lhs])) isStable = false;
                }
            }
        }
    } while (!isStable);
    return res;
}

vector<set<tokens> > follows = follow_algorithm();

set<tokens> select(grammar_rule r)
{
    set<tokens> res = first(r.rhs);
    if (isNullable(r.rhs)) {
        mergeSets(res, follows[r.lhs]);
    }
    return res;
}

static vector<set<tokens> > compute_select_internal()
{
    vector<set<tokens> > res;
    for (int i = 0; i < grammar.size(); ++i) {
        res.push_back(select(grammar[i]));
    }
    return res;
}

vector<set<tokens> > selects = compute_select_internal();

/*****************************************************/
/*****************************************************/

/**
 * determines which variables are nullable, i.e. can produce an empty word
 * calls print_nullable when finished
 */
void compute_nullable(){
    print_nullable(nullableBitMap);
}

/**
 * computes first for all nonterminal (see nonterminal enum in grammar.h)
 * calls print_first when finished
 */
void compute_first(){
    print_first(firsts);
}

/**
 * computes follow for all nonterminal (see nonterminal enum in grammar.h)
 * calls print_follow when finished
 */
void compute_follow(){
    print_follow(follows);
}

/**
 * computes select for all grammar rules (see grammar global variable in grammar.h)
 * calls print_select when finished
 */
void compute_select(){
    print_select(selects);
}

/**
 * implements an LL(1) parser for the grammar using yylex()
 */
void parser(){
    
}
