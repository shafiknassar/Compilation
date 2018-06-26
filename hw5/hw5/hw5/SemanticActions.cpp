
#include "includes.h"
#define FUNC_RES_REG "$v0"

using std::map;
using std::string;
using std::make_pair;

/*****************************************/
/* global variables */
/*****************************************/

vector<Table> tableStack;
vector<int>   offsetStack;
vector<string> functionsStack;

Assembler ass;
MipsRegisters &regsPool = ass.regAllocator;
bool isMainDef = false;

/*****************************************/
/* Helper Functions */
/*****************************************/

bool paramMatchExpected(FuncTableEntry *funcData, ExprList *expList) {
    vector<Type*> &expected = funcData->paramTypes;
    vector<Expression*> &actual   = expList->v;
    if (actual.size() != expected.size()) {
        return false;
    }
    for (int i = 0; i < actual.size(); ++i) {
        if (expected[i]->type == M_INT && actual[i]->type == M_BYTE)
        {
            continue;
        }
        if (expected[i]->type != actual[i]->type ||
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
        string name = etos(paramTypes[i]->type);
        if (isArrType(paramTypes[i]->type)) {
            TypeId t = convertFromArrType(paramTypes[i]->type);
            name = output::makeArrayType(etos(t), paramTypes[i]->size);
        }
        res->push_back(name);
    }
    return res;
}

vector<pair<string, int> > getPlacesFromList(vector<Expression*> list) {
    vector<pair<string, int> > res;
    for (int i = 0; i< list.size(); i++) {
        int ifArr = isArrType(list[i]->type) ? list[i]->size : NOT_ARR;
        res.push_back(make_pair(list[i]->place, ifArr));
    }
    return res;
}

void openScope()
{
    TypeId retTypetmp = tableStack.back().retType;
    bool isWhileTmp = tableStack.back().isWhile;
    tableStack.push_back(*new Table());
    tableStack.back().isWhile = isWhileTmp;
    tableStack.back().retType = retTypetmp;
    offsetStack.push_back(offsetStack.back());
}

void closeScope()
{
    //output::endScope();
    //printScope();
    Table tbl = tableStack.back();
    /* clearing scope's local vars */
    int total_sz = 0;
    if (tbl.isFunc) goto CLEANUP;
    for (int i = 0; i < tbl.entryStack.size(); ++i) {
        int sz = 1;
        if (isArrType(tbl.entryStack[i]->type)) {
            sz = ((ArrTableEntry*)tbl.entryStack[i])->size;
        } else if (tbl.entryStack[i]->type == FUNC) {
            sz = 0;
        }
        total_sz += sz;
    }
    if (total_sz != 0) {
        stringstream ss;
        ss << total_sz * WORD_SIZE;
        ass.emitCode("    addu $sp, $sp, " + ss.str());
    }
CLEANUP:
    tableStack.pop_back();
    offsetStack.pop_back();
}

Table& openFuncScope(Type *retType)
{
    tableStack.push_back(*new Table());
    tableStack.back().retType = retType->type;
    tableStack.back().isFunc  = true;
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

void calculateArgOffsets(FormList &src, vector<int> &dst)
{
    int curr = 0;
    vector<int> tmp;
    for (int i = src.size()-1; i >= 0; --i) {
        curr -= src.idList[i]->size;
        tmp.push_back(curr);
    }
    for (int i = src.size()-1; i >= 0; --i) {
        dst.push_back(tmp[i]);
    }
}

void openWhileScope()  { openScope(); tableStack.back().isWhile = true; }
void closeWhileScope() { closeScope(); }

void handleBoolExp(Expression* exp) {
    string next = ass.getNextInst();
    //DBUG(__FUNCTION__ << __LINE__ << " " <<next)
    ass.bpatch(exp->trueList, next);
    ass.emitBool(exp->place, true);
    int true_line = ass.emitCode(JUMP);
    vector<int> true_inst = ass.makelist(true_line);
    next = ass.getNextInst();
    //DBUG(__FUNCTION__ << __LINE__ << " " <<next)
    ass.bpatch(exp->falseList, next);
    ass.emitBool(exp->place, false);
    next = ass.getNextInst();
    //DBUG(__FUNCTION__ << __LINE__ << " " <<next)
    ass.bpatch(true_inst, next);
}

/*****************************************/
/* Rule Functions */
/*****************************************/

void rule_Program__end() {
    FuncTableEntry *mainEntry = funcLookup(tableStack, new Variable("main"));
    if (!mainEntry ||
        mainEntry->retType != M_VOID || mainEntry->paramTypes.size() != 0) {
        output::errorMainMissing();
        exit(0);
    }
    ass.printDataBuffer();
    ass.printCodeBuffer();
}

void rule_init() {
    //MARK: DONE
    tableStack.push_back(*new Table());
    offsetStack.push_back(0);
    
    vector<Variable*> *args = new vector<Variable*>();
    args->push_back(new Variable(M_STRING));
    tableStack[0].insertFunc("print", M_VOID, *args);
    args->clear();
    args->push_back(new Variable(M_INT));
    tableStack[0].insertFunc("printi", M_VOID, *args);
    
    ass.emitProgramInit();
}

void rule_FuncHeader(Type *retType, Variable *var, FormList *args)
{
    //MARK: DONE
    TypeId return_type = retType->type;
    if (!(return_type == M_INT || return_type == M_BYTE
          || return_type == M_BOOL || return_type == M_VOID))
    {
        output::errorMismatch(yylineno);
        exit(0);
    }
    
    if (isAlreadyDefined(tableStack, var)) {
        output::errorDef(yylineno, var->id);
        exit(0);
    }
    
    /* Declare function in scope (global) */
    tableStack.back().insertFunc(var->id, return_type, args->idList);
    functionsStack.push_back(var->id);
    
    /* create function scope - openScope */
    Table &currScope = openFuncScope(retType);
    
    vector<int> argOffsets;
    calculateArgOffsets(*args, argOffsets);
    for (int i = args->size()-1; i >= 0 ; i--) {
        if (var->id == args->idList[i]->id) {
            output::errorDef(yylineno, var->id);
            exit(0);
        }
        currScope.insert(args->idList[i], args->idList[i]->type, argOffsets[i]);
    }
    ass.emiFunctionHeader(var->id);
}

void rule_FuncBody(Node* stat) {
    string func_name = functionsStack.back();
    if (tableStack.back().haveReturn) return;
    ass.bpatch(stat->nextList, ass.getNextInst());
    ass.emitFunctionReturn(func_name);
}

/*****************************************/
/* Formal Decleration Rules */
/*****************************************/

FormList* rule_FormalsList(Variable *id, FormList *fl)
{
    //MARK: DONE
    if(fl->redefined(id)) {
        output::errorDef(yylineno, id->id);
        exit(0);
    }
    fl->add(id);
    return fl;
}

Variable* rule_FormalDecl__Type_ID(Type *type, Variable *var)
{
    //MARK: DONE
    var->type = type->type;
    var->size = typeSize(var->type);
    if (isAlreadyDefined(tableStack, var)) {
        output::errorDef(yylineno, var->id);
        exit(0);
    }

    return var;
}

Variable* rule_FormalDecl__Type_ID_NUM(Type *type, Variable *var, Expression *num)
{
    //MARK: DONE
    int arr_size = atoi(num->value.c_str());
    if (arr_size <= 0 || arr_size >= 256) {
        output::errorInvalidArraySize(yylineno, var->id);
        exit(0);
    }
    var->type = convertToArrType(type->type);
    var->size = arr_size;
    
    if (var->type == ERROR) {
        output::errorMismatch(yylineno);
        exit(0);
    }
    if (isAlreadyDefined(tableStack, var)) {
        output::errorDef(yylineno, var->id);
        exit(0);
    }
    
    return var;
}


Variable* rule_FormalDecl__Type_ID_NUMB(Type *type, Variable *var, Expression *num)
{
    //MARK: DONE
    int arr_size = atoi(num->value.c_str());
    if (arr_size > 255) {
        output::errorByteTooLarge(yylineno, num->value);
        exit(0);
    }
    return rule_FormalDecl__Type_ID_NUM(type, var, num);
}

/*****************************************/
/* Statement Rules */
/*****************************************/

Node* rule_Statements(Node* stats, Node* marker, Node* stat) {
    //MARK: DONE
    //DBUG(__FUNCTION__ << __LINE__ << marker->quad)
    ass.bpatch(stats->nextList, marker->quad);
    stat->breakList = ass.merge(stats->breakList, stat->breakList);
    return stat;
}

Node* rule_StatementScope(Node* stat, Node* marker) {
    ass.bpatch(stat->nextList, marker->quad);
    return stat;
}

void rule_Statement__Type_ID_SC(Type *type, Variable *var)
{
    //MARK: DONE
    if (isAlreadyDefined(tableStack, var)) {
        output::errorDef(yylineno, var->id);
        exit(0);
    }
    tableStack.back().insert(var, type->type, offsetStack.back());
    offsetStack.back() += type->size;
    ass.allocateLocalVar();
}

void rule_Statement__Type_ID_NUM_SC(Type *type, Variable *var, Expression *num)
{
    //MARK: DONE
    int arr_size = atoi(num->value.c_str());
    if (arr_size <= 0 || arr_size >= 256) {
        output::errorInvalidArraySize(yylineno, var->id);
        exit(0);
    }
    var->type = convertToArrType(type->type);
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
    
    ass.allocateLocalArr(type->size);
    regsPool.unbind(num->place);
    
}
void rule_Statement__Type_ID_NUMB_SC(Type *type, Variable *var, Expression *num)
{
    //MARK: DONE
    int arr_size = atoi(num->value.c_str());
    if (arr_size > 255) {
        output::errorByteTooLarge(yylineno, num->value);
        exit(0);
    }
    rule_Statement__Type_ID_NUM_SC(type, var, num);
}

void rule_Statement__Type_ID_ASSIGN_Exp_SC(Type* type, Variable *var, Expression *exp)
{
    //MARK: DONE
    var->type = type->type;
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
    if (exp->type == M_BOOL) {
        handleBoolExp(exp);
    }
    tableStack.back().insert(var, type->type, offsetStack.back());
    offsetStack.back() += type->size;
    /* if needed, value of id can be assigned here */
    ass.allocateLocalVar(exp->place);
    regsPool.unbind(exp->place);
}

void rule_Statement__ID_ASSIGN_Exp_SC(Variable *var, Expression *exp)
{
    //MARK: DONE
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
    if(isArrType(var->type) && var->size != exp->size) {
        output::errorMismatch(yylineno);
        exit(0);
    }
    if (!isAlreadyDefined(tableStack, var)) {
        output::errorUndef(yylineno, var->id);
        exit(0);
    }
    if (exp->type == M_BOOL) {
        handleBoolExp(exp);
    }
    /* if needed, value of id can be assigned here */
    if (isArrType(var->type)) {
        ass.assignArrToArr(
                           (-1)*entry->offset*WORD_SIZE,
                           exp->place,
                           size);
    } else {
        ass.assignValToVar((-1)*entry->offset*WORD_SIZE, exp->place);
    }
    regsPool.unbind(exp->place);
}

void rule_Statement__ID_Exp_ASSIGN_Exp_SC(Variable *arr, Expression *exp, Expression *rval)
{
    //MARK: DONE
    TableEntry *entry = idLookup(tableStack, arr);
    if (NULL == entry) {
        output::errorUndef(yylineno, arr->id);
        exit(0);
    }
    if ((exp->type != M_INT && exp->type != M_BYTE)
        || !ARR_TYPE_MATCH(entry, rval)) {
        output::errorMismatch(yylineno);
        exit(0);
    }
    if (rval->type == M_BOOL) {
        handleBoolExp(rval);
    }
    int size = 0;
    string sizeReg = regsPool.getEmptyRegister();
    stringstream ss;
    if (isArrType(entry->type)) size = ((ArrTableEntry*)entry)->size;
    ss << size;
    ass.emitCode("    li " + sizeReg + ", " + ss.str());
    ass.assignValToArrElem((-1)*entry->offset*WORD_SIZE, sizeReg, exp->place, rval->place);
    regsPool.unbind(exp->place);
    regsPool.unbind(rval->place);
}

void rule_Statement__RETURN_SC()
{
    //MARK: DONE
    if (tableStack.back().retType != M_VOID)
    {
        output::errorMismatch(yylineno);
        exit(0);
    }
    string funcName = functionsStack.back();
    if (tableStack.back().isFunc) tableStack.back().haveReturn = true;
    ass.emitFunctionReturn(funcName);
    regsPool.unbindAll();
}

void rule_Statement__RETURN_Exp_SC(Expression *exp)
{
    //MARK: DONE
    if (tableStack.back().retType == M_INT && exp->type == M_BYTE)
        return;
    TypeId func_ret_type = tableStack.back().retType;
    if (func_ret_type == M_VOID ||
       (func_ret_type != exp->type && func_ret_type != exp->size))
    {
        output::errorMismatch(yylineno);
        exit(0);
    }
    if (exp->type == M_BOOL) {
        handleBoolExp(exp);
    }
    string funcName = functionsStack.back();
    if (tableStack.back().isFunc) tableStack.back().haveReturn = true;
    ass.emitFunctionReturn(funcName, exp->place);
    regsPool.unbind(exp->place);
    regsPool.unbindAll();
}

Expression* checkType(Expression* cond) {
    if (cond->type != M_BOOL) {
        output::errorMismatch(yylineno);
        exit(0);
    }
    return cond;
}

Node* rule_Statement__IF_Statement(Expression *cond, Node* marker_m, Node* stat1, Node* marker_n) {
    //MARK: DONE
    //DBUG("rule_Statement__IF_Statement " <<  marker_m->quad)
    
    Node* stat = new Node();
    ass.bpatch(cond->trueList, marker_m->quad);
    stat->nextList = ass.merge(ass.merge(cond->falseList, stat1->nextList), marker_n->nextList);
//    vector<int> tmp =
//    string next = ass.getNextInst();
//    tmp = ass.merge(tmp, marker_n->nextList);
//    DBUG(__FUNCTION__ << __LINE__ << " " <<next)
//    ass.bpatch(tmp, next);
    stat->breakList = stat1->breakList;
    regsPool.unbind(cond->place);
    delete cond;
    
    return stat;
}

Node* rule_Statement__IF_ELSE_Statement(Expression *cond, Node* marker_m1,
                                        Node* stat1, Node* marker_n,
                                        Node* marker_m2, Node* stat2) {
    //MARK: DONE
    //DBUG("rule_Statement__IF_ELSE_Statement " << marker_m1->quad << " " << marker_m2->quad)
    if (cond->type != M_BOOL) {
        output::errorMismatch(yylineno);
        exit(0);
    }
    Node* stat = new Node();
    ass.bpatch(cond->trueList, marker_m1->quad);
    ass.bpatch(cond->falseList, marker_m2->quad);
    vector<int> tmp = ass.merge(stat1->nextList, marker_n->nextList);
    stat->nextList = ass.merge(tmp, stat2->nextList);
//    string next = ass.getNextInst();
//    DBUG(__FUNCTION__ << __LINE__ << " " <<next)
//    ass.bpatch(stat->nextList, next);
    stat->breakList = ass.merge(stat1->breakList, stat2->breakList);
    regsPool.unbind(cond->place);
    delete cond;
    return stat;
}

Node* rule_Statement__WHILE_Statement(Expression *cond, Node* marker_m1,
                                      Node* marker_m2, Node* stat1) {
    //MARK: DONE
    if(cond->type != M_BOOL) {
        output::errorMismatch(yylineno);
        exit(0);
    }
    Node* stat = new Node();
    ass.bpatch(stat1->nextList, marker_m1->quad);
    ass.bpatch(cond->trueList, marker_m2->quad);
    stat->nextList = ass.merge(cond->falseList, stat1->breakList);
    ass.emitCode(JUMP + marker_m1->quad);
    regsPool.unbind(cond->place);
    delete cond;
    return stat;
}

Node* rule_Statement__BREAK_SC()
{
    //MARK: PROBABLY DONE
    if (!tableStack.back().isWhile) {
        output::errorUnexpectedBreak(yylineno);
        exit(0);
    }
    Node* stat = new Node();
    stat->breakList = ass.makelist(ass.emitCode(JUMP));
    return stat;
}

/*****************************************/
/* Call Rules */
/*****************************************/

Expression* rule_Call__ID_ExpList(Variable *var, ExprList *expList)
{
    //MARK: DONE
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
    Expression* exp = new Expression(funcData->retType);
    vector<int> usedRegistersIndices;
    vector<string> usedRegisters = regsPool.getUsedRegisters();
    vector<pair<string, int> > argsPlaces = getPlacesFromList(expList->v);
    //regsPool.unbindAll(usedRegisters);
    ass.emitFunctionCall(usedRegisters, funcData->name, argsPlaces);
    //regsPool.bindAll(usedRegisters);
    for (int i = 0; i < argsPlaces.size(); i++) {
        regsPool.unbind(string(argsPlaces[i].first));
    }
    //DBUG("rule_Call__ID_ExpList")
    exp->place = FUNC_RES_REG;
    
    return exp;
}

Expression* rule_Call__ID(Variable *var) {
    //MARK: DONE
    FuncTableEntry *funcData = funcLookup(tableStack, var);
    if (NULL == funcData) {
        output::errorUndefFunc(yylineno, var->id);
        exit(0);
    }
    if (funcData->paramTypes.size() != 0)
    {
        vector<string> *strs = typeListToStringVector(funcData->paramTypes);
        output::errorPrototypeMismatch(yylineno, var->id, *strs);
        exit(0);
    }
    
    Expression* exp = new Expression(funcData->retType);
    vector<string> usedRegisters = regsPool.getUsedRegisters();
    vector<pair<string, int> > empty;
    regsPool.unbindAll(usedRegisters);
    ass.emitFunctionCall(usedRegisters, funcData->name, empty);
    regsPool.bindAll(usedRegisters);
    exp->place = FUNC_RES_REG;
    
    return exp;
}

/*****************************************/
/* Expression Rules */
/*****************************************/

Expression* rule_Exp__ID(Variable *var)
{
    //MARK: DONE
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
    regsPool.bind(regName);
    res->place = regName;
    ass.emitLoadVar(-1*entry->offset*WORD_SIZE, regName, isArrType(entry->type));
    if (entry->type == M_BOOL) {
        res->trueList = ass.makelist(ass.emitCode("    beq " + regName + ", " + "1, "));
        res->falseList = ass.makelist(ass.emitCode(JUMP));
    }
    return res;
}

Expression* rule_Exp__ID_Exp(Variable *var, Expression *exp)
{
    //MARK: DONE
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
    regsPool.bind(regName);
    res->place = regName;
    
    int size = 0;
    string sizeReg = regsPool.getEmptyRegister();
    stringstream ss;
    size = ((ArrTableEntry*)entry)->size;
    ss << size;
    ass.emitCode("    li " + sizeReg + ", " + ss.str());
    
    ass.emitLoadArrElem(-1*entry->offset*WORD_SIZE, exp->place, regName, sizeReg);
    regsPool.unbind(exp->place);
    delete exp;
    
    if (type == M_BOOL) {
        res->trueList = ass.makelist(ass.emitCode("    beq " + regName + ", " + "1, "));
        res->falseList = ass.makelist(ass.emitCode(JUMP));
    }
    
    return res;
}

Expression* rule_Exp__Call(Expression* exp) {
    string regName = regsPool.getEmptyRegister();
    //DBUG("rule_Exp__Call")
    ass.emitCode("    move " + regName + ", $v0");
    if(exp->type == M_BOOL){
        exp->trueList = ass.makelist(ass.emitCode("    beq " + regName + ", " + "1, "));
        exp->falseList = ass.makelist(ass.emitCode(JUMP));
    }
    regsPool.bind(regName);
    exp->place = regName;
    return exp;
}

Expression* rule_Exp__Exp_BINOP_Exp(Expression *exp1, string binop, Expression *exp2)
{
    //MARK: DONE
    if (!IS_NUM_TYPE(exp2) || !IS_NUM_TYPE(exp1)) {
        output::errorMismatch(yylineno);
        exit(0);
    }
    TypeId exp_type = M_BYTE;
    if (exp2->type == M_INT || exp1->type == M_INT)
        exp_type = M_INT;
    
    Expression* exp = new Expression(exp_type);
    bool need_mask = (exp_type == M_BYTE);
    exp->place = exp1->place;
    regsPool.unbind(exp1->place);
    regsPool.bind(exp->place);
    ass.emitBinOp(binop, exp->place, exp1->place, exp2->place, need_mask);
    regsPool.unbind(exp2->place);
    delete exp1;
    delete exp2;
    
    return exp;
}

Expression* rule_Exp__NUM(Expression *num) {
    //MARK: DONE
    string reg = regsPool.getEmptyRegister();
    if (reg == not_found) {/*???*/}
    regsPool.bind(reg);
    num->place = reg;
    ass.emitLoadConst(reg, num->value);
    return num;
}

Expression* rule_Exp__STRING(Expression *str) {
    //MARK: DONE
    ass.addStringLiteral(str->value);
    return str;
}

Expression* rule_Exp__TRUE() {
    //MARK: DONE
    Expression* exp = new Expression(M_BOOL);
    exp->place = regsPool.getEmptyRegister();
    regsPool.bind(exp->place);
    exp->trueList = ass.makelist(ass.emitCode(JUMP));
    return exp;
}

Expression* rule_Exp__FALSE() {
    //MARK: DONE
    Expression* exp = new Expression(M_BOOL);
    exp->place = regsPool.getEmptyRegister();
    regsPool.bind(exp->place);
    exp->falseList = ass.makelist(ass.emitCode(JUMP));
    return exp;
}

Expression* rule_Exp__NOT_Exp(Expression *exp1)
{
    //MARK: DONE
    if (exp1->type != M_BOOL) {
        output::errorMismatch(yylineno);
        exit(0);
    }
    Expression* exp = new Expression(M_BOOL);
    exp->place = exp1->place;
    exp->trueList = exp1->falseList;
    exp->falseList = exp1->trueList;
    delete exp1;
    
    return exp;
}

Expression* rule_Exp__Exp_AND_Exp(Expression *exp1, Node* marker, Expression *exp2)
{
    //MARK: DONE
    if (exp1->type != M_BOOL ||
        exp2->type != M_BOOL) {
        output::errorMismatch(yylineno);
        exit(0);
    }
    Expression* exp = new Expression(M_BOOL);
    ass.bpatch(exp1->trueList, marker->quad);
    exp->trueList = exp2->trueList;
    exp->falseList = ass.merge(exp1->falseList, exp2->falseList);
    exp->place = exp1->place;
    regsPool.unbind(exp2->place);
    delete exp1;
    delete exp2;
    
    return exp;
}
Expression* rule_Exp__Exp_OR_Exp(Expression *exp1, Node* marker,Expression *exp2)
{
    //MARK: DONE
    if (exp1->type != M_BOOL ||
        exp2->type != M_BOOL) {
        output::errorMismatch(yylineno);
        exit(0);
    }
    Expression* exp = new Expression(M_BOOL);
    ass.bpatch(exp1->falseList, marker->quad);
    exp->trueList = ass.merge(exp1->trueList, exp2->trueList);
    exp->falseList = exp2->falseList;
    exp->place = exp1->place;
    regsPool.unbind(exp2->place);
    delete exp1;
    delete exp2;
    
    return exp;
}


Expression* rule_Exp__Exp_RELOP_Exp(Expression *exp1, string relop, Expression *exp2)
{
    //MARK: DONE
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
    regsPool.unbind(exp2->place);
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


ExprList* rule_ExprList(ExprList *l, Expression *e) {
    if (e->type == M_BOOL) handleBoolExp(e);
    l->v.push_back(e);
    return l;
}


