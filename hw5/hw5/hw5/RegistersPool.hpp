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
/******************************/
/* defines */
/******************************/

#define not_found  "not found"
#define expression "expression"
#define AVLABLE_REGISTERS_NUM 18
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
    // 8-15=>$t0-$t7;
    // 16-23=>$s0-$s7; 24,25=>$t8,$t9;
    vector<string> regs_names;
    map<string, Register> registers; //available registers
    
    MipsRegisters();
    void bind(string registerName, string var);
    void unbind(string registerName);
    string getVarName(string registerName);
    string registerLookup(string varName);
    string getEmptyRegister();
    
};
#endif /* RegisterPool_hpp */
