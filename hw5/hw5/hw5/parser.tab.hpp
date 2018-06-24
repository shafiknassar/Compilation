/* A Bison parser, made by GNU Bison 2.3.  */

/* Skeleton interface for Bison's Yacc-like parsers in C

   Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002, 2003, 2004, 2005, 2006
   Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     VOID = 258,
     INT = 259,
     BYTE = 260,
     BOOL = 261,
     RETURN = 262,
     IF = 263,
     WHILE = 264,
     BREAK = 265,
     LPAREN = 266,
     LBRACE = 267,
     LBRACK = 268,
     ASSIGN = 269,
     RBRACE = 270,
     SC = 271,
     NUM = 272,
     B = 273,
     FALSE = 274,
     TRUE = 275,
     STRING = 276,
     ID = 277,
     COMMA = 278,
     AND = 279,
     OR = 280,
     RELOP = 281,
     ADDSUB = 282,
     MULDIV = 283,
     NOT = 284,
     RBRACK = 285,
     RPAREN = 286,
     ELSE = 287
   };
#endif
/* Tokens.  */
#define VOID 258
#define INT 259
#define BYTE 260
#define BOOL 261
#define RETURN 262
#define IF 263
#define WHILE 264
#define BREAK 265
#define LPAREN 266
#define LBRACE 267
#define LBRACK 268
#define ASSIGN 269
#define RBRACE 270
#define SC 271
#define NUM 272
#define B 273
#define FALSE 274
#define TRUE 275
#define STRING 276
#define ID 277
#define COMMA 278
#define AND 279
#define OR 280
#define RELOP 281
#define ADDSUB 282
#define MULDIV 283
#define NOT 284
#define RBRACK 285
#define RPAREN 286
#define ELSE 287




#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef int YYSTYPE;
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif

extern YYSTYPE yylval;

