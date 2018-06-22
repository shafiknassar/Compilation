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
    ss << ": .asciiz " << value;
    stringDataCounter++;
    
    codeBuff.emitData(ss.str());
    
    return label;
}

string Assembler::getLastStringLiteral() {
    stringstream ss;
    ss << "str_" << stringDataCounter-1;
    return ss.str();
}

string Assembler::getNextInst() {
    return codeBuff.genLabel();
}

void Assembler::emitLoadVar(int varOS, string regName, bool isArr) {
    stringstream ssVarOs;
    ssVarOs << varOS;
    if (isArr) { /* reference semantics */
        //reg <- fp + varOS
        codeBuff.emit("    move " + regName + ", $fp");
        codeBuff.emit("    addu " + regName + ", " + regName + ", " + ssVarOs.str());
    } else {
        codeBuff.emit("    lw " + regName + ", " + ssVarOs.str() + "($fp)");
    }
}

void Assembler::emitLoadArrElem(int arrOS, string idxRegName, string trgRegName) {
    codeBuff.emit("    sll " + idxRegName + ", " + idxRegName + ", 2"); /* multiply by 4 */
    codeBuff.emit("    add " + idxRegName + ", " + idxRegName + ", $fp");
    stringstream ssArrOs;
    ssArrOs << arrOS;
    codeBuff.emit("    lw " + trgRegName + ", " + ssArrOs.str() + "("+idxRegName+")");
}

void Assembler::emitLoadConst(string regName, string val) {
    codeBuff.emit("    li " + regName + ", " + val);
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
        res = codeBuff.emit("    beq " + src2Place + ", 0, div_by_zero_handler");
        mipsOp = "div ";
    }
    int tmp = codeBuff.emit("    " + mipsOp + trgPlace + ", " + src1Place + ", " + src2Place);
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
    
    return codeBuff.emit("    " + to_emit + src1Place + ", " + src2Place +", ");
};

void Assembler::emitFunctionReturn(string funcName, string resRegName) {
    if (resRegName != "") {
        codeBuff.emit("    move $v0, " + resRegName);
    }
    if (funcName == "main") {
        codeBuff.emit("    li $v0,10");
        codeBuff.emit("    syscall");
    } else {
        codeBuff.emit("    move $sp,$fp");
        codeBuff.emit("    sub $sp,$sp,4");
        codeBuff.emit("    lw $fp,($sp)");
        codeBuff.emit("    addu $sp,$sp,4");
        codeBuff.emit("    jr $ra");
    }
    codeBuff.emit(".end " + funcName + "\n");
}

void Assembler::emitBeforCall(vector<string> usedRegs, string funcName, vector<pair<string, int> > args) {
    //save used registers
    DBUG("usedRegs: " << usedRegs.size())
    for (int i = 0; i < usedRegs.size(); i++) {
        string regName = usedRegs[i];
        
        codeBuff.emit("    sub $sp,$sp,4");
        codeBuff.emit("    sw " + regName + ",0($sp)");
    }
    
    //save return addr ($ra)
    codeBuff.emit("    sub $sp, $sp, 4");
    codeBuff.emit("    sw $ra, 0($sp)");
    
    //print("str") take string as param
    DBUG("check if print")
    if(funcName == "print") {
        string last_label = getLastStringLiteral();
        codeBuff.emit("    la  $t0, " + last_label);
        codeBuff.emit("    sub $sp, $sp, 4");
        codeBuff.emit("    sw $t0, 0($sp)");
    } else {
        //pass args on stack
        DBUG("pass arg")
        for (int i = 0; i < args.size(); i++) {
            string reg = string(args[i].first);
            int size = args[i].second;
            DBUG("reg name: " << reg << " reg type size: " << size)
            //pass array by value!
            for (int j = 0; j < size; j++) {
                stringstream ss;
                ss << j*WORD_SIZE;
                DBUG(ss.str())
                codeBuff.emit("    lw $t0, " + ss.str() + "("+reg+")");
                codeBuff.emit("    sub $sp, $sp, 4");
                codeBuff.emit("    sw $t0, 0($sp)");
                ss.str("");
            }
        }
    }
    
}

void Assembler::emitAfterCall(vector<string> usedRegs, vector<pair<string, int> > args) {
    //clean args from stack
    for (int i = 0; i < args.size(); i++) {
        int size = args[i].second;
        for (int j = 0; j < size; j++) {
            codeBuff.emit("    addu $sp, $sp, 4");
        }
    }
    
    //restore return addr ($ra)
    codeBuff.emit("    lw $ra, 0($sp)");
    codeBuff.emit("    addu $sp, $sp, 4");
    
    //restore used registers
    for (int i = (int)usedRegs.size() - 1; i >= 0; --i) {
        string regName = usedRegs[i];
        codeBuff.emit("    lw " + regName + ",0($sp)");
        codeBuff.emit("    addu $sp, $sp, 4");
    }
}

