
#include "includes.h"
#include "SemanticActions.hpp"

#define JUMP "j "
using std::map;
using std::string;

/*****************************************/
/* global variables */
/*****************************************/

vector<Table> tableStack;
vector<int>   offsetStack;
Assembler ass;
MipsRegisters regsPool;
bool isMainDef = false;

/*****************************************/
/* 5ara 3arasak */
/*****************************************/

void printScope()
{
    Table curr_scope = tableStack.back();
    for (int i = 0; i < curr_scope.entryStack.size(); i++) {
        TableEntry *entry = (curr_scope.entryStack[i]);
        string type(etos(entry->type));
        if (entry->type == FUNC) {
            FuncTableEntry *func_entry = (FuncTableEntry*)entry;
            vector<string> *args = func_entry->getArgs();
            type = output::makeFunctionType(etos(func_entry->retType->id), *args);
        } else if (isArrType(entry->type)) {
            ArrTableEntry* arr_entry = (ArrTableEntry*)entry;
            TypeId arr_type = convertFromArrType(arr_entry->type);
            type = output::makeArrayType(etos(arr_type), arr_entry->size);
        }
        output::printID(entry->name, entry->offset, type);
    }
}

void openScope()
{
    Type *tmp = tableStack.back().retType;
    bool isWhileTmp = tableStack.back().isWhile;
    tableStack.push_back(*new Table());
    tableStack.back().isFunc  = false;
    tableStack.back().isWhile = isWhileTmp;
    tableStack.back().retType = tmp;
    offsetStack.push_back(offsetStack.back());
}

void closeScope()
{
    output::endScope();
    printScope();
    tableStack.pop_back();
    offsetStack.pop_back();
}

Table& openFuncScope(string funcName, Type *retType)
{
    tableStack.push_back(*new Table());
    tableStack.back().retType = retType;
    tableStack.back().isFunc  = true;
    tableStack.back().isWhile = false;
    offsetStack.push_back(offsetStack.back());
    
    return tableStack.back();
}

void extractTypesFromFormList(FormList &src, vector<TypeId> &dst)
{
    for (int i = 0; i < src.idList.size(); ++i)
    {
        dst.push_back(src.idList[i]->type);
    }
}
/* TODO: delete and fix usage */
void calculateArgOffsets(FormList &src, vector<int> &dst)
{
    int curr = 0;
    vector<int> tmp;
    for (int i = src.size()-1; i >= 0; --i) {
        curr -= src.typeList[i]->size;
        tmp.push_back(curr);
    }
    for (int i = src.size()-1; i >= 0; --i) {
        dst.push_back(tmp[i]);
    }
}

void openWhileScope()  { openScope(); tableStack.back().isWhile = true; }
void closeWhileScope() { closeScope(); }

/*****************************************/
/* Rule Functions */
/*****************************************/

void rule_Program__end() {
    if (!isMainDef) {
        output::errorMainMissing();
        exit(0);
    }
    FuncTableEntry *mainEntry = funcLookup(tableStack, new Variable("main"));
    if (mainEntry->retType->id != M_VOID || mainEntry->paramTypes.size() != 0) {
        output::errorMainMissing();
        exit(0);
    }
    /*output::endScope();
    printScope();*/
    ass.printDataBuffer();
    ass.printCodeBuffer();
}

void rule_init()
{
    vector<Type*> *args = new vector<Type*>();
    args->push_back(new Type(M_STRING, 1));
    tableStack.push_back(*new Table());
    tableStack.back().isFunc  = false;
    tableStack.back().isWhile = false;
    offsetStack.push_back(0);
    tableStack[0].insertFunc("print", new Type(M_VOID, 0), *args);
    args = new vector<Type*>();
    args->push_back(new Type(M_INT, 1));
    tableStack[0].insertFunc("printi", new Type(M_VOID, 0), *args);
}

void rule_Funcs__FuncDecl();

