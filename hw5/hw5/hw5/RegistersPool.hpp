//
//  RegisterPool.hpp
//  hw5
//
//  Created by Bilal Tamish on 20/06/2018.
//  Copyright © 2018 ShafikNassar. All rights reserved.
//

#ifndef RegistersPool_hpp
#define RegistersPool_hpp

#include "includes.h"
#define not_found "not found"
#define REGISTERS_NUM 32
#define WORD_SIZE 4

using std::string;
using std::vector;
using std::map;

struct Register {
    string varName;
    bool inUse;
    
    Register() : varName(""), inUse(false) {}
    Register(string var, bool t): varName(var),inUse(t) {}
    
    void free();
};

struct MipsRegisters {
    // 0=>$zero; 1=>$at; 2,3=>$v0,$v1; 4-7=>$a0-$a3; 8-15=>$t0-$t7;
    // 16-23=>$s0-$s7; 24,25=>$t8,$t9; 26,27=>$k0,$k1;
    // 28=>$gp; 29=>$sp; 30=>$fp; 31=>$ra;
    map<string, Register> registers; //available registers
    int pc; //program counter
    
    MipsRegisters();
    void Bind(string registerName, string var);
    void unbind(string registerName);
    string getVarName(string registerName);
    string registerLookup(string varName);
    string getEmptyRegister();
    
};
#endif /* RegisterPool_hpp */
