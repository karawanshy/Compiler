#include <string.h>
#include <stdlib.h>
#include "quad1.h"

int quad_table_size;
int var_table_size;

int next_temp = 0; //counter of the next temp number (initialized with 0 because the first temp we are going to create is temp0)
int next_var; //counter of the variables in the variables table, variables[next_var] is the next available place for a variable
int nextinst; //counter of the instructions in the instructions table, quad_table[instr_count] is the next available space for an instruction
int error_in_file = 0; // 1 if there is at least one error of any kind found in the file, and 0 if none errors found yet

quad_arg* variables; //table of the declared variables and their types
quad* quad_table; //table of the generated quad instructions

//Adds the variable name to the variables list and put -1 as its type which means invalid
void add_id(quad_arg q, int line_num)
{
    //first of all check if the variable is already declared, if so, its an error
    for(int i=0; i<next_var; i++){
		if(!strcmp(variables[i]->name , q->name)){
            mark_error(line_num, "ID with the same name has already been declared!");
            error_in_file = 1;
            free(q->name);
            free(q);
            return;
        }
	}
    if (next_var >= var_table_size)
        expand_var_table();
    
    variables[next_var] = allocate_quad_arg();
    //allocating memory for the variable name
    variables[next_var]->name = (char*)malloc(strlen(q->name) + 1);
    //copying the given string to the variable name
    strcpy(variables[next_var]->name, q->name);
    //initializing the type of the variable to -1 (invalid)
	variables[next_var]->type = (enum quad_arg_type)-1;
    //updating the next available space for a new variable in the table
	next_var++;
}

//Searchs for the id in the array and returns true 
int search_id(quad_arg q)
{
	for(int i=0; i<next_var; i++)
	{
		if(!strcmp(variables[i]->name , q->name))
			return 1;
	}
	return 0;
}

//Sets the type t to all the variables that there type is -1 (invalid - yet to be updated)
void fill_id_type(enum quad_arg_type t)
{
   for(int i=0; i<next_var; i++)
	{
	    if(variables[i]->type == (enum quad_arg_type)-1)
		    variables[i]->type = t;
	}
}

//sets the given id type to its type in the variables table or the opposite according to update
void set_id_type(quad_arg id, int update, int line_num){
    bool b = true;

    for(int i=0; i<next_var; i++)
	{
	    if(!strcmp(variables[i]->name , id->name)){
            if(update)
		        id->type = variables[i]->type;
            else
                variables[i]->type = id->type;
            b = false;
        }
	}
    if(b){
        mark_error(line_num, "ID hasn't been declared!");
        error_in_file = 1;
    }
}

//initializes the variables table
void init_variable_table(int size)
{
  if (size > 0) {
    var_table_size = size;
  } else {
    var_table_size = DEFAULT_VARIABLE_TABLE_SIZE;
  }

  variables = (quad_arg*)calloc(var_table_size, sizeof(quad_arg));
  next_var = 0;
}

void expand_var_table()
{
  //allocate memory for the new variables_array
  var_table_size *= 2;
  quad_arg *new_var_table = (quad_arg*)calloc(var_table_size, sizeof(quad_arg));

  //copy over the old one to the new one
  for (int i = 0; i < next_var; i++) {
    new_var_table[i] = variables[i];
  }

  //free the old variables table
  free_var_table();

  variables = new_var_table;
}

void free_var_table(){
    for(int i=0; i< next_var; i++){
        free(variables[i]->name);
        free(variables[i]);
    }
    free(variables);
}

//initializes the quad instruction table
void init_quad_table(int size)
{
  if (size > 0) {
    quad_table_size = size;
  } else {
    quad_table_size = DEFAULT_QUAD_TABLE_SIZE;
  }

  quad_table = (quad*)calloc(quad_table_size, sizeof(quad));
  nextinst = 0;
}

void expand_quad_table()
{
  //allocate memory for the new quad_table
  quad_table_size *= 2;
  quad *new_quad_table = (quad*)calloc(quad_table_size, sizeof(quad));

  //copy over the old one to the new one
  for (int i = 0; i < nextinst; i++) {
    new_quad_table[i] = quad_table[i];
  }

  //free the old quad_table
  free_quad_table();

  quad_table = new_quad_table;
}

