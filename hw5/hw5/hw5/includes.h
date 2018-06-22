//
//  includes.h
//  hw5
//
//  Created by Bilal Tamish on 19/06/2018.
//  Copyright © 2018 ShafikNassar. All rights reserved.
//

#ifndef includes_h
#define includes_h

#define DBUG(...) \
    cout << "DEBUG: " << __VA_ARGS__ << endl; \

#include <stdio.h>
#include <sstream>
#include <stdlib.h>
#include <algorithm>
#include <vector>
#include <cassert>
#include <iostream>
#include <map>

#include "Program.hpp"
#include "SymbolTable.hpp"
#include "RegistersPool.hpp"
#include "output.hpp"
#include "bp.hpp"
#include "Assembler.hpp"
#include "parser.tab.hpp"
#include "SemanticActions.hpp"

extern YYSTYPE yylval;

#endif /* includes_h */
