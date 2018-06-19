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
     B = 261,
     BOOL = 262,
     MINUS = 263,
     PLUS = 264,
     DIV = 265,
     MULT = 266,
     AND = 267,
     OR = 268,
     NOT = 269,
     TRUE = 270,
     FALSE = 271,
     NUM = 272,
     STRING = 273,
     RPAREN = 274,
     LPAREN = 275,
     RBRACE = 276,
     LBRACE = 277,
     RBRACK = 278,
     LBRACK = 279,
     IF = 280,
     ELSE = 281,
     WHILE = 282,
     BREAK = 283,
     RETURN = 284,
     RELOP = 285,
     BINOP = 286,
     ASSIGN = 287,
     ID = 288,
     SC = 289,
     COMMA = 290
   };
#endif
/* Tokens.  */
#define VOID 258
#define INT 259
#define BYTE 260
#define B 261
#define BOOL 262
#define MINUS 263
#define PLUS 264
#define DIV 265
#define MULT 266
#define AND 267
#define OR 268
#define NOT 269
#define TRUE 270
#define FALSE 271
#define NUM 272
#define STRING 273
#define RPAREN 274
#define LPAREN 275
#define RBRACE 276
#define LBRACE 277
#define RBRACK 278
#define LBRACK 279
#define IF 280
#define ELSE 281
#define WHILE 282
#define BREAK 283
#define RETURN 284
#define RELOP 285
#define BINOP 286
#define ASSIGN 287
#define ID 288
#define SC 289
#define COMMA 290




#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef int YYSTYPE;
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif

extern YYSTYPE yylval;