void free_quad_table(){
    for(int i=0; i< nextinst; i++){
        free_quad(quad_table[i]);
        free(quad_table[i]);
    }
    free(quad_table);
}

void free_quad(quad arg){

    if(arg->arg1 != NULL){
        if(arg->arg1->name != NULL){
            free(arg->arg1->name);
        }
        free(arg->arg1);
    }

    if(arg->arg2 != NULL){
        if(arg->arg2->name != NULL)
            free(arg->arg2->name);
        free(arg->arg2);
    }
    if(arg->arg3 != NULL){
        if(arg->arg3->name != NULL)
            free(arg->arg3->name);
        free(arg->arg3);
    }
}

node* makelist(int addr, char num[], int is_num){
    node *new_node = (node*)malloc(sizeof(node));

    //adding the addr to the list
    new_node->addr = addr;
    new_node->next = NULL;
    
    if(is_num){
        //creating quad arg and adding the given numas its name
        new_node->num = allocate_quad_arg();
        new_node->num->name = allocate_name(num, 0, 0);
    }else
        new_node->num = NULL;
    
    return new_node;
}

//returns the merged list of the two given lists
node* merge(node *a,node *b)
{
    node* curr = a;

    if(a == NULL)
        return b;
    
    if(b == NULL)
        return a;
        
    while(curr->next != NULL)
        curr = curr->next;

    curr->next = b;
    
    return a;
}

void free_list(node* list){
    node* curr ;
    while(list != NULL){
        curr = list;
        list = list->next;
        free(curr->num); //freeing the char* (argument name)
        free(curr);
    }
}

//creats and returns new quad argument
quad_arg allocate_quad_arg()
{
    quad_arg new_arg = (quad_arg)calloc(1, sizeof(struct _quad_arg));

    return new_arg;
}

//creates a name from a number or from an id depending on the giving b
char* allocate_name(char id_name[], int num_name, int b){
    char *tmp_name;
    if(b){
        //calculation the size we need to allocate for the argument name
        int size = snprintf(NULL, 0, "%d", num_name);
        //allocating space
        tmp_name = (char*)malloc(size + 1);
        //adding the addr as the argument name
        sprintf(tmp_name, "%d", num_name);
    }else{
        //allocating space
        tmp_name = (char*)malloc(strlen(id_name) + 1);
        //copying the given name to the new allocated one
        strcpy(tmp_name, id_name);
    }
    return tmp_name;
}


//creating new variables withe name temp%d
quad_arg create_temp(enum quad_arg_type type)
{
    quad_arg newtemp;
    int digits_counter_num = next_temp/ 10;
    int num_digits = 1; //we store in it how much digits we need after the word temp
    while (digits_counter_num != 0) { //updating the digit number
        num_digits++; //adding one more digit to the word
        digits_counter_num /= 10;
    }

    int tempname_len = num_digits + 4; //the temp variable length
    char *tempname = (char*)calloc(tempname_len + 1, sizeof('a')); //allocating space for the temp variable name
    snprintf(tempname, tempname_len + 1, "temp%d", next_temp); //creating the temp variable name
    
    newtemp = allocate_quad_arg();
    newtemp->name = tempname;
    newtemp->type = type;

    next_temp++;

    return newtemp;
}

//generates a new quad instruction in the quad table
void emit(enum quad_op op, quad_arg arg1, quad_arg arg2, quad_arg result){
    //if the quad table id full we expand it
    if (nextinst >= quad_table_size)
        expand_quad_table();

    quad_table[nextinst] = (quad)calloc(1, sizeof(struct _quad));
    quad_table[nextinst]->op = op;
    quad_table[nextinst]->arg1 = arg1;
    quad_table[nextinst]->arg2 = arg2;
    quad_table[nextinst]->arg3 = result;
    nextinst++;
}

//prints the given error to stderr
void mark_error(int ln, const char *msg) {
  fprintf(stderr, "%d: %s\n", ln, msg);
}

