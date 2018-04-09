%{

/* Declarations section */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
    
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
    X(COMA),            \
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
    X(ERROR),           \

    
#define X(s)  s
typedef enum {
    TOKENS_TABLE
} Token;
#undef X
    
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

void showToken(Token);
void handleEscSeq(char*,char*);


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
%%

\"                              { buff_ptr = &buff; BEGIN(double_string); }
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
<double_string>([^\"\\])*       strcpy(buff_ptr, yytext); buff_ptr += yyleng;
<double_string>{escapeSeq}      handleEscSeq(yytext, buff_ptr);
<double_string>\"               { showToken(DSTRING); BEGIN(INITIAL); }
{sstring}                       showToken(STRING);
({digits}|{letters})+           showToken(VAL);
\&{letters}                     showToken(DECLARATION);
\*{letters}                     showToken(DEREFERENCE);
<<EOF>>                         { printf("1\n"); yyterminate(); }
{whitespace}                    ;
.                               showToken(ERROR);

%%

void handleEscSeq(char* text, char* buff_p) {
    char esc_seq = text[1];
#define X(s, ss) \
case s: \
    *buff_p++ = ss; \
    break;
switch(esc_seq)
{
ESCAPED_SEQS_TABLE
case 'x':
   *buff_p++ = strtol(text+2, NULL, 16);
   break;
}
#undef X
}
                     
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
    char *toPrint = yytext;
    switch(t)
    {
    case ERROR: // TODO: should handle errors here - might need different enum values for different errors. Can all errors be detected at this level?
        printf("Error %s\n", yytext);
        exit(0);
        
    case COMMENT:
        /* TODO: make sure it works on EOF! CR -> LF, EOF -> LF */
        // TODO should also make sure EOF works and not <<EOF>> - can use flex manual
        yytext[yyleng-1] = '\0';
        break;
        
    case INTEGER:
        num = (int)strtol(yytext, NULL, 0); //takes care of bases 10 and 16
        if (yyleng >= 2 && 'o' == yytext[1])
        {
            num = (int)strtol(yytext+2, NULL, 8);
        }
        printf("%d %s %d\n", yylineno, name, num);
        return;

    case DSTRING:
        toPrint = &buff;
        break;
                 
    case SSTRING:
        toPrint = &buff;
        break;
    default: ;
        
    };
    printf("%d %s %s\n", yylineno, name, toPrint);
}