void rule_FuncHeader(Type *retType, Variable *var, FormList *args)
{
    if (!(retType->id == M_INT || retType->id == M_BYTE
          || retType->id == M_BOOL || retType->id == M_VOID))
    {
        output::errorMismatch(yylineno);
        exit(0);
    }
    
    if (var->id == "main") {
        if (isMainDef) {
            output::errorDef(yylineno, var->id);
            exit(0);
        } else {
            isMainDef = true;
        }
    }
    
    if (isAlreadyDefined(tableStack, var)) {
        output::errorDef(yylineno, var->id);
        exit(0);
    }
    
    
    /* Declare function in scope (global) */
    //vector<TypeId> argTypes;
    //extractTypesFromFormList(*args, argTypes);
    tableStack.back().insertFunc(var->id, retType, args->typeList);
    /* create function scope - openScope */
    Table &currScope = openFuncScope(var->id, retType);
    vector<int> argOffsets;
    calculateArgOffsets(*args, argOffsets);
    for (int i = args->size()-1; i >= 0 ; i--) {
        currScope.insert(args->idList[i], args->typeList[i]->id, argOffsets[i]);
    }
    
}


FormList* rule_FormalsList__FormalDecl_COMMA_FormalsList(
                                            FormDec *fd, FormList *fl)
{
    Variable *decl_id = fd->id;
    if(fl->redefined(decl_id)) {
        output::errorDef(yylineno, decl_id->id);
        exit(0);
    }
    fl->add(fd);
    return fl;
}

FormDec* rule_FormalDecl__Type_ID(Type *type, Variable *var)
{
    var->type = type->id;
    var->size = typeSize(var->type);
    if (isAlreadyDefined(tableStack, var)) {
        output::errorDef(yylineno, var->id);
        exit(0);
    }
    //tableStack.back().insert(id, offsetStack.back());
    //offsetStack.back() += type->size;
    return new FormDec(var, type);
}

FormDec* rule_FormalDecl__Type_ID_LBRACK_NUM_RBRACK(Type *type, Variable *var, Expression *num)
{
    int arr_size = atoi(num->value.c_str());
    if (arr_size <= 0 || arr_size >= 256) {
        output::errorInvalidArraySize(yylineno, var->id);
        exit(0);
    }
    var->type = convertToArrType(type->id);
    var->size = arr_size;
    type->id = convertToArrType(type->id);
    type->size = var->size;
    
    
    
    if (var->type == ERROR) {
        output::errorMismatch(yylineno);
        exit(0);
    }
    if (isAlreadyDefined(tableStack, var)) {
        output::errorDef(yylineno, var->id);
        exit(0);
    }
    //tableStack.back().insertArr(id, offsetStack.back(), arr_size);
    //type->size *= arr_size;
    //offsetStack.back() += type->size;
    
    return new FormDec(var, type);
}


FormDec* rule_FormalDecl__Type_ID_LBRACK_NUMB_RBRACK(Type *type, Variable *var, Expression *num)
{
    int arr_size = atoi(num->value.c_str());
    if (arr_size > 255) {
        output::errorByteTooLarge(yylineno, num->value);
        exit(0);
    }
    return rule_FormalDecl__Type_ID_LBRACK_NUM_RBRACK(type, var, num);
}

void rule_Statement__Type_ID_SC(Type *type, Variable *var)
{
    if (isAlreadyDefined(tableStack, var)) {
        output::errorDef(yylineno, var->id);
        exit(0);
    }
    tableStack.back().insert(var, type->id, offsetStack.back());
    offsetStack.back() += type->size;
    ass.allocateLocalVar(WORD_SIZE);
}
void rule_Statement__Type_ID_LBRACK_NUM_RBRACK_SC(Type *type, Variable *var, Expression *num)
{
    int arr_size = atoi(num->value.c_str());
    if (arr_size <= 0 || arr_size >= 256) {
        output::errorInvalidArraySize(yylineno, var->id);
        exit(0);
    }
    var->type = convertToArrType(type->id);
    var->size = arr_size;
    if (var->type == ERROR) {
        output::errorMismatch(yylineno);
        exit(0);
    }
    
    if (isAlreadyDefined(tableStack, var)) {
        output::errorDef(yylineno, var->id);
        exit(0);
    }
    tableStack.back().insertArr(var, offsetStack.back(), arr_size);
    type->size = arr_size;
    offsetStack.back() += type->size;
    
    ass.allocateLocalArr(type->size, WORD_SIZE);
}
void rule_Statement__Type_ID_LBRACK_NUMB_RBRACK_SC(Type *type, Variable *var, Expression *num)
{
    int arr_size = atoi(num->value.c_str());
    if (arr_size > 255) {
        output::errorByteTooLarge(yylineno, num->value);
        exit(0);
    }
    rule_Statement__Type_ID_LBRACK_NUM_RBRACK_SC(type, var, num);
}

