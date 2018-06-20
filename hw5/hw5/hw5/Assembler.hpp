//
//  Assembler.hpp
//  hw5
//
//  Created by Shafik Nassar on 20/06/2018.
//  Copyright © 2018 ShafikNassar. All rights reserved.
//

#ifndef Assembler_hpp
#define Assembler_hpp

#include "bp.hpp"
#include <stdio.h>

using std::string;

class Assembler {
private:
    CodeBuffer codeBuff;
public:
    int emitCode(string code);
    
};

#endif /* Assembler_hpp */
