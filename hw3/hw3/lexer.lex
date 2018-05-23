%{
    #include "attributes.h"
    #include "parser.tab.hpp"
%}

%option yylineno
%option noyywrap

%%
void                                {return VOID;}
int                                 {return INT;}
byte                                {return BYTE;}
b                                   {return B;}
bool                                {return BOOL;}
and                                 {return AND;}
or                                  {return OR;}
not                                 {return NOT;}
true                                {return TRUE;}
false                               {return FALSE;}
return                              {return RETURN;}
if                                  {return IF;}
else                                {return ELSE;}
while                               {return WHILE;}
break                               {return BREAK;}
;                                   {return SC;}
,                                   {return COMMA;}
\(                                  {return LPAREN;}
\)                                  {return RPAREN;}
\{                                  {return LBRACE;}
\}                                  {return RBRACE;}
\[                                  {return LBRACK;}
\]                                  {return RBRACK;}
=                                   {return ASSIGN;}
==                                  {return EQ;}  /* check if backslash is necessary */
!=                                  {return NEQ;} /* check if backslash is necessary */
\<                                  {return LT;}
\>                                  {return GT;}
\<=                                 {return LE;}
\>=                                 {return GE;}
\+                                  {return PLUS;}
\-                                  {return MINUS;} /* check if backslash is necessary */
\*                                  {return MULT;}
\\                                  {return DIV;}
[a-zA-Z][a-zA-Z0-9]*                {return ID;}
0|[1-9][0-9]*                       {return NUM;}
\"([^\n\r\"\\]|\\[rnt"\\])+\"       {return STRING;}
[ \t\n\r]|(\n\r)|(\r\n)|([\n\r])    ; // Whitespace, Newline, Tabs
\/\/[^\r\n]*[ \r|\n|\r\n]?          ; // Comment
.                                   {return ERROR_LEX;}
%%
