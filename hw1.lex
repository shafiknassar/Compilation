%{

/* Declarations section */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
    
#define TOKENS_TABLE    \
    X(STARTSTRUCT),     \
    X(ENDSTRUCT),       \
    X(LLIST),           \
    X(RLIST),           \
    X(LDICT),           \
    X(RDICT),           \
    X(KEY),             \
    X(COMPLEXKEY),      \
    X(ITEM),            \
    X(COMA),            \
    X(TYPE),            \
    X(COMMENT),         \
    X(TRUE),            \
    X(FALSE),           \
    X(INTEGER),         \
    X(REAL),            \
    X(STRING),          \
    X(VAL),             \
    X(DECLARATION),     \
    X(DEREFERENCE),     \
    X(ERROR),           \

    
#define X(s)  s
typedef enum {
    TOKENS_TABLE
} Token;
#undef X
    
    
void showToken(Token);

%}

%option yylineno
%option noyywrap
printables      ([ -\~])+
digit   		([0-9])
digits          {digit}+
letter  		([a-zA-Z])
whitespace		([\t\n ])
letters         {letter}+
newLine         ([\n\r])|(\r\n)
wildcard        .*
sign            ([\+\-])
decimal         {sign}?{digits}
octadigits      ([0-7])+
hexadigits      ([0-9a-f])+
octa            (0o){octadigits}
hexa            (0x){hexadigits}
real            {sign}?({digits}\.{digits}?|{digits}?\.{digits})
exp             {real}e{sign}{digits}
sstring         \'([^\'])*\'
escapeSeqs      (\\)(\\|\"|a|b|e|f|n|r|t|v|0|(x{hexadigit}{hexadigit}))
dstring         \"(([^\"\\])|{escapeSeqs})*\"

%%

---                             showToken(STARTSTRUCT);
\.\.\.                          showToken(ENDSTRUCT);
\[                              showToken(LLIST);
\]                              showToken(RLIST);
\{                              showToken(LDICT);
\}                              showToken(RDICT);
:                               showToken(KEY);
\?                              showToken(COMPLEXKEY);
\-                              showToken(ITEM);
,                               showToken(COMA);
\!\!{letters}                   showToken(TYPE);
\#{wildcard}({newLine}|EOF)     showToken(COMMENT);
true                            showToken(TRUE);
false                           showToken(FALSE);
{decimal}|{octa}|{hexa}         showToken(INTEGER);
{real}|{exp}|\.inf|\.NaN        showToken(REAL);
{sstring}|{dstring}             showToken(STRING);
({digits}|{letters})+           showToken(VAL);
\&{letters}                     showToken(DECLARATION);
\*{letters}                     showToken(DEREFERENCE);
<<EOF>>                         { printf("1\n"); yyterminate(); }
{whitespace}                    ;
.                               printf("Lex doesn't know what that is!\n");

%%

                     
#define X(s)  #s
const char *tokenStrings[32] = {
    TOKENS_TABLE
};
#undef X

char* replace_char(char* str, char find, char replace){
    char *current_pos = strchr(str,find);
    while (current_pos){
        *current_pos = replace;
        current_pos = strchr(current_pos,find);
    }
    return str;
}
                     
void showToken(Token t)
{
    const char *name = tokenStrings[t];
    int num;
    switch(t)
    {
    case ERROR:
        printf("Error %s\n", yytext);
        exit(0);
        break;
        
    case COMMENT:
        /* TODO: make sure it works on EOF! CR -> LF, EOF -> LF */
        yytext[yyleng-1] = '\0';
        break;
        
    case INTEGER:
        num = (int)strtol(yytext, NULL, 0);
        if ('o' == yytext[1])
        {
            num = (int)strtol(yytext+2, NULL, 8);
        }
        printf("%d %s %d\n", yylineno, name, num);
        return;
        
    case STRING:
        yytext[yyleng-1] = '\0';
        yytext++;
        yytext = replace_char(yytext, '\n', ' ');
        yytext = replace_char(yytext, '\r', ' ');
        break;
    default: ;
        
    };
    printf("%d %s %s\n", yylineno, name, yytext);
}

