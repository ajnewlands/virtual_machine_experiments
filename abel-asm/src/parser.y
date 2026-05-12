%{
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include <vector>
#include "ast.hpp"

extern "C" int yylex();
extern int yylineno;
void yyerror(const char *s);

Program g_program;

static Function *g_current_func = nullptr;
static std::vector<std::string> g_temp_names;
%}

%union {
	uint64_t ival;
	char *sval;
}

%token <ival> INTEGER
%token <sval> IDENTIFIER SLOT_REF
%token SECTION DOT_DATA DOT_TEXT EQU
%token FUNCTION END VAR
%token PUSH POP ADD LOAD STORE CALL RET
%token LPAREN RPAREN COLON COMMA NEWLINE

%%

program:
	lines
	;

lines:
	/* empty */
	| lines line
	;

line:
	NEWLINE
	| constant NEWLINE
	| section_decl NEWLINE
	| func_start NEWLINE
	| var_decl NEWLINE
	| instruction NEWLINE
	| END NEWLINE
	  {
	    if (g_current_func) {
	      g_program.functions.push_back(*g_current_func);
	      delete g_current_func;
	      g_current_func = nullptr;
	    }
	  }
	| END
	  {
	    if (g_current_func) {
	      g_program.functions.push_back(*g_current_func);
	      delete g_current_func;
	      g_current_func = nullptr;
	    }
	  }
	;

constant:
	IDENTIFIER EQU INTEGER
	  {
	    Constant c;
	    c.name = $1;
	    c.value = $3;
	    g_program.constants.push_back(c);
	    free($1);
	  }
	;

section_decl:
	SECTION DOT_DATA
	| SECTION DOT_TEXT
	;

func_start:
	FUNCTION IDENTIFIER LPAREN param_list RPAREN COLON
	  {
	    g_current_func = new Function();
	    g_current_func->name = $2;
	    g_current_func->args = g_temp_names;
	    g_temp_names.clear();
	    free($2);
	  }
	;

param_list:
	/* empty */
	| param_names
	;

param_names:
	IDENTIFIER
	  {
	    g_temp_names.push_back($1);
	    free($1);
	  }
	| param_names COMMA IDENTIFIER
	  {
	    g_temp_names.push_back($3);
	    free($3);
	  }
	;

var_decl:
	VAR var_names
	  {
	    if (g_current_func) {
	      for (auto &v : g_temp_names)
	        g_current_func->vars.push_back(v);
	      g_temp_names.clear();
	    }
	  }
	;

var_names:
	IDENTIFIER
	  {
	    g_temp_names.push_back($1);
	    free($1);
	  }
	| var_names COMMA IDENTIFIER
	  {
	    g_temp_names.push_back($3);
	    free($3);
	  }
	;

instruction:
	PUSH INTEGER
	  {
	    if (g_current_func) {
	      Instruction i;
	      i.type = InstrType::PUSH;
	      i.operand = std::to_string($2);
	      g_current_func->instructions.push_back(i);
	    }
	  }
	| PUSH IDENTIFIER
	  {
	    if (g_current_func) {
	      Instruction i;
	      i.type = InstrType::PUSH;
	      i.operand = $2;
	      g_current_func->instructions.push_back(i);
	    }
	    free($2);
	  }
	| POP
	  {
	    if (g_current_func) {
	      Instruction i;
	      i.type = InstrType::POP;
	      g_current_func->instructions.push_back(i);
	    }
	  }
	| ADD
	  {
	    if (g_current_func) {
	      Instruction i;
	      i.type = InstrType::ADD;
	      g_current_func->instructions.push_back(i);
	    }
	  }
	| LOAD SLOT_REF
	  {
	    if (g_current_func) {
	      Instruction i;
	      i.type = InstrType::LOAD;
	      i.operand = $2;
	      g_current_func->instructions.push_back(i);
	    }
	    free($2);
	  }
	| STORE SLOT_REF
	  {
	    if (g_current_func) {
	      Instruction i;
	      i.type = InstrType::STORE;
	      i.operand = $2;
	      g_current_func->instructions.push_back(i);
	    }
	    free($2);
	  }
	| CALL IDENTIFIER
	  {
	    if (g_current_func) {
	      Instruction i;
	      i.type = InstrType::CALL;
	      i.operand = $2;
	      g_current_func->instructions.push_back(i);
	    }
	    free($2);
	  }
	| RET
	  {
	    if (g_current_func) {
	      Instruction i;
	      i.type = InstrType::RET;
	      g_current_func->instructions.push_back(i);
	    }
	  }
	;

%%

void yyerror(const char *s) {
	fprintf(stderr, "Parse error at line %d: %s\n", yylineno, s);
}
