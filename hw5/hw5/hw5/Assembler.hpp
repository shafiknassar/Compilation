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
using std::vector;
using std::pair;

/***********************************/
/* defines */
/***********************************/

#define INC_SP "    addu $sp, $sp, 4"
#define DEC_SP "    sub $sp, $sp, 4"
#define JUMP      "    j "
#define ADD_OP    "+"
#define SUB_OP    "-"
#define MUL_OP    "*"
#define DIV_OP    "/"



class Assembler {
private:
    CodeBuffer &codeBuff;
    int stringDataCounter;
    
public:
    
    Assembler() :
        codeBuff(CodeBuffer::instance()),
        stringDataCounter(3) {}
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
    string getLastStringLiteral();
    
    /*
     @description: generates a label for the next intsruction
     @returns the label assigned.
     */
    string getNextInst();
    
    /*
     @description: generates code for handling division by zero exception
                   and index out of bounds.
     @returns none.
     */
    void emitDivByZeroHandler();
    void emitIndexOutOfBoundsHandler();
    
    void emitProgramInit();
    
    void emiFunctionHeader(string funName);
    
    void emitPrint();
    
    void emitPrinti();
    
    void emitLoadVar(int varOS, string regName, bool isArr);
    
    void emitLoadArrElem(int arrOS, string idxRegName, string trgRegName);
    
    void emitLoadConst(string regName, string val);
    
    void emitBool(string regName, bool value);
    
    void emitBeforCall(vector<string> usedRegs, string funcName,
                       vector<pair<string, int> > args);
    void emitAfterCall(vector<string> usedRegs, vector<pair<string, int> > args);
    void emitFunctionReturn(string funcName, string resRegName = "");
    void emitFunctionCall(vector<string> usedRegisters, string funcName,
                          vector<pair<string, int> > args);
    
    void allocateLocalVar(string regName = "");
    void allocateLocalArr(int numOfElems);
    
    void assignValToVar(int varOS, string regName);
    void assignValToArrElem(int arrOS, string arrSizeReg, string idxRegName, string trgRegName);
    void assignArrToArr(int srcOs, string trgOsReg, int size, string tmpReg);
    
    /*
     @description: generates code for binary/relational operation
     @returns none.
     */
    int emitBinOp(string op, string trgPlace, string src1Place, string src2Place, bool needMask);
    /*
     note: relational operation is trnaslated to a conditional jump.
     the target of the jump need to be backpatched.
     */
    int emitRelOp(string op, string src1Place, string src2Place);

    /* CodeBuffer APIs */
    void bpatch(const vector<int>& address_list, const string &loc);
    
    static vector<int> makelist(int litem);
    
    static vector<int> merge(const vector<int> &l1,const vector<int> &l2);

    //print the content of the data buffer to stdout including a .data header
    void printDataBuffer();
    
    //print the content of the code buffer to stdout including a .text header
    void printCodeBuffer();
};

#endif /* Assembler_hpp */
