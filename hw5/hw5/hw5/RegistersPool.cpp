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
using std::map;
using std::pair;

MipsRegisters::MipsRegisters() : rtoi() {
    string reg_names[] = {"$t0", "$t1", "$t2", "$t3",
                          "$t4", "$t5", "$t6", "$t7",
                          "$s0", "$s1", "$s2", "$s3",
                          "$s4", "$s5", "$s6", "$s7",
                          "$t8", "$t9"
                         };
    for (int i = 0; i < AVAILABLE_REGISTERS_NUM; i++) {
        rtoi[reg_names[i]] = i;
        registers.push_back(Register(reg_names[i]));
    }
}

void MipsRegisters::bind(string registerName) {
    for (int i = 0; i < AVAILABLE_REGISTERS_NUM; i++) {
        if(registers[i].regName == registerName) registers[i].inUse = true;
    }
}

void MipsRegisters::bindAll(vector<string>& registersNames) {
    for (int i = 0; i < registersNames.size(); i++) {
        int j = rtoi[registersNames[i]];
        registers[j].inUse = true;
    }
}

void MipsRegisters::unbind(string registerName) {
    for (int i = 0; i < AVAILABLE_REGISTERS_NUM; i++) {
        Register& reg = registers[i];
        if(reg.regName == registerName) reg.inUse = false;
    }
}

void MipsRegisters::unbindAll(vector<string>& registersNames) {
    for (int i = 0; i < registersNames.size(); i++) {
        int j = rtoi[registersNames[i]];
        registers[j].inUse = false;
    }
}

void MipsRegisters::unbindAll() {
    for (int i = 0; i < registers.size(); i++) {
        registers[i].inUse = false;
    }
}

string MipsRegisters::getEmptyRegister() {
    //we can use: $t0-$t7, $s0-$s7, $t8-$t9
    for (int i = 0; i < AVAILABLE_REGISTERS_NUM; i++) {
        Register& reg = registers[i];
        if (!reg.inUse) return reg.regName;
    }
    return not_found;
}

vector<string> MipsRegisters::getUsedRegisters() {
    vector<string> used;
    for (int i = 0; i < AVAILABLE_REGISTERS_NUM; i++) {
        Register& reg = registers[i];
        if (reg.inUse) {
            used.push_back(reg.regName);
        }
    }
    return used;
}
