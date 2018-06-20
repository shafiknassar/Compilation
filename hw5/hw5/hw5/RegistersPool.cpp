//
//  RegisterPool.cpp
//  hw5
//
//  Created by Bilal Tamish on 20/06/2018.
//  Copyright © 2018 ShafikNassar. All rights reserved.
//

#include "includes.h"
using std::vector;
using std::string;

void Register::free() {
    varName = "";
    inUse = false;
}

Register* MipsRegisters::getValue(int registerName) {
    return registers[registerName];
}

void MipsRegisters::Bind(int registerName, string var) {
    // can't use $zero
    if (registerName == 0 || registers[registerName]->inUse) return;
    Register* reg = registers[registerName];
    reg->varName = var;
    reg->inUse = true;
}

void MipsRegisters::clearRegisters() {
    registers.clear();
    registers.resize(REGISTERS_NUM);
}

int MipsRegisters::registerLookup(string varName) {
    for (int i = 0 ; i < REGISTERS_NUM; i++) {
        if (registers[i]->varName == varName) return i;
    }
    return -1;
}

int MipsRegisters::getEmptyRegister() {
    //we can use: $t0-$t7, $s0-$s4, $t8-$t9
    for (int i =  8; i <= 25; i++) {
        if (!registers[i]->inUse) return i;
    }
    return -1;
}
