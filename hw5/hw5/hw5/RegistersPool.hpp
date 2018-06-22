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
#define AVAILABLE_REGISTERS_NUM 18
#define WORD_SIZE 4

using std::string;
using std::vector;
using std::map;
using std::make_pair;

struct Register {
    string regName;
    bool inUse;
    
    Register(string regName) : regName(regName), inUse(false) {}
    
    void free();
};

struct MipsRegisters {
    // 8-15=>$t0-$t7;
    // 16-23=>$s0-$s7; 24,25=>$t8,$t9;
    vector<Register> registers;//available registers
    
    MipsRegisters();
    void bind(string registerName);
    void bindAll(vector<string>& registersNames);
    void unbind(string registerName);
    void unbindAll(vector<string>& registersNames);
    string getVarName(string registerName);
    string getEmptyRegister();
    vector<string> getUsedRegisters();
};
#endif /* RegisterPool_hpp */
