#ifndef QUAD2_H_
#define QUAD2_H_

/* oprators */
enum math_op {PLUS, MINUS, MUL, DIV, NEQ, EQ, LE, GE, LE_EQ, GE_EQ, SC_INT, SC_FLOAT};

/* types of variables */
enum quad_arg_type {INT_TYPE, FLOAT_TYPE};

//struct for quad argument
typedef struct _quad_arg *quad_arg;
struct _quad_arg{
  char* name;
  enum quad_arg_type type;
};

typedef struct node {
    int addr;
    quad_arg num;
    struct node* next;
} node;


typedef struct list1{
    node* truelist;
    node* falselist;
}list1;

typedef struct list2{
    node* nextlist;
    node* breaks;
    node* cases;
}list2;

#endif

