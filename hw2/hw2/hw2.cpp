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
#include <map>

/*****************************************************/
/* Defines */
/*****************************************************/
#define IS_NONTERMINAL(var) ((var) < NONTERMINAL_ENUM_SIZE)
#define IS_TERMINAL(var)    (!IS_NONTERMINAL(var))
#define TO_NT(i)            (static_cast<nonterminal>(i))
#define TO_TOKEN(i)         (static_cast<tokens>(i))
#define for_each_token(t)   for (int _##t = STARTSTRUCT; (t=TO_TOKEN(_##t)) <= EF; ++_##t)
#define for_each_nt(t)      for (int _##t = S; (t=TO_NT(_##t)) < NONTERMINAL_ENUM_SIZE; ++_##t)
#define contains(set, e)    ((set).find(e) != (set).end())
#define IS_NULLABLE(nt)     (nullableBitMap[nt])
#define ERROR               (-1)

/*********************/
/* DEBUG TRACE */

#define TRACE               printf("%s ; %d", __FUNCTION__,__LINE__); printf

/*****************************************************/
/* Using */
/*****************************************************/
using std::vector;
using std::set;
using std::map;

typedef vector <grammar_rule>               grammar_rules;
typedef map<nonterminal, map<tokens, int> > table;
typedef vector<int>                         AnalasysStack;

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
            __FUNCTION__;
            __LINE__;
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
/* Global Variables */
/*****************************************************/
/* We use global variables to follow
 the mythodology used in grammar.cpp */


//vector <grammar_rules*> mapNonterminalsToRules;
//vector<bool> nullableBitMap = compute_nullable_internal();
//vector<set<tokens> > firsts = first_algorithm();
//vector<set<tokens> > selects = compute_select_internal();
//vector<set<tokens> > follows = follow_algorithm();

/*****************************************************/
/* Static functions */
/*****************************************************/


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

#define table_contains(table, nt, t)              \
        ((table).find(nt) != (table).end() &&     \
        (table)[nt].find(t) != (table)[nt].end())

static table make_table()
{
    table new_table;
    tokens t;
    nonterminal nt;
    set<tokens >::iterator it;
    
    for_each_nt(nt) {
        for_each_token(t) {
            new_table[nt][t] = ERROR;
        }
    }
    
    for (int i = 0; i < selects.size(); ++i) {
        nt = grammar[i].lhs;
        for_each_token(t) {
            /* TODO: make sure that no element was in [nt][t] before hand */
            /* TODO: bilal, please check that I didn't a5re bl code */
            if (contains(selects[i], t)) {
                if (ERROR != new_table[nt][t]) {
                    printf("Conflict between rule %d and %d\n", i, new_table[nt][t]);
                } else {
                    new_table[nt][t] = i;
                }
            }
        }
    }
    return new_table;
}

table selects_table = make_table();

inline static int M(int X, int t)
{
    return selects_table[TO_NT(X)][TO_TOKEN(t)];
}

inline static AnalasysStack init_stack()
{
    AnalasysStack res;
    res.push_back(S);
    return res;
}

AnalasysStack stack = init_stack();

static bool match(int x, int t)
{
    bool res = (x == t);
    TRACE ("matching %d and %d\n", x,t);
    if (res) {
        stack.pop_back();
    } // maybe else print something?
    return res;
}

static bool predict(int x, int t)
{
    int rule_number = M(x,t);
    TRACE("x=%d, t=%d\n", x,t);
    if (ERROR == rule_number) return false; /* or maybe print something? */
    printf("%d\n", rule_number);
    vector<int> rhs = grammar[rule_number].rhs;
    TRACE("popped %d\n", stack.back());
    stack.pop_back();
    for (int i = static_cast<int>(rhs.size())-1; i >= 0; --i) {
        stack.push_back(rhs[i]);
        TRACE("     pushed %d\n", stack.back());
    }
    return true;
}

/* returns true iff the parsing succeeded */
bool LL1()
{
    tokens t = TO_TOKEN(yylex());
    while (stack.size() > 0) {
        TRACE("Token is %d\n", t);
        TRACE("Stack size = %lu\n", stack.size());
        int head = stack.back();
        TRACE("trace: head is %d\n", head);
        if (IS_TERMINAL(head)) {
            if (!match(head, t)) {
                goto syntax_error;
            }
        } else {
            if (!predict(head, t)) {
                goto syntax_error;
            }
        }
        t = TO_TOKEN(yylex());
    }
    if (EF == t) {
        printf("Success\n");
        return true;
    }
syntax_error:
    printf("Syntax error\n");
    return false;
}


/*****************************************************/
/* Tests API */
/*****************************************************/

void makeTableTest() {
    table tbl = make_table();
    tokens t;
    nonterminal nt;
    for_each_token(t) {
        for_each_nt(nt) {
            printf("table[%d][%d] = %d \n", nt, t, tbl[nt][t]);
        }
    }
}

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
    LL1();
}