void rule_Statement__Type_ID_ASSIGN_Exp_SC(Type* type, Variable *var, Expression *exp)
{
    var->type = type->id;
    var->size = exp->size;
    if (!TYPES_MATCH(var, exp) &&
        !(var->type == M_INT && exp->type == M_BYTE)) {
        output::errorMismatch(yylineno);
        exit(0);
    }
    if (isAlreadyDefined(tableStack, var))
    {
        output::errorDef(yylineno, var->id);
        exit(0);
    }
    tableStack.back().insert(var, type->id, offsetStack.back());
    offsetStack.back() += type->size;
    /* if needed, value of id can be assigned here */
    ass.allocateLocalVar(WORD_SIZE);
    ass.assignValToVar(offsetStack.back()*WORD_SIZE, exp->place);
}

void rule_Statement__ID_ASSIGN_Exp_SC(Variable *var, Expression *exp)
{
    TableEntry *entry = idLookup(tableStack, var);
    if (NULL ==  entry || entry->type == FUNC) {
        output::errorUndef(yylineno, var->id);
        exit(0);
    }
    var->type = entry->type;
    int size = 1;
    if (isArrType(var->type)) size = ((ArrTableEntry*)entry)->size;
    var->size = size;
    

    if (!TYPES_MATCH(var, (exp)) &&
        !BYTE_TO_INT_MATCH(exp, var)) {
        output::errorMismatch(yylineno);
        exit(0);
    }
    //cout << size << " | " << var->size << " | " << exp->size << endl;
    if(isArrType(var->type) && var->size != exp->size) {
        output::errorMismatch(yylineno);
        exit(0);
    }
    if (!isAlreadyDefined(tableStack, var)) {
        output::errorUndef(yylineno, var->id);
        exit(0);
    }
    /* if needed, value of id can be assigned here */
    if (isArrType(var->type)) {
        ass.assignArrToArr(
                           entry->offset*WORD_SIZE,
                           exp->place,
                           size,
                           regsPool.getEmptyRegister());
    } else {
        ass.assignValToVar(entry->offset*WORD_SIZE, exp->place);
    }
}

void rule_Statement__ID_LBRACK_Exp_RBRACK_ASSIGN_Exp_SC(Variable *arr, Expression *exp, Expression *rval)
{
    TableEntry *entry = idLookup(tableStack, arr);
    if (NULL == entry) {
        output::errorUndef(yylineno, arr->id);
        exit(0);
    }
    if ((exp->type != M_INT && exp->type != M_BYTE)
        || !ARR_TYPE_MATCH(entry, rval)){
        output::errorMismatch(yylineno);
        exit(0);
    }
    /* if needed, value of id can be assigned here */
    ass.assignValToArrElem(entry->offset*WORD_SIZE, exp->place, rval->place);
}

void rule_Statement__RETURN_SC()
{
    if (tableStack.back().retType->id != M_VOID)
    {
        output::errorMismatch(yylineno);
        exit(0);
    }
}

void rule_Statement__RETURN_Exp_SC(Expression *exp)
{
    if (tableStack.back().retType->id == M_INT && exp->type == M_BYTE)
        return;
    
    if (tableStack.back().retType->id != exp->type &&
            tableStack.back().retType->size != exp->size)
    {
        output::errorMismatch(yylineno);
        exit(0);
    }
}

Node* rule_Statement__IF_Statement(Expression *cond, Node* marker, Node* stat1) {
    if (cond->type != M_BOOL) {
        output::errorMismatch(yylineno);
        exit(0);
    }
    Node* stat = new Node();
    ass.bpatch(cond->trueList, marker->quad);
    stat->nextList = ass.merge(cond->falseList, stat1->nextList);
    regsPool.unbind(cond->place);
    delete cond;
    return stat;
}

Node* rule_Statement__IF_ELSE_Statement(Expression *cond, Node* marker_m1,
                                        Node* stat1, Node* marker_n,
                                        Node* marker_m2, Node* stat2) {
    if (cond->type != M_BOOL) {
        output::errorMismatch(yylineno);
        exit(0);
    }
    Node* stat = new Node();
    ass.bpatch(cond->trueList, marker_m1->quad);
    ass.bpatch(cond->falseList, marker_m2->quad);
    vector<int> tmp = ass.merge(stat1->nextList, marker_n->nextList);
    stat->nextList = ass.merge(tmp, stat2->nextList);
    regsPool.unbind(cond->place);
    delete cond;
    return stat;
}

