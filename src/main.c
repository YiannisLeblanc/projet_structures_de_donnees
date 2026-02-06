#include <stdio.h>
#include <stdlib.h>
#include "file_io/file.h"
#include "program/lexer.h"
#include "program/parser.h"
#include "program/program.h"
#include "program/run.h"
#include "structures/prog_token_list.h"

//#define DEVELOPPEMENT

#ifdef DEVELOPPEMENT

void example() {

    // Source code
    /*
    a = 1
    if a
        print 2
    else
        print 3
    return a
    */
    char *code = "a = 1\nif a\n    print 2\nelse\n    print 3\nreturn a";

    // Token list
    // Creation of the tokens (of type t_prog_token)
    const char *s1 = "1";
    t_expr_rpn expr_1 = { .expr = parse_expr(&s1) };
    const char *s2 = "a";
    t_expr_rpn expr_2 = { .expr = parse_expr(&s2) };
    const char *s3 = "2";
    t_expr_rpn expr_3 = { .expr = parse_expr(&s3) };
    const char *s4 = "3";
    t_expr_rpn expr_4 = { .expr = parse_expr(&s4) };
    const char *s5 = "a";
    t_expr_rpn expr_5 = { .expr = parse_expr(&s5) };
#define LEN_TOKEN_LIST 13
    t_prog_token array[LEN_TOKEN_LIST] = {
        { PT_VAR, {.var = 'a'}},                // a
        { PT_KEYWORD, {.keyword = KW_ASSIGN}},  // =
        { PT_EXPR, {.expr_rpn = expr_1}},       // 1
        { PT_KEYWORD, {.keyword = KW_IF}},      // if
        { PT_EXPR, {.expr_rpn = expr_2}},       // a
        { PT_KEYWORD, {.keyword = KW_PRINT}},   // print
        { PT_EXPR, {.expr_rpn = expr_3}},       // 2
        { PT_KEYWORD, {.keyword = KW_ELSE}},    // else
        { PT_KEYWORD, {.keyword = KW_PRINT}},   // print
        { PT_EXPR, {.expr_rpn = expr_4}},       // 3
        {PT_KEYWORD, {.keyword = KW_ENDBLOCK}}, // (end-if)
        { PT_KEYWORD, {.keyword = KW_RETURN}},  // return
        { PT_EXPR, {.expr_rpn = expr_5}}        // a
    };

    // Creation of the actual t_prog_token_list
    t_prog_token_list token_list = ptl_create_empty_list();
    for (int i = 0; i < LEN_TOKEN_LIST; i++)
        ptl_push_back(&token_list, array[i]);

    // AST
    t_ast node_1;
    t_ast node_2;
    t_ast node_3;
    t_ast node_4;
    t_ast node_5;
    node_1.command = Assignment;
    node_1.statement = (u_statement) { .assignment_st = {.var = 'a', .expr = expr_1}};
    node_1.next = &node_2;
    node_2.command = If;
    node_2.statement = (u_statement) { .if_st = {.cond = expr_2, .if_true = &node_3, .if_false = &node_4 }};
    node_2.next = &node_5;
    node_3.command = Print;
    node_3.statement = (u_statement) { .print_st = { .expr = expr_3}};
    node_3.next = NULL;
    node_4.command = Print;
    node_4.statement = (u_statement) { .print_st = { .expr = expr_4}};
    node_4.next = NULL;
    node_5.command = Return;
    node_5.statement = (u_statement) { .return_st = {.expr = expr_5 }};
    node_5.next = NULL;

    //t_ast *prog_example = &node_1;

    t_ast* prog_example = parse(&token_list);

    // Checking that the AST is correctly drawn
    print_ast(prog_example, "../output/code_ex.mmd");

    // Execution of the program
    run(prog_example);
    /*
     Expected display:
        2
        -> 1
    */
}

int main(int argc, char** argv) {
    /*
    example();
    return EXIT_SUCCESS;
    */
    const char *file_name = "../code/monCode.txt";
    char *code = read_file(file_name);
    if (code == NULL)
        return EXIT_FAILURE;
    run_program(code);
    export_program_ast(code, file_name);

    free(code);
    return EXIT_SUCCESS;
}

#else

int main(int argc, char** argv){
    if(argc > 2){
        printf("ERROR too many arguments.\n");
        return EXIT_FAILURE;
    }

    if(argc == 1){
        printf("Usage: %s <filename>\n\n", argv[0]);
        printf("Arguments:\n");
        printf("  <filename>  Chemin vers le fichier de code à interpréter\n\n");
        printf("Example:\n");
        printf("  %s ./monCode.txt\n", argv[0]);
        return EXIT_SUCCESS;
    }

    const char *file_name;
    char *code = read_file(argv[1]);
    if (code == NULL)
        return EXIT_FAILURE;
    run_program(code);

    free(code);
    return EXIT_SUCCESS;
}

#endif