%{
    #include "attributes.hpp"
    #include "parser.tab.hpp"
    #include "output.hpp"
%}

%option yylineno
%option noyywrap

%%
void                                {yyval = (*Node) new Type(TypeID::VOID, 0); return VOID;}
int                                 {yyval = (*Node) new Type(TypeId::INT, 4); return INT;}
byte                                {yyval = (*Node) new Type(TypeID::BYTE, 1); return BYTE;}
b                                   {return B;}
bool                                {yyval = (*Node) new Type(TypeID::BOOL, 1); return BOOL;}
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
[\<\>\<=\>]|==|!=                   {return RELOP;}
[\+\-\*\/]                          {return BINOP;}
[a-zA-Z][a-zA-Z0-9]*                {yyval = (*Node) new Id(yytext); return ID;}
0|[1-9][0-9]*                       {yyval = (*Node) new NumVal(yytext); return NUM;}
\"([^\n\r\"\\]|\\[rnt"\\])+\"       {return STRING;}
[ \t\n\r]|(\n\r)|(\r\n)|([\n\r])    ; // Whitespace, Newline, Tabs
\/\/[^\r\n]*[ \r|\n|\r\n]?          ; // Comment
.                                   { output::errorLex(yylineno); exit(0); }
%%