Node* rule_Statement__WHILE_Statement(Expression *cond, Node* marker_m1,
                                      Node* marker_m2, Node* stat1) {
    if(cond->type != M_BOOL) {
        output::errorMismatch(yylineno);
        exit(0);
    }
    Node* stat = new Node();
    ass.bpatch(stat1->nextList, marker_m1->quad);
    ass.bpatch(cond->trueList, marker_m2->quad);
    stat->nextList = cond->falseList;
    ass.emitCode(JUMP + marker_m1->quad);
    regsPool.unbind(cond->place);
    delete cond;
    return stat;
}

void rule_Statement__BREAK_SC()
{
    if (!tableStack.back().isWhile) {
        output::errorUnexpectedBreak(yylineno);
        exit(0);
    }
}

bool paramMatchExpected(FuncTableEntry *funcData, ExprList *expList) {
    vector<Type*> &expected = funcData->paramTypes;
    vector<Expression*> &actual   = expList->v;
    if (actual.size() != expected.size()) {
        return false;
    }
    for (int i = 0; i < actual.size(); ++i) {
        if (expected[i]->id == M_INT && actual[i]->type == M_BYTE)
        {
            continue;
        }
        if (expected[i]->id != actual[i]->type ||
                expected[i]->size != actual[i]->size)
        {
            return false;
        }
    }
    return true;
}

vector<string>* typeListToStringVector(vector<Type*> &paramTypes)
{
    vector<string> *res = new vector<string>();
    for (int i = (int)paramTypes.size() - 1 ; i >= 0 ; i--) {
        string name = etos(paramTypes[i]->id);
        if (isArrType(paramTypes[i]->id)) {
            TypeId t = convertFromArrType(paramTypes[i]->id);
            name = output::makeArrayType(etos(t), paramTypes[i]->size);
        }
        res->push_back(name);
    }
    return res;
}

Expression* rule_Call__ID_LPAREN_ExpList_RPAREN(Variable *var, ExprList *expList)
{
    FuncTableEntry *funcData = funcLookup(tableStack, var);
    if (NULL == funcData) {
        output::errorUndefFunc(yylineno, var->id);
        exit(0);
    }
    if (!paramMatchExpected(funcData, expList)) {
        vector<string> *strs = typeListToStringVector(funcData->paramTypes);
        output::errorPrototypeMismatch(yylineno, var->id, *strs);
        exit(0);
    }
    return new Expression(funcData->retType->id, funcData->retType->size);
}
Expression* rule_Call__ID_LPAREN_RPAREN(Variable *var) {
    FuncTableEntry *funcData = funcLookup(tableStack, var);
    if (NULL == funcData) {
        output::errorUndefFunc(yylineno, var->id);
        exit(0);
    }
    if (funcData->paramTypes.size() != 0)
    {
        output::errorPrototypeMismatch(yylineno, var->id, *typeListToStringVector(funcData->paramTypes));
        exit(0);
    }
    return new Expression(funcData->retType->id, funcData->retType->size);
}

Expression* rule_Exp__ID(Variable *var)
{
    TableEntry *entry = idLookup(tableStack, var);
    
    int size = 1;
    if (NULL == entry) {
        output::errorUndef(yylineno, var->id);
        exit(0);
    }
    
    if (isArrType(entry->type))
    {
        size = ((ArrTableEntry*)entry)->size;
    }
    Expression *res = new Expression(entry->type, size);
    string regName = regsPool.getEmptyRegister();
    if (regName == not_found) { /* TODO: WTF DO WE DO? */ }
    regsPool.bind(regName, var->id);
    res->place = regName;
    ass.emitLoadVar(entry->offset*WORD_SIZE, regName, isArrType(entry->type));
    return res;
}

Expression* rule_Exp__ID_LBRACK_Exp_RBRACK(Variable *var, Expression *exp)
{
    TableEntry *entry = idLookup(tableStack, var);
    if (NULL == entry) {
        output::errorUndef(yylineno, var->id);
        exit(0);
    }
    TypeId type = convertFromArrType(entry->type);
    if (type == ERROR) {
        output::errorMismatch(yylineno);
        exit(0);
    }
    Expression *res = new Expression(type);
    string regName = regsPool.getEmptyRegister();
    if (regName == not_found) { /* TODO: WTF DO WE DO? */ }
    regsPool.bind(regName, var->id);
    res->place = regName;
    ass.emitLoadArrElem(entry->offset*WORD_SIZE, exp->place, regName);
    regsPool.unbind(exp->place);
    delete exp;
    
    return res;
}

