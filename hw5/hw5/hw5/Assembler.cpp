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

void emitBinOp(string op, string trgPlace, string src1Place, string src2Place) {
    
}
