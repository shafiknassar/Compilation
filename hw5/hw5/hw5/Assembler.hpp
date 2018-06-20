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
    CodeBuffer &codeBuff;
    int stringDataCounter;
public:
    
    Assembler() :
        codeBuff(CodeBuffer::instance()),
        stringDataCounter(0)
    {
        
    }
    /*
     @description: emits code to text section.
     @returns the location in code buffer.
     */
    int emitCode(string code);
    /* 
     @description: adds a string literal to the data section.
     @returns the label assigned to string.
     */
    string addStringLiteral(string value);
    
    /*
     @description: generates a label for the next intsruction
     @returns the label assigned.
     */
    string getNextInst();
    /*
     @description: generates code for handling division by zero exception
     @returns none.
     */
    void emitDivByZero(); /* TODO: implement */
    
    /*
     @description: generates code binary operation
     @returns none.
     */
    void emitBinOp(string op, string trgPlace, string src1Place, string src2Place);
};

#endif /* Assembler_hpp */