Expression* rule_Exp__Exp_BINOP_Exp(Expression *exp1, string binop, Expression *exp2)
{
    if (!IS_NUM_TYPE(exp2) || !IS_NUM_TYPE(exp1)) {
        output::errorMismatch(yylineno);
        exit(0);
    }
    TypeId exp_type = M_BYTE;
    if (exp2->type == M_INT || exp1->type == M_INT)
        exp_type = M_INT;
    
    Expression* exp = new Expression(exp_type);
    exp->place = exp1->place;
    regsPool.unbind(exp1->place);
    regsPool.bind(exp->place, expression);
    ass.emitBinOp(binop, exp->place, exp1->place, exp2->place);
    regsPool.unbind(exp2->place);
    
    delete exp1;
    delete exp2;
    
    return exp;
}

Expression* rule_Exp__NUM(Expression *num) {
    string reg = regsPool.getEmptyRegister();
    if (reg == not_found) {/*???*/}
    regsPool.bind(reg, expression);
    num->place = reg;
    ass.emitLoadConst(reg, num->value);
    return num;
}

Expression* rule_Exp__STRING(Expression *str) {
    ass.addStringLiteral(str->value);
    return str;
}

Expression* rule_Exp__TRUE() {
    Expression* exp = new Expression(M_BOOL);
    exp->trueList = ass.makelist(ass.emitCode(JUMP));
    return exp;
}

Expression* rule_Exp__FALSE() {
    Expression* exp = new Expression(M_BOOL);
    exp->falseList = ass.makelist(ass.emitCode(JUMP));
    return exp;
}

Expression* rule_Exp__NOT_Exp(Expression *exp)
{
    if (exp->type != M_BOOL) {
        output::errorMismatch(yylineno);
        exit(0);
    }
    vector<int>* tmplist = &exp->trueList;
    exp->trueList = exp->falseList;
    exp->falseList = *tmplist;
    
    return exp;
}

Expression* rule_Exp__Exp_AND_Exp(Expression *exp1, Node* marker, Expression *exp2)
{
    if (exp1->type != M_BOOL ||
        exp2->type != M_BOOL) {
        output::errorMismatch(yylineno);
        exit(0);
    }
    Expression* exp = new Expression(M_BOOL);
    ass.bpatch(exp1->trueList, marker->quad);
    exp->trueList = exp2->trueList;
    exp->falseList = ass.merge(exp1->falseList, exp2->falseList);
    
    delete exp1;
    delete exp2;
    
    return exp;
}
Expression* rule_Exp__Exp_OR_Exp(Expression *exp1, Node* marker,Expression *exp2)
{
    if (exp1->type != M_BOOL ||
        exp2->type != M_BOOL) {
        output::errorMismatch(yylineno);
        exit(0);
    }
    Expression* exp = new Expression(M_BOOL);
    ass.bpatch(exp1->falseList, marker->quad);
    exp->trueList = ass.merge(exp1->trueList, exp2->trueList);
    exp->falseList = exp2->falseList;
    
    delete exp1;
    delete exp2;
    
    return exp;
}

Expression* rule_Exp__Exp_RELOP_Exp(Expression *exp1, string relop, Expression *exp2)
{
    if (!IS_NUM_TYPE(exp2) || !IS_NUM_TYPE(exp1)) {
        output::errorMismatch(yylineno);
        exit(0);
    }
    int nextinst = -1;
    Expression* exp = new Expression(M_BOOL);
    nextinst = ass.emitRelOp(relop, exp1->place, exp2->place);
    exp->trueList = ass.makelist(nextinst);
    nextinst = ass.emitCode(JUMP);
    exp->falseList = ass.makelist(nextinst);
    exp->place = exp1->place;
    
    delete exp1;
    delete exp2;
    
    return exp;
}

Node* marker__M() {
    Node* marker = new Node();
    marker->quad = ass.getNextInst();
    return marker;
}

Node* marker__N() {
    Node* marker = new Node();
    int next_inst = ass.emitCode(JUMP);
    marker->nextList = ass.makelist(next_inst);
    return marker;
}





