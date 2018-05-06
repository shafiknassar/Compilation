%{

/* Declarations section */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tokens.h"
    
    
#define CHAR_DEL        0x7f
#define MAX_FILE_SIZE   1024
    
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
    X(COMMA),           \
    X(TYPE),            \
    X(COMMENT),         \
    X(TRUE),            \
    X(FALSE),           \
    X(INTEGER),         \
    X(REAL),            \
    X(DSTRING),         \
    X(SSTRING),         \
    X(VAL),             \
    X(DECLARATION),     \
    X(DEREFERENCE),     \
    X(STRING),          \

/*
#define X(s)  s
typedef enum {
    TOKENS_TABLE
} Token_priv;
#undef X
 */
    
typedef enum tokens tokens;
    
#define ESCAPED_SEQS_TABLE  \
    X('a','\a')             \
    X('b','\b')             \
    X('e','\e')             \
    X('n','\n')             \
    X('f','\f')             \
    X('r','\r')             \
    X('t','\t')             \
    X('v','\v')             \
    X('0','\0')             \
    X('"','\"')             \
    X('\\','\\')


char buff[MAX_FILE_SIZE+1];
char *buff_ptr;

tokens showToken(tokens t);
void handleEscSeq(char*,char*);


%}

%option yylineno
%option noyywrap
printables      ([ -\~])+
digit   		([0-9])
digits          {digit}+
letter  		([a-zA-Z])
whitespace		([\t\n\r ])
letters         {letter}+
newLine         ([\n\r])|(\r\n)
wildcard        .*
sign            ([\+\-])
decimal         {sign}?{digits}
octadigit       ([0-7])
hexadigit       ([0-9a-fA-F])
octadigits      {octadigit}+
hexadigits      {hexadigit}+
octa            (0o){octadigits}
hexa            (0x){hexadigits}
real            {sign}?({digits}\.{digits}?|{digits}?\.{digits})
exp             {real}e{sign}{digits}
sstring         \'([^\'])*\'
escapeSeq       (\\)(\\|\"|a|b|e|f|n|r|t|v|0|(x{hexadigit}{hexadigit}))
dstring         (([^\"\\])|{escapeSeqs})*

%x double_string
%x single_string
%x comment
%%

\"                              { buff_ptr = buff; BEGIN(double_string); }
\'                              { buff_ptr = buff; BEGIN(single_string); }
#                               { buff_ptr = buff; BEGIN(comment);       }
---                             printf("lexer: %d\n", STARTSTRUCT); return (STARTSTRUCT);
\.\.\.                          printf("lexer: %d\n", ENDSTRUCT);return (ENDSTRUCT);
\[                              printf("lexer: %d\n", LLIST);return (LLIST);
\]                              printf("lexer: %d\n", RLIST);return (RLIST);
\{                              printf("lexer: %d\n", LDICT);return (LDICT);
\}                              printf("lexer: %d\n", RDICT);return (RDICT);
:                               printf("lexer: %d\n", KEY);return (KEY);
\?                              printf("lexer: %d\n", COMPLEXKEY);return (COMPLEXKEY);
\-                              printf("lexer: %d\n", ITEM);return (ITEM);
,                               printf("lexer: %d\n", COMMA);return (COMMA);
\!\!{letters}                   printf("lexer: %d\n", TYPE);return (TYPE);
true                            printf("lexer: %d\n", TRUE);return (TRUE);
false                           printf("lexer: %d\n", FALSE);return (FALSE);
{decimal}|{octa}|{hexa}         printf("lexer: %d\n", INTEGER);return (INTEGER);
{real}|{exp}|\.inf|\.NaN        printf("lexer: %d\n", REAL);return (REAL);
<double_string>([^\"\\\n])*     strcpy(buff_ptr, yytext); buff_ptr += yyleng;
<double_string>{escapeSeq}      handleEscSeq(yytext, buff_ptr); *(++buff_ptr) = '\0';
<double_string>"\n"             *(buff_ptr) = ' '; *(++buff_ptr) = '\0';
<double_string>\"               { BEGIN(INITIAL); return (STRING);}
<double_string>"\\".            printf("Error undefined escape sequence %s\n", yytext+1); exit(0);
<single_string>[^\']*           { buff_ptr = buff; strcpy(buff_ptr, yytext); return (STRING); }
<single_string>\'               BEGIN(INITIAL);
<single_string,double_string><<EOF>> printf("Error unclosed string\n"); exit(0);
<comment>([^\n\r])*             /*showToken(COMMENT)*/;
<comment><<EOF>>                BEGIN(INITIAL);
<comment>{newLine}              BEGIN(INITIAL);
{letters}({digits}|{letters})*  printf("lexer: %d\n", VAL);return (VAL);
\&{letters}                     printf("lexer: %d\n", DECLARATION);return (DECLARATION);
\*{letters}                     printf("lexer: %d\n", DEREFERENCE);return (DEREFERENCE);
<<EOF>>                         printf("%d EOF \n", yylineno); yyterminate();
{whitespace}                    ;
.                               printf("Error %s\n", yytext); exit(0);

%%


void handleEscSeq(char* text, char* buff_p) {
    char esc_seq = text[1];
#define X(s, ss) \
case s: \
    *buff_p = ss; \
    break;
    switch(esc_seq)
    {
        ESCAPED_SEQS_TABLE
    case 'x':
        *buff_p = strtol(text+2, NULL, 16);
        break;
}
#undef X
}
                     
#define X(s)  #s
const char *tokenStrings[32] = {
    TOKENS_TABLE
};
#undef X
         
  
                
                
tokens showToken(tokens t)
{
    const char *name = tokenStrings[t];
    int num;
    char *toPrint = yytext;
    switch(t)
    {
    /*case COMMENT:
        printf("%d %s #%s\n", yylineno, name, toPrint);
        return t;
     */
        
    case INTEGER:
        num = (int)strtol(yytext, NULL, 0); //takes care of bases 10 and 16
        if (yyleng >= 2 && 'o' == yytext[1])
        {
            num = (int)strtol(yytext+2, NULL, 8);
        }
        printf("%d %s %d\n", yylineno, name, num);
        return t;

    case STRING:
        toPrint = buff;
        name = tokenStrings[STRING];
        break;

    default: ;
        
    };
    printf("%d %s %s\n", yylineno, name, toPrint);
    return t;
}

