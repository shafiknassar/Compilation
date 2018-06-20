%{
    #include "includes.h"
%}

%option yylineno
%option noyywrap

%%
void                                {yylval = (Node*) new Type(M_VOID, 0); return VOID;}
int                                 {yylval = (Node*) new Type(M_INT, 1); return INT;}
byte                                {yylval = (Node*) new Type(M_BYTE, 1); return BYTE;}
b                                   {(Expression*)->type = N_BYTE; return B;}
bool                                {yylval = (Node*) new Type(M_BOOL, 1); return BOOL;}
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
(\=\=|\!\=|\<|\>|\<\=|\>\=)         {yylval = new Node(yytext); return RELOP;}
[\+\-\*\/]                          {return BINOP;}
[a-zA-Z][a-zA-Z0-9]*                {yylval = (Node*) new Variable(yytext); return ID;}
0|[1-9][0-9]*                       {yylval = (Node*) new Expression(M_INT, yytext); return NUM;}
\"([^\n\r\"\\]|\\[rnt"\\])+\"       {yylval = (Node*) new Expression(STRING, yytext); return STRING;}
[ \t\n\r]|(\n\r)|(\r\n)|([\n\r])    ; // Whitespace, Newline, Tabs
\/\/[^\r\n]*[ \r|\n|\r\n]?          ; // Comment
.                                   { output::errorLex(yylineno); exit(0); }
%%
