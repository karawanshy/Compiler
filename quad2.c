#include <string.h>
#include "quad1.h"

extern quad* quad_table;
extern int error_in_file; // 1 if there is at least one error of any kind found in the file, and 0 if none errors found yet

//backpatching the addr to the quad instructions in the addresses in the given list
void backpatch(node* list, int addr){
    node* curr = list;

    while(curr != NULL){
        //creating a new empty quad argument
        quad_table[curr->addr]->arg3 = allocate_quad_arg();
        
        //creating a name of the given addr (addr + 1 because the table starts at0 but the instructions should start at 1)
        quad_table[curr->addr]->arg3->name = allocate_name(NULL, addr + 1, 1);

        curr = curr->next;
    }

    free_list(list);
}

// Generates code for the binary operation (ADDOP and MULOP)
quad_arg generate_binary_instr(quad_arg arg1, enum math_op op, quad_arg arg2)
{
    quad_arg result;

    if(arg1->type == INT_TYPE && arg2->type == INT_TYPE){
        result = create_temp(INT_TYPE);
        if(!error_in_file){
            if(op == PLUS)
              emit(IADD, arg1, arg2, result);
            if(op == MINUS)
                emit(ISUB, arg1, arg2, result);
            if(op == MUL)
                emit(IMLT, arg1, arg2, result);
            if(op == DIV)
                emit(IDIV, arg1, arg2, result);
        }
    }else{
        result = create_temp(FLOAT_TYPE);

        //casting the int variable to float
        if(arg1->type == INT_TYPE && arg2->type == FLOAT_TYPE){
            arg1->type = FLOAT_TYPE; //casting arg1 to float
        }else if(arg1->type == FLOAT_TYPE && arg2->type == INT_TYPE){
           arg2->type = FLOAT_TYPE; //casting arg2 to float
        }

        if(!error_in_file){
            if(op == PLUS){
                emit(RADD, arg1, arg2, result);}
            if(op == MINUS){
                emit(RSUB, arg1, arg2, result);}
            if(op == MUL){
                emit(RMLT, arg1, arg2, result);}
            if(op == DIV){
                emit(RDIV, arg1, arg2, result);}
        }

    }
    return result;
}

// Generates code for the binary comparison (RELOP)
void generate_binary_comparison_instr(quad_arg arg1, enum math_op op, quad_arg arg2)
{
    //result is always INT_TYPE regardless of the arguments types
    quad_arg result = create_temp(INT_TYPE);
    quad_arg t1;
    quad_arg t2;


    if(!error_in_file){
        if(arg1->type == INT_TYPE && arg2->type == INT_TYPE){
            switch (op){
            case EQ:
                emit(IEQL, arg1, arg2, result);
                break;
            case NEQ:
                emit(INQL, arg1, arg2, result);
                break;
            case GE:
                emit(IGRT, arg1, arg2, result);
                break;
            case LE:
                emit(ILSS, arg1, arg2, result);
                break;
            case GE_EQ: //generating the >= op to three instructions
                t1 = create_temp(INT_TYPE);
                t2 = create_temp(INT_TYPE);
                emit(IGRT, arg1, arg2, t1);
                emit(IEQL, arg1, arg2, t2);
                emit(IADD, t1, t2, result);
                break;
            case LE_EQ: //generating the <= op to three instructions
                t1 = create_temp(INT_TYPE);
                t2 = create_temp(INT_TYPE);
                emit(ILSS, arg1, arg2, t1);
                emit(IEQL, arg1, arg2, t2);
                emit(IADD, t1, t2, result);
                break;
            default:
                break;
            }
        }else{
            //casting the int variable to float
            if(arg1->type == INT_TYPE && arg2->type == FLOAT_TYPE){
                arg1->type = FLOAT_TYPE; //casting arg1 to float
            }else if(arg1->type == FLOAT_TYPE && arg2->type == INT_TYPE){
                arg2->type = FLOAT_TYPE; //casting arg2 to float
            }
            switch (op){
            case EQ:
                emit(REQL, arg1, arg2, result);
                break;
            case NEQ:
                emit(RNQL, arg1, arg2, result);
                break;
            case GE:
                emit(RGRT, arg1, arg2, result);
                break;
            case LE:
                emit(RLSS, arg1, arg2, result);
                break;
            case GE_EQ: //generating the >= op to three instructions
                t1 = create_temp(INT_TYPE);
                t2 = create_temp(INT_TYPE);
                emit(RGRT, arg1, arg2, t1);
                emit(REQL, arg1, arg2, t2);
                emit(RADD, t1, t2, result);
                break;
            case LE_EQ: //generating the <= op to three instructions
                t1 = create_temp(INT_TYPE);
                t2 = create_temp(INT_TYPE);
                emit(RLSS, arg1, arg2, t1);
                emit(REQL, arg1, arg2, t2);
                emit(RADD, t1, t2, result);
                break;
            default:
                break;
            }
        }
        emit(JMPZ, result, NULL, NULL);
    }else{ //because we still need to continue to fine if there is more errors
        //casting the int variable to float
        if(arg1->type == INT_TYPE && arg2->type == FLOAT_TYPE){
            arg1->type = FLOAT_TYPE; //casting arg1 to float
        }else if(arg1->type == FLOAT_TYPE && arg2->type == INT_TYPE){
            arg2->type = FLOAT_TYPE; //casting arg2 to float
        }
    }
}

