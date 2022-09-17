#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "quad1.h"
#include "parser.tab.h"


extern quad *quad_table;
extern quad_arg *variables;
extern int error_in_file;

extern FILE *yyin;

char *input_file_name;
FILE *input_file = NULL;

int yyparse();

//checks if the fiven input file name is legal (ends with .ou)
int check_file_name(){
    char *s = strchr(input_file_name, '.');
    if (s != NULL)
        s++;

    return strcmp(s, "ou");
}

//creates an output file name that ends with .qud
char* create_output_file_name(){
    char* tmp = strtok(input_file_name, ".");
    strcat(tmp, ".qud");
    return tmp;
}

int main(int argc, char *argv[])
{

    if (argc == 1) {
        fprintf(stderr, "No input file name was given!\n");
        exit(1);
    } else if (argc > 2) {
        fprintf(stderr, "You should enter one file name!\n");
        exit(1);
    }
    
    input_file_name = argv[1];

    if(check_file_name()){
        fprintf(stderr, "The file name should end with .ou!\n");
        exit(1);
    }
    
    input_file = fopen(input_file_name, "r");
    if (!input_file) {
        fprintf(stderr, "Could'nt open the file: %s\n", input_file_name);
        exit(1);
    } 

    // Set yyin to parse from the input file
    yyin = input_file;

    init_quad_table(-1);
    init_variable_table(-1);

    yyparse();

    if(!error_in_file){
        char* output_file_name = create_output_file_name();
        FILE* output_file = fopen(output_file_name, "w");
        if (!output_file) {
            fprintf(stderr, "Could'nt open the file: %s\n", input_file_name);
            exit(1);
        }
        print_quad_table_to_file(output_file);
    }else{
        fprintf(stderr, "\nThere were errors found in the file, The compilation of the input file failed!\n");
        exit(1);
    }

  return 0;
}
