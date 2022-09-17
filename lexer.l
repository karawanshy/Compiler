%{
#include <string.h>
#include "quad2.h"
#include "parser.tab.h"

int line_number = 1;

enum quad_arg_type num_type;

%}

%option noyywrap
%x COMMENT
%x CAST_OP

white [ \t]+

letter [a-zA-Z]
digit [0-9]
id ({letter}|_)({letter}|{digit}|_)*
integer {digit}+
float {integer}"."{integer}?|"."{integer}

%%
"("                     return '(';
")"                     return ')';
"{"                     return '{';
"}"                     return '}';
"="                     return '=';
","                     return ',';
";"                     return ';';
":"                     return ':';
">="		            { yylval.op = GE_EQ; return RELOP;}
">"		                { yylval.op = GE; return RELOP;}
"<="		            { yylval.op = LE_EQ; return RELOP;}
"<"		                { yylval.op = LE; return RELOP;}
"=="		            { yylval.op = EQ; return RELOP;}
"!="		            { yylval.op = NEQ; return RELOP;}
"!"		                return NOT;
"&&"		            return AND;
"||"		            return OR;
"*"		                { yylval.op = MUL; return MULOP;}
"/"		                { yylval.op = DIV; return MULOP;}
"+"		                { yylval.op = PLUS; return ADDOP;}
"-"		                { yylval.op = MINUS; return ADDOP;}
"static_cast<int>"      { yylval.op = SC_INT; return CAST;}
"static_cast<float>"    { yylval.op = SC_FLOAT; return CAST;}

"break"	                return BREAK;
"case"		            return CASE;
"default"	            return DEFAULT;
"else"		            return ELSE;
"float"	                return FLOAT;
"if"		            return IF;
"input"	                return INPUT;
"int"		            return INT;
"output"	            return OUTPUT;
"switch"	            return SWITCH;
"while"	                return WHILE;

"static_cast"	        BEGIN(CAST_OP);

<CAST_OP>\<\i\n\t\>     { yylval.op = SC_INT; return CAST;}
<CAST_OP>\<\f\l\o\a\t\> { yylval.op = SC_FLOAT; return CAST;}

{integer}               { strcpy(yylval.id, yytext); num_type = INT_TYPE; return NUM;}

{float}                 { strcpy(yylval.id, yytext); num_type = FLOAT_TYPE; return NUM;}

{id}                    { strcpy(yylval.id, yytext); return ID;}

{white}                 /* skip whitespace, but we need to count line numbers */
\n                      line_number++; 

\/\*                    BEGIN(COMMENT);
<COMMENT>\*\/           BEGIN(INITIAL);
<COMMENT>.              /* skip comment text */
<COMMENT>\n             line_number++; /* eat up multiline comments */

.                       { printf ("%d: unrecognizeddd token %s\n", line_number, yytext); }

%%