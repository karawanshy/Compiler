#ifndef QUAD_H_
#define QUAD_H_

#include <iostream>
#include "quad2.h"

#define DEFAULT_QUAD_TABLE_SIZE 500
#define DEFAULT_VARIABLE_TABLE_SIZE 100

#define INPUT_INST 0
#define OUTPUT_INST 1

/* quad operations */
enum quad_op {
  IASN,
  IPRT,
  IINP,
  IEQL,
  INQL,
  ILSS,
  IGRT,
  IADD,
  ISUB,
  IMLT,
  IDIV,
  RASN,
  RPRT,
  RINP,
  REQL,
  RNQL,
  RLSS,
  RGRT,
  RADD,
  RSUB,
  RMLT,
  RDIV,
  ITOR,
  RTOI,
  JUMP,
  JMPZ,
  HALT
};

//quad instructon struct
typedef struct _quad{
  enum quad_op op;
  quad_arg arg1;
  quad_arg arg2;
  quad_arg arg3;

  int line_num;
} *quad;


//takes a quad argument and adds it to the variables table with tyoe -1 (invalid), 
//and prints an error if the variable has already been declared
void add_id(quad_arg q, int line_num);
//searchs for the given variable in the variables table and returnes if it has been declared or not
int search_id(quad_arg q);
//fills the type of all the variables in the table that their type is -1 to the given type t. 
//called at the end of a decleration
void fill_id_type(enum quad_arg_type t);
//if update is 1: sets the arg type according its type in the table, 
//if update is 0: updates the arg type in the table to its actual type (the given arg type)
void set_id_type(quad_arg id, int update, int line_num);

//initializes the variables decleration table and store it in 'variables' global variable, 
//according to the given size (giving -1 initializes the table with DEFAULT_VARIABLE_TABLE_SIZE)
void init_variable_table(int size);
//expands the variables table with double the space it has now
void expand_var_table();
//freeing the variables table
void free_var_table();

//initializes the quad instructions table and store it in quad_table global variable, 
//according to the given size (giving -1 initializes the table with DEFAULT_QUAD_TABLE_SIZE)
void init_quad_table(int size);
//expands the quad table with double the space it has now
void expand_quad_table();
//freeing the quad  instructions table
void free_quad_table();
//freeing one quad (instruction)
void free_quad(quad arg);

//allocates a first node in a linked list, if is_num is 1, it allocates a name too, if its 0 it just add the addr to the node 
//in other words for list in list1 it just add the addr, and for list in list2 it adds the addr and the name (for cases lists)
node* makelist(int addr, char num[], int is_num);
//returnes the merged list of the given two lists (links the first one to the second (if not NULL))
node* merge(node *a,node *b);
//freeing list
void free_list(node* list);

//creates a quad argument. Value field must be set manually
quad_arg allocate_quad_arg();
//allocates name, if b is 1 we convert num_name to string and return it (used for patching)
//and if its 0 we allocate space and we copy to it the id_num
char* allocate_name(char id_name[], int num_name, int b);
//creates a new variable name temp%d and returns it (%d is the global variable next_temp)
quad_arg create_temp(enum quad_arg_type type);


//generates a quad in the quad table and adds to it the given parameters
void emit(enum quad_op op, quad_arg arg1, quad_arg arg2, quad_arg result);
//prints an error with the given line number and massege
void mark_error(int ln, const char *msg);


//puts the given addr in the result cell of all the quads in the table that their addresses in the given list
void backpatch(node* list, int addr);
//generates instruction of type binary (ADDOP, MULOP)
quad_arg generate_binary_instr(quad_arg arg1, enum math_op op, quad_arg arg2);
//generates instruction of type comparison (RELOP)
void generate_binary_comparison_instr(quad_arg arg1, enum math_op op, quad_arg arg2);
//generates cast instruction (static_cast) and prints an error if we're trying to cast float to int
quad_arg generate_cast_instr(enum math_op op, quad_arg arg, int line_number);
//generates an assign instruction and prints an error if we're trying to assign float to int
void generate_assign_instr(quad_arg arg1, quad_arg arg2, int line_num);
//generates input or output instruction depending on the given inst
void generate_input_output_instr(int inst, quad_arg arg);
//generates a switch instruction, def_addr is for knowing the address of the default instruction (because its not in the cases list)
void generate_switch_test_instr(quad_arg arg, node* list, int def_addr);

//writing the quad code to the file
void print_quad_table_to_file(FILE* file);

#endif