//
//  Assembler.cpp
//  hw5
//
//  Created by Shafik Nassar on 20/06/2018.
//  Copyright © 2018 ShafikNassar. All rights reserved.
//

#include "Assembler.hpp"
#include "includes.h"
using std::pair;

int Assembler::emitCode(string code) {
    return codeBuff.emit(code);
}

string Assembler::addStringLiteral(string value) {
    stringstream ss;
    ss << "str_" << stringDataCounter;
    string label = ss.str();
    ss << ": " << value;
    
    codeBuff.emitData(ss.str());
    
    return label;
}

string Assembler::getNextInst() {
    return codeBuff.genLabel();
}

void Assembler::emitLoadVar(int varOS, string regName, bool isArr) {
    stringstream ssVarOs;
    ssVarOs << varOS;
    if (isArr) { /* reference semantics */
        //reg <- fp + varOS
        codeBuff.emit("move " + regName + ", $fp");
        codeBuff.emit("addu " + regName + ", " + regName + ", " + ssVarOs.str());
    } else {
        codeBuff.emit("lw " + regName + ", " + ssVarOs.str() + "($fp)");
    }
}

void Assembler::emitLoadArrElem(int arrOS, string idxRegName, string trgRegName) {
    codeBuff.emit("sll " + idxRegName + ", " + idxRegName + ", 2"); /* multiply by 4 */
    codeBuff.emit("add " + idxRegName + ", " + idxRegName + ", $fp");
    stringstream ssArrOs;
    ssArrOs << arrOS;
    codeBuff.emit("lw " + trgRegName + ", " + ssArrOs.str() + "("+idxRegName+")");
}

void Assembler::emitLoadConst(string regName, string val) {
    codeBuff.emit("li " + regName + ", " + val);
}

int Assembler::emitBinOp(string op, string trgPlace, string src1Place, string src2Place) {
    string mipsOp;
    int res = -1;
    if (op == ADD_OP) {
        mipsOp = "add ";
    } else if (op == SUB_OP) {
        mipsOp = "sub ";
    } else if (op == MUL_OP) {
        mipsOp = "mul ";
    } else if (op == DIV_OP) {
        res = codeBuff.emit("beq " + src2Place + ", 0, div_by_zero_handler");
        mipsOp = "add ";
    }
    int tmp = codeBuff.emit(mipsOp + trgPlace + ", " + src1Place + ", " + src2Place);
    res = (res == -1)? tmp : res;
    return res;
}


int Assembler::emitRelOp(string op, string src1Place, string src2Place) {
    map<string, string> trans;
    string arr[6][2] =
    {
        {"==", "beq "}, {"!=", "bne "}, {"<", "blt "},
        {">", "bgt "}, {"<=", "ble "}, {">=", "bge "}
    };
    for (int i = 0; i < 6; ++i) {
        trans.insert(pair<string, string>(arr[i][0], arr[i][1]));
    }
    string to_emit = trans[op];
    
    return codeBuff.emit(to_emit + src1Place + ", " + src2Place +", ");
};

void Assembler::emitFunctionReturn(string resRegName) {
    if (resRegName != "") {
        codeBuff.emit("move $v0, " + resRegName);
    }
    codeBuff.emit("move $sp,$fp");
    codeBuff.emit("sub $sp,$sp,4");
    codeBuff.emit("lw $fp,($sp)");
    codeBuff.emit("add $sp,$sp,4");
    codeBuff.emit("jr $ra");
}

void Assembler::allocateLocalVar(int size) {
    stringstream ssSize;
    ssSize << size;
    codeBuff.emit("sw $zero, ($sp)"); /* inits numerical values as 0, booleans as false */
    codeBuff.emit("addu $sp, $sp, " + ssSize.str());
}

void Assembler::allocateLocalArr(int numOfElems, int elemSize) {
    for (int i = 0; i < numOfElems; ++i) {
        allocateLocalVar(elemSize);
    }
}

void Assembler::assignArrToArr(int trgOs, string srcAddrsReg, int size, string tmpReg) {
    for (int i = 0; i < size; i++) {
        // lw $tmp, i*4($src)
        // sw $tmp, trgOf+i*4($fp)
        stringstream ss;
        ss << i*WORD_SIZE;
        codeBuff.emit("lw " + tmpReg + ", " + ss.str() + "("+srcAddrsReg+")");
        ss.str(""); /* clear stream */
        ss << i*WORD_SIZE + trgOs;
        codeBuff.emit("sw " + tmpReg + ", " + ss.str() + "($fp)");
    }
}


void Assembler::assignValToVar(int varOS, string regName) {
    stringstream ssVarOs;
    ssVarOs << varOS;
    codeBuff.emit("sw " + regName + ", " + ssVarOs.str() + "($fp)");
}

void Assembler::assignValToArrElem(
                                   int arrOS,
                                   string idxRegName,
                                   string srcRegName)
{
    codeBuff.emit("sll " + idxRegName + ", " + idxRegName + ", 2"); /* multiply by 4 */
    codeBuff.emit("add " + idxRegName + ", " + idxRegName + ", $fp");
    stringstream ssArrOs;
    ssArrOs << arrOS;
    codeBuff.emit("sw " + srcRegName + ", " + ssArrOs.str() + "("+idxRegName+")");
}

/* CodeBuffer APIs */
void Assembler::bpatch(const vector<int>& address_list, const string &loc) {
    codeBuff.bpatch(address_list, loc);
}

vector<int> Assembler::makelist(int litem) {
    return CodeBuffer::makelist(litem);
}

vector<int> Assembler::merge(const vector<int> &l1,const vector<int> &l2) {
    return CodeBuffer::merge(l1, l2);
}

//print the content of the data buffer to stdout including a .data header
void Assembler::printDataBuffer() {
    codeBuff.printDataBuffer();
}

//print the content of the code buffer to stdout including a .text header
void Assembler::printCodeBuffer() {
    codeBuff.printCodeBuffer();
}

void Assembler::emitDivByZeroHandler() {
    codeBuff.emit("div_by_zero_handler:");
    
    codeBuff.emit("la $a0,str_0");
    codeBuff.emit("li $v0,4");
    codeBuff.emit("syscall");
    
    codeBuff.emit("la $a0,str_1");
    codeBuff.emit("li $v0,4");
    codeBuff.emit("syscall");
    
    codeBuff.emit("li $v0,10");
    codeBuff.emit("syscall");
    
}

void Assembler::emitPrinti() {
    codeBuff.emit("printi:");
    codeBuff.emit("lw $a0,4($sp)");
    codeBuff.emit("li $v0,1");
    codeBuff.emit("syscall");
    codeBuff.emit("jr $ra");
}


void Assembler::emitPrint() {
    codeBuff.emit("print:");
    codeBuff.emit("lw $a0,4($sp)");
    codeBuff.emit("li $v0,4");
    codeBuff.emit("syscall");
    codeBuff.emit("jr $ra");
}
