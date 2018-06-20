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

MipsRegisters::MipsRegisters() {
    // 0=>$zero; 1=>$at; 2,3=>$v0,$v1; 4-7=>$a0-$a3; 8-15=>$t0-$t7;
    // 16-23=>$s0-$s7; 24,25=>$t8,$t9; 26,27=>$k0,$k1;
    // 28=>$gp; 29=>$sp; 30=>$fp; 31=>$ra;
    string reg_name[] = {"$zero", "$at", "$v0", "$v1", "$a0", "$a1", "$a2", "$a3",
                     "$t0", "$t1", "$t2", "$t3", "$t4", "$t5", "$t6", "$t7",
                     "$s0", "$s1", "$s2", "$s3", "$s4", "$s5", "$s6", "$s7",
                     "$t8", "$t9", "$k0", "$k1", "$gp", "$sp", "$fp", "$ra"};
    for (int i = 0; i < REGISTERS_NUM; i++) {
        registers[reg_name[i]] = Register();
    }
    pc = 4;
}

string MipsRegisters::getVarName(string registerName) {
    return registers[registerName].varName;
}

void MipsRegisters::bind(string registerName, string var) {
    // can't use $zero
    if (registerName == "$zero" || registers[registerName].inUse) return;
    Register& reg = registers[registerName];
    reg.varName = var;
    reg.inUse = true;
}

void MipsRegisters::unbind(string registerName) {
    Register& reg = registers[registerName];
    if (registerName == "$zero" || !reg.inUse) return;
    reg.free();
}

string MipsRegisters::registerLookup(string varName) {
    map<string, Register>::iterator it;
    for (it = registers.begin(); it != registers.end(); ++it) {
        Register& reg = (*it).second;
        if (reg.varName == varName) return (*it).first;
    }
    return not_found;
}

string MipsRegisters::getEmptyRegister() {
    //we can use: $t0-$t7, $s0-$s4, $t8-$t9
    map<string, Register>::iterator it;
    for (it = registers.begin(); it != registers.end(); ++it) {
        Register& reg = (*it).second;
        if (!reg.inUse) return (*it).first;
    }
    return not_found;
}
