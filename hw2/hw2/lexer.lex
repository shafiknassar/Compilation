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
    X(VAL),             \
    X(DECLARATION),     \
    X(DEREFERENCE),     \
    X(STRING),          \

/*
#define X(s)  s
typedef enum {
    TOKENS_TABLE
} Token;
#undef X
*/
#define ESCAPED_SEQS_TABLE  \
    X('a','\a')            \
    X('b','\b')            \
    X('e','\e')            \
    X('n','\n')            \
    X('f','\f')            \
    X('r','\r')            \
    X('t','\t')             \
    X('v','\v')             \
    X('0','\0')             \
    X('"','\"')             \
    X('\\','\\')


char buff[MAX_FILE_SIZE+1];
char *buff_ptr;

void showToken(tokens);
void handleEscSeq(char*,char*);


%}

%option yylineno
%option noyywrap
printables      ([\x0-\x7E])+
nonprintables   ([\x0-\x1F\x7F-\xFF])+
digit   		([0-9])
digits          {digit}+
letter  		([a-zA-Z])
whitespace		([\t\n\r ])
letters         {letter}+
newLine         (\n\r)|(\r\n)|([\n\r])
newLineOneChar  ([\n\r])
newLineTwoChar  (\r\n)
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

\"                              { *buff = '\0'; buff_ptr = buff; BEGIN(double_string); }
\'                              { *buff = '\0'; buff_ptr = buff; BEGIN(single_string); }
#                               { *buff = '\0'; buff_ptr = buff; BEGIN(comment);       }
---                             return(STARTSTRUCT);
\.\.\.                          return(ENDSTRUCT);
\[                              return(LLIST);
\]                              return(RLIST);
\{                              return(LDICT);
\}                              return(RDICT);
:                               return(KEY);
\?                              return(COMPLEXKEY);
\-                              return(ITEM);
,                               return(COMMA);
\!\!{letters}                   return(TYPE);
true                            return(TRUE);
false                           return(FALSE);
{decimal}|{octa}|{hexa}         return(INTEGER);
{real}|{exp}|\.inf|\.NaN        return(REAL);
<double_string>{newLine}        {*(buff_ptr) = ' '; *(++buff_ptr) = '\0';}
<double_string>{nonprintables}*  printf("Error %s\n", yytext); exit(0);
<double_string>{escapeSeq}      handleEscSeq(yytext, buff_ptr); *(++buff_ptr) = '\0';
<double_string>"\\".            printf("Error undefined escape sequence %s\n", yytext+1); exit(0);
<double_string>\"               { BEGIN(INITIAL); return(STRING); }
<double_string>[^\"\\\n\r]*     strcpy(buff_ptr, yytext); buff_ptr += yyleng;

<single_string>[^\']*           strcpy(buff_ptr, yytext); buff_ptr += yyleng;
<single_string>\'               BEGIN(INITIAL); return(STRING);
<single_string,double_string><<EOF>> printf("Error unclosed string\n"); exit(0);
<comment>([^\n\r])*             /*return(COMMENT)*/;
<comment><<EOF>>                BEGIN(INITIAL);
<comment>{newLine}              BEGIN(INITIAL);
{letters}({digits}|{letters})*  return(VAL);
\&{letters}                     return(DECLARATION);
\*{letters}                     return(DEREFERENCE);
<<EOF>>                         return EF;
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
                     
void showToken(tokens t)
{
    const char *name = tokenStrings[t];
    long num;
    char *toPrint = yytext;
    switch(t)
    {
    /*case COMMENT:
        printf("%d %s #%s\n", yylineno, name, toPrint);
        return;
        */
    case INTEGER:
        if (yyleng >= 2 && 'o' == yytext[1])
        {
            num = (int)strtol(yytext+2, NULL, 8);
        } else if (yyleng >= 2 && 'x' == yytext[1]) {
            num = strtol(yytext, NULL, 16);
        } else {
            num = strtol(yytext, NULL, 10);
        }
        printf("%d %s %ld\n", yylineno, name, num);
        return;

    case STRING:
        toPrint = buff;
        name = tokenStrings[STRING];
        break;

    default: ;
        
    };
    printf("%d %s %s\n", yylineno, name, toPrint);

}

