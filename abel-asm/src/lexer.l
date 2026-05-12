%{
#include <string>
#include "ast.hpp"
#include "parser.tab.hpp"

extern "C" int yylex();
%}

%option noyywrap
%option yylineno

%%

[ \t]+              { /* skip whitespace */ }
;[^\n]*             { /* skip comments */ }
\n                  { return NEWLINE; }

"section"           { return SECTION; }
".data"             { return DOT_DATA; }
".text"             { return DOT_TEXT; }
"EQU"               { return EQU; }
"function"          { return FUNCTION; }
"end"               { return END; }
"var"               { return VAR; }
"push"              { return PUSH; }
"pop"               { return POP; }
"add"               { return ADD; }
"load"              { return LOAD; }
"store"             { return STORE; }
"call"              { return CALL; }
"ret"               { return RET; }

0[xX][0-9a-fA-F]+  { yylval.ival = std::stoull(yytext, nullptr, 16); return INTEGER; }
[0-9]+              { yylval.ival = std::stoull(yytext, nullptr, 10); return INTEGER; }

@[a-zA-Z_][a-zA-Z0-9_]* { yylval.sval = strdup(yytext + 1); return SLOT_REF; }
[a-zA-Z_][a-zA-Z0-9_]*  { yylval.sval = strdup(yytext); return IDENTIFIER; }

"("                 { return LPAREN; }
")"                 { return RPAREN; }
":"                 { return COLON; }
","                 { return COMMA; }

.                   { /* skip unknown chars */ }

%%
