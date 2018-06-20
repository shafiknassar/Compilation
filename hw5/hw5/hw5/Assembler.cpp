//
//  Assembler.cpp
//  hw5
//
//  Created by Shafik Nassar on 20/06/2018.
//  Copyright © 2018 ShafikNassar. All rights reserved.
//

#include "Assembler.hpp"
#include "includes.h"

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

void Assembler::emitBinOp(string op, string trgPlace, string src1Place, string src2Place) {
    string mipsOp;
    if (op == ADD_OP) {
        mipsOp = "add ";
    } else if (op == SUB_OP) {
        mipsOp = "sub ";
    } else if (op == MUL_OP) {
        mipsOp = "mul ";
    } else if (op == DIV_OP) {
        codeBuff.emit("beq " + src2Place + ", 0, div_by_zero_handler");
        mipsOp = "add ";
    }
    codeBuff.emit(mipsOp + trgPlace + ", " + src1Place + ", " + src2Place);
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
