%{

/* Declarations section */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
    
#define CHAR_DEL     0x7f
    
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

//    X('\','\\')             \
    
    
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
octadigit       ([0-7])
hexadigit       ([0-9a-f])
octadigits      {octadigit}+
hexadigits      {hexadigit}+
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
.                               showToken(ERROR);

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
    char *pChar;
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
        if ('o' == yytext[1])
        {
            num = (int)strtol(yytext+2, NULL, 8);
        }
        printf("%d %s %d\n", yylineno, name, num);
        return;
        
    case STRING:
        yytext[yyleng-1] = '\0';
        
        /*maybe the next 2 lines need to be done only for dstring*/
        yytext = replace_char(yytext, '\n', ' ');
        yytext = replace_char(yytext, '\r', ' ');
            
        if ('"' == yytext[0]) // is double-quote string
        {
            pChar = strchr(yytext,'\\');
            while (pChar)
            {
                //assert (pChar - yytext < yyleng - 1);
                switch (*(pChar+1))
                { // handle escape sequences
#define X(s, ss)   \
case s: \
pChar[0] = CHAR_DEL; \
pChar[1] = ss; \
break;

                ESCAPED_SEQS_TABLE
                
#undef X
                case '\\':
                    pChar[0] = CHAR_DEL;
                    break;
                case 'x':
                    pChar[1] = pChar[2];
                    pChar[2] = pChar[3];
                    pChar[3] = '$'; //any char that is not a digit - to terminate strtol after 2 chars
                    pChar[0] = strtol(pChar+1, NULL, 16);
                    pChar[1] = CHAR_DEL;
                    pChar[2] = CHAR_DEL;
                    pChar[3] = CHAR_DEL;
                    break;
                default: break; //TODO: handle error (unknown sequence)
                }
                pChar = strchr(pChar+1,'\\');
            }
        }
        yytext++;
        break;
    default: ;
        
    };
    printf("%d %s %s\n", yylineno, name, yytext);
}

