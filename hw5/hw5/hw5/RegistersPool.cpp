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
    string reg_names[] = {"$t0", "$t1", "$t2", "$t3", "$t4", "$t5", "$t6", "$t7",
                          "$s0", "$s1", "$s2", "$s3", "$s4", "$s5", "$s6", "$s7",
                          "$t8", "$t9"
                         };
    
    for (int i = 0; i < AVLABLE_REGISTERS_NUM; i++) {
        regs_names.push_back(reg_names[i]);
        registers[regs_names[i]] = Register();
    }
}

string MipsRegisters::getVarName(string registerName) {
    return registers[registerName].varName;
}

void MipsRegisters::bind(string registerName, string var) {
    if (registers[registerName].inUse) return;
    Register& reg = registers[registerName];
    reg.varName = var;
    reg.inUse = true;
}

void MipsRegisters::unbind(string registerName) {
    Register& reg = registers[registerName];
    if (!reg.inUse) return;
    reg.free();
}

string MipsRegisters::registerLookup(string varName) {
    for (int i = 0; i < AVLABLE_REGISTERS_NUM; i++) {
        Register& reg = registers[regs_names[i]];
        if (reg.varName == varName) return regs_names[i];
    }
    return not_found;
}

string MipsRegisters::getEmptyRegister() {
    //we can use: $t0-$t7, $s0-$s7, $t8-$t9
    for (int i = 0; i < AVLABLE_REGISTERS_NUM; i++) {
        Register& reg = registers[regs_names[i]];
        if (!reg.inUse) return regs_names[i];
    }
    return not_found;
}
