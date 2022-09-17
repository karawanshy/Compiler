%{
#include <stdio.h>
#include <stdlib.h>
#include <cstring>
#include "quad1.h"

extern int yylex();
void yyerror(const char *s);

extern int line_number;
extern int nextinst;
extern enum quad_arg_type num_type;
    
%}

%union{
    quad_arg arg;
    enum math_op op;
    list1 l1;
    list2 l2;
    int val;
    enum quad_arg_type type;
    char id[100];
}

%token INT FLOAT IF ELSE WHILE SWITCH DEFAULT CASE BREAK INPUT OUTPUT
%token <id> NUM ID 
%token <op> RELOP ADDOP MULOP AND OR NOT CAST
%left RELOP ADDOP MULOP AND OR 

%type <arg> expression term factor idlist
%type <val> M
%type <type> type
%type <l1> boolexpr boolterm boolfactor
%type <l2> stmt if_stmt while_stmt switch_stmt stmt_block stmtlist break_stmt caselist N assignment_stmt input_stmt output_stmt
%nterm program declerations decleration   

%%

line: program;

program : declerations stmt_block M { backpatch($2.nextlist, $3);
                                      backpatch($2.breaks, $3);};

declerations : declerations decleration
	     | {};

decleration : idlist ':' type ';' { fill_id_type($3); } 
            ;

type : INT { $$ = INT_TYPE; }
     | FLOAT { $$ = FLOAT_TYPE; };

idlist : idlist ',' ID {    quad_arg tmp = allocate_quad_arg();
                            tmp->name = allocate_name($3, 0, 0); 
                            add_id(tmp, line_number); }
       | ID {   quad_arg tmp = allocate_quad_arg();
                tmp->name = allocate_name($1, 0, 0); 
                add_id(tmp, line_number);  }
       ;

stmt : assignment_stmt { $$ = $1; }
     | input_stmt      { $$ = $1; }
     | output_stmt     { $$ = $1; }
     | if_stmt         { $$ = $1; }
     | while_stmt      { $$ = $1; }
     | switch_stmt     { $$ = $1; }
     | break_stmt      { $$ = $1; }
     | stmt_block      { $$ = $1; };

assignment_stmt :   ID '=' expression ';' { quad_arg tmp = allocate_quad_arg();
                                            tmp->name = allocate_name($1, 0, 0); 
                                            set_id_type(tmp, 1, line_number);
                                            generate_assign_instr(tmp, $3, line_number);
                                            $$.nextlist = NULL;
                                            $$.breaks = NULL;};

input_stmt : INPUT '(' ID ')' ';' { quad_arg tmp = allocate_quad_arg();
                                    tmp->name = allocate_name($3, 0, 0); 
                                    set_id_type(tmp, 1, line_number);
                                    generate_input_output_instr(INPUT_INST, tmp); 
                                    $$.nextlist = NULL;
                                    $$.breaks = NULL;};

output_stmt : OUTPUT '(' expression ')' ';' { generate_input_output_instr(OUTPUT_INST, $3);
                                              $$.nextlist = NULL; 
                                              $$.breaks = NULL;};

if_stmt : IF '(' boolexpr ')' M stmt N ELSE M stmt { backpatch($3.falselist, $9);
                                                     node* tmp1 = merge($6.nextlist, $10.nextlist);
                                                     $$.nextlist = merge(tmp1, $7.nextlist); 
                                                     $$.breaks = merge($6.breaks, $10.breaks);};

while_stmt : WHILE M '(' boolexpr ')' M stmt {  backpatch($7.nextlist, $2);
                                                $$.nextlist = merge($4.falselist, $7.breaks);
                                                quad_arg tmp = allocate_quad_arg();
                                                tmp->name = allocate_name(NULL, $2 + 1, 1);
                                                emit(JUMP, NULL, NULL, tmp); };

switch_stmt : SWITCH '(' expression ')' N '{' caselist DEFAULT ':' M stmtlist '}' { node* tmp = merge($11.breaks, $11.nextlist);
                                                                                    $$.nextlist = merge($7.breaks, tmp);
                                                                                    backpatch($7.nextlist, $10);
                                                                                    backpatch($5.nextlist, nextinst);
                                                                                    generate_switch_test_instr($3, $7.cases, $10);};

caselist : caselist CASE NUM ':' M stmtlist { $$.breaks = merge($1.breaks, $6.breaks);
                                              backpatch($1.nextlist, $5);
                                              $$.nextlist = $6.nextlist;
                                              node *tmp = makelist($5, $3, 1);
                                              $$.cases = merge($1.cases, tmp);}
         | { $$.nextlist = NULL;
             $$.breaks = NULL;
             $$.cases = NULL;}

break_stmt : BREAK ';' {$$.breaks = makelist(nextinst, NULL, 0);
                        emit(JUMP, NULL, NULL, NULL);  
                        $$.nextlist = NULL;};

stmt_block : '{' stmtlist '}' { $$.nextlist = $2.nextlist;
                                $$.breaks = $2.breaks;};

stmtlist : stmtlist M stmt { backpatch($1.nextlist, $2);
                             $$.nextlist = $3.nextlist;
                             $$.breaks = $3.breaks;}
 	 | {$$.nextlist = NULL;
        $$.breaks = NULL;};

M : { $$ = nextinst; };

N : {   $$.nextlist = makelist(nextinst, NULL, 0);
        emit(JUMP, NULL, NULL, NULL); }

boolexpr : boolexpr OR M boolterm { backpatch($1.falselist, $3); 
                                    $$.truelist = merge($1.truelist, $4.truelist);
                                    $$.falselist = $4.falselist;}                   
	 | boolterm { $$ = $1; };

boolterm : boolterm AND M boolfactor {  $$.falselist = merge($1.falselist, $4.falselist); 
                                        $$.truelist = $4.truelist;}
 	 | boolfactor { $$ = $1; };

boolfactor : NOT '(' boolexpr ')' { $$.truelist = $3.falselist;
                                    $$.falselist = $3.truelist; }
	   | expression RELOP expression {  if($2 == LE_EQ || $2 == GE_EQ){
                                            $$.truelist = makelist(nextinst + 4, NULL, 0);
                                            $$.falselist = makelist(nextinst + 3, NULL, 0);
                                        }else{
                                            $$.truelist = makelist(nextinst + 2, NULL, 0);
                                            $$.falselist = makelist(nextinst + 1, NULL, 0);
                                        }
                                        generate_binary_comparison_instr($1, $2, $3);};
expression : expression ADDOP term  { $$ = generate_binary_instr($1, $2, $3); }
	   | term { $$ = $1; };

term : term MULOP factor { $$ = generate_binary_instr($1, $2, $3);}
     | factor { $$ = $1; };

factor : '(' expression ')' { $$ = $2; }
       | CAST '(' expression ')' { $$ = generate_cast_instr($1, $3, line_number); }
       | ID {   quad_arg tmp = allocate_quad_arg();
                tmp->name = allocate_name($1, 0, 0);
                set_id_type(tmp, 1, line_number); 
                $$ = tmp;}
       | NUM {  $$ = allocate_quad_arg();
                $$->name = allocate_name($1, 0, 0);
                $$->type = num_type; }

%%

void yyerror(const char *s){
    fprintf(stderr, "%d: %s\n", line_number, s);
}