void Assembler::emitFunctionCall(vector<string> usedRegs, string funcName,
                                 vector<pair<string, int> > args) {
    //save used registers, $ra and push args to stack
    DBUG("befor call")
    emitBeforCall(usedRegs, funcName, args);
    
    //call func
    codeBuff.emit("    jal " + funcName);
    
    //restore used registers, $ra and pop args from stack
    DBUG("after call")
    emitAfterCall(usedRegs, args);
    
}

void Assembler::allocateLocalVar(int size) {
    stringstream ssSize;
    ssSize << size;
    codeBuff.emit("    sw $zero, ($sp)"); /* inits numerical values as 0, booleans as false */
    codeBuff.emit("    addu $sp, $sp, " + ssSize.str());
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
        codeBuff.emit("    lw " + tmpReg + ", " + ss.str() + "("+srcAddrsReg+")");
        ss.str(""); /* clear stream */
        ss << i*WORD_SIZE + trgOs;
        codeBuff.emit("    sw " + tmpReg + ", " + ss.str() + "($fp)");
    }
}


void Assembler::assignValToVar(int varOS, string regName) {
    stringstream ssVarOs;
    ssVarOs << varOS;
    codeBuff.emit("    sw " + regName + ", " + ssVarOs.str() + "($fp)");
}

void Assembler::assignValToArrElem(int arrOS, string arrSizeReg,
                                   string idxRegName, string srcRegName)
{
    codeBuff.emit("    bge " + idxRegName + ", " + arrSizeReg + ", " + "index_out_of_bound");
    codeBuff.emit("    sll " + idxRegName + ", " + idxRegName + ", 2"); /* multiply by 4 */
    codeBuff.emit("    add " + idxRegName + ", " + idxRegName + ", $fp");
    stringstream ssArrOs;
    ssArrOs << arrOS;
    codeBuff.emit("    sw " + srcRegName + ", " + ssArrOs.str() + "("+idxRegName+")");
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
    
    codeBuff.emit("    la $a0,str_0");
    codeBuff.emit("    li $v0,4");
    codeBuff.emit("    syscall");
    
    codeBuff.emit("    la $a0,str_2");
    codeBuff.emit("    li $v0,4");
    codeBuff.emit("    syscall");
    
    codeBuff.emit("    li $v0,10");
    codeBuff.emit("    syscall");
    codeBuff.emit(".end div_by_zero_handler\n");
    
}

void Assembler::emitIndexOutOfBoundsHandler() {
    codeBuff.emit("index_out_of_bounds_handler:");
    
    codeBuff.emit("    la $a0,str_1");
    codeBuff.emit("    li $v0,4");
    codeBuff.emit("    syscall");
    
    codeBuff.emit("    la $a0,str_2");
    codeBuff.emit("    li $v0,4");
    codeBuff.emit("    syscall");
    
    codeBuff.emit("    li $v0,10");
    codeBuff.emit("    syscall");
    codeBuff.emit(".end index_out_of_bounds_handler\n");
}

void Assembler::emitPrinti() {
    codeBuff.emit("printi:");
    codeBuff.emit("    lw $a0,4($sp)");
    codeBuff.emit("    li $v0,1");
    codeBuff.emit("    syscall");
    codeBuff.emit("    jr $ra");
    codeBuff.emit(".end printi\n");
}


void Assembler::emitPrint() {
    codeBuff.emit("print:");
    codeBuff.emit("    lw $a0,4($sp)");
    codeBuff.emit("    li $v0,4");
    codeBuff.emit("    syscall");
    codeBuff.emit("    jr $ra");
    codeBuff.emit(".end print\n");
}

void Assembler::emiFunctionHeader(string funName) {
    codeBuff.emit(funName + string(":"));
    codeBuff.emit("    addu $sp, $sp, -4");
    codeBuff.emit("    sw   $fp, ($sp)");
    codeBuff.emit("    addu $fp, $sp, 4");
}

void Assembler::emitProgramInit() {
    codeBuff.emitData("str_0: .asciiz \"Error division by zero\"");
    codeBuff.emitData("str_1: .asciiz \"Error index out of bounds\"");
    codeBuff.emitData("str_2: .asciiz \"\\n\"");
    emitPrinti();
    emitPrint();
    emitDivByZeroHandler();
    emitIndexOutOfBoundsHandler();
}
