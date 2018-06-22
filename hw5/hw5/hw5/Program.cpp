#include "includes.h"

void FormList::add(Variable *id, Type *t) {
    idList.push_back(id);
    idList.back()->size = t->size;
}

void FormList::add(Variable *id) {
    idList.push_back(id);
}

bool FormList::redefined(Variable* id) {
    for (int i = 0; i < idList.size(); ++i) {
        if (id->id == idList[i]->id) return true;
    }
    return false;
}

int FormList::size() {
    return (int)idList.size();
}

TypeId convertFromArrType(TypeId arr_t) {
    switch (arr_t) {
        case INT_ARR:
            return M_INT;
        case BYTE_ARR:
            return M_BYTE;
        case BOOL_ARR:
            return M_BOOL;
        default:
            break;
    }
    return ERROR;
}

TypeId convertToArrType(TypeId arr_t) {
    switch (arr_t) {
        case M_INT:
            return INT_ARR;
        case M_BYTE:
            return BYTE_ARR;
        case M_BOOL:
            return BOOL_ARR;
        default:
            break;
    }
    return ERROR;
}

int typeSize(TypeId id)
{
    switch (id) {
        case M_INT:
            return 1;
        case M_VOID:
            return 0;
        case M_STRING: ;
        default: ;
    }
    return 1;
}

string etos(TypeId type)
{
    switch (type) {
        case M_INT: return string("INT");
        case M_VOID: return string("VOID");
        case M_BOOL: return string("BOOL");
        case M_STRING: return string("STRING");
        case M_BYTE: return string("BYTE");
        case INT_ARR: return string("IARR");
        case BYTE_ARR: return string("CARR");
        case BOOL_ARR: return string("BARR");
        case FUNC: return string("FUNC");
        case ERROR: return string("error");
    
        default: return string("ERROR");
    }
}

bool isArrType(TypeId t) {
    switch (t) {
        case INT_ARR:
        case BYTE_ARR:
        case BOOL_ARR:
            return true;
        default:
            break;
    }
    return false;
}