// Generates code for the cast operation
quad_arg generate_cast_instr(enum math_op op, quad_arg arg, int line_num){

    quad_arg result;
    if(op == SC_INT){
        result = create_temp(INT_TYPE);
        if(!error_in_file)
            emit(RTOI, arg, NULL, result);
        //updating the variable type after casting
        arg->type = INT_TYPE;
        //updating the variable type in the variables table after casting
        set_id_type(arg, 0, line_num);
    }else{
        result = create_temp(FLOAT_TYPE);
        if(!error_in_file)
            emit(ITOR, arg, NULL, result);
        //updating the variable type after casting
        arg->type = FLOAT_TYPE;
        //updating the variable type in the variables table after casting
        set_id_type(arg, 0, line_num);
    }

    return result;
}

// Generates code for the assignment operation
void generate_assign_instr(quad_arg arg1, quad_arg arg2, int line_num){

    if(!search_id(arg1)){
        mark_error(line_num, "ID hasn't been declared!");
        error_in_file = 1;
    }

    if(arg1->type == INT_TYPE && arg2->type == INT_TYPE){ //if both sides int
        if(!error_in_file)
            emit(IASN, arg2, NULL, arg1);
    }else if((arg1->type == FLOAT_TYPE && arg2->type == FLOAT_TYPE) || (arg1->type == FLOAT_TYPE)){ //if both sides float, or the left side float and the right side int
        if(arg1->type == FLOAT_TYPE && arg2->type == INT_TYPE)
           arg2->type = FLOAT_TYPE; //casting arg2 to float

        if(!error_in_file)
            emit(RASN, arg2, NULL, arg1);
    }else{
        mark_error(line_num, "Cannot assign a float to an int!");
        error_in_file = 1;
    }
}

// Generates code for the input and output operation
void generate_input_output_instr(int inst, quad_arg arg){
    if(!error_in_file){
        if(inst == INPUT_INST){ //if its an input instruction
            if(arg->type == INT_TYPE)
                emit(IINP, NULL, NULL, arg);
            else
                emit(RINP, NULL, NULL, arg);
        }else{//if its an output instruction
            if(arg->type == INT_TYPE)
                emit(IPRT, NULL, NULL, arg);
            else
                emit(RPRT, NULL, NULL, arg);
        }
    }
}

// Generates code for the switch cases
void generate_switch_test_instr(quad_arg arg, node* list, int def_addr){
    node* curr = list;
    quad_arg result = create_temp(INT_TYPE);
    quad_arg tmp;

    while(curr != NULL){
        //creating a new empty quad argument
        tmp = allocate_quad_arg();
        //allocating and adding the addr as the new argument name
        tmp->name = allocate_name(NULL, curr->addr, 1);
        
        if(!error_in_file){
            emit(INQL, arg, curr->num, result);
            emit(JMPZ, result, NULL, tmp);
        }

        curr = curr->next;
    }
    if(!error_in_file){ //creating the default jump
        //creating a new empty quad argument
        tmp = allocate_quad_arg();
        //allocating and adding the addr as the new argument name
        tmp->name = allocate_name(NULL, def_addr, 1);
        
        if(!error_in_file){
            emit(JUMP, NULL, NULL, tmp);
        }
    }
    free_list(list);
}