void print_quad_table_to_file(FILE* file){
    for(int i=0; i<nextinst;i++){
        switch (quad_table[i]->op)
        {
        case IASN:
            fprintf(file, "IASN %s %s\n", quad_table[i]->arg3->name, quad_table[i]->arg1->name);
            break;
        case IPRT:
            fprintf(file, "IPRT %s\n", quad_table[i]->arg3->name);
            break;
        case IINP:
            fprintf(file, "IINP %s\n", quad_table[i]->arg3->name);
            break;
        case IEQL:
            fprintf(file, "IEQL %s %s %s\n", quad_table[i]->arg3->name, quad_table[i]->arg1->name, quad_table[i]->arg2->name);
            break;
        case INQL:
            fprintf(file, "INQL %s %s %s\n", quad_table[i]->arg3->name, quad_table[i]->arg1->name, quad_table[i]->arg2->name);
            break;
        case ILSS:
            fprintf(file, "ILSS %s %s %s\n", quad_table[i]->arg3->name, quad_table[i]->arg1->name, quad_table[i]->arg2->name);
            break;
        case IGRT:
            fprintf(file, "IGRT %s %s %s\n", quad_table[i]->arg3->name, quad_table[i]->arg1->name, quad_table[i]->arg2->name);
            break;
        case IADD:
            fprintf(file, "IADD %s %s %s\n", quad_table[i]->arg3->name, quad_table[i]->arg1->name, quad_table[i]->arg2->name);
            break;
        case ISUB:
            fprintf(file, "ISUB %s %s %s\n", quad_table[i]->arg3->name, quad_table[i]->arg1->name, quad_table[i]->arg2->name);
            break;
        case IMLT:
            fprintf(file, "IMLT %s %s %s\n", quad_table[i]->arg3->name, quad_table[i]->arg1->name, quad_table[i]->arg2->name);
            break;
        case IDIV:
            fprintf(file, "IDIV %s %s %s\n", quad_table[i]->arg3->name, quad_table[i]->arg1->name, quad_table[i]->arg2->name);
            break;
        case RASN:
            fprintf(file, "RASN %s %s\n", quad_table[i]->arg3->name, quad_table[i]->arg1->name);
            break;
        case RPRT:
            fprintf(file, "RPRT %s\n", quad_table[i]->arg3->name);
            break;
        case RINP:
            fprintf(file, "RINP %s\n", quad_table[i]->arg3->name);
            break;
        case REQL:
            fprintf(file, "REQL %s %s %s\n", quad_table[i]->arg3->name, quad_table[i]->arg1->name, quad_table[i]->arg2->name);
            break;
        case RNQL:
            fprintf(file, "RNQL %s %s %s\n", quad_table[i]->arg3->name, quad_table[i]->arg1->name, quad_table[i]->arg2->name);
            break;
        case RLSS:
            fprintf(file, "RLSS %s %s %s\n", quad_table[i]->arg3->name, quad_table[i]->arg1->name, quad_table[i]->arg2->name);
            break;
        case RGRT:
            fprintf(file, "RGRT %s %s %s\n", quad_table[i]->arg3->name, quad_table[i]->arg1->name, quad_table[i]->arg2->name);
            break;
        case RADD:
            fprintf(file, "RADD %s %s %s\n", quad_table[i]->arg3->name, quad_table[i]->arg1->name, quad_table[i]->arg2->name);
            break;
        case RSUB:
            fprintf(file, "RSUB %s %s %s\n", quad_table[i]->arg3->name, quad_table[i]->arg1->name, quad_table[i]->arg2->name);
            break;
        case RMLT:
            fprintf(file, "RMLT %s %s %s\n", quad_table[i]->arg3->name, quad_table[i]->arg1->name, quad_table[i]->arg2->name);
            break;
        case RDIV:
            fprintf(file, "RDIV %s %s %s\n", quad_table[i]->arg3->name, quad_table[i]->arg1->name, quad_table[i]->arg2->name);
            break;
        case ITOR:
            fprintf(file, "ITOR %s %s\n", quad_table[i]->arg3->name, quad_table[i]->arg1->name);
            break;
        case RTOI:
            fprintf(file, "RTOI %s %s\n", quad_table[i]->arg3->name, quad_table[i]->arg1->name);
            break;
        case JUMP:
            fprintf(file, "JUMP %s\n", quad_table[i]->arg3->name);
            break;
        case JMPZ:
            fprintf(file, "JMPZ %s %s\n", quad_table[i]->arg3->name, quad_table[i]->arg1->name);
            break;
        default:
            break;
        }
    }
    fprintf(file, "HALT\n");
}
