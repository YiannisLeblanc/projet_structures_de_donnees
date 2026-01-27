
#include <stdio.h>
#include <stdlib.h>

#include "program/program.h"
#include "program/lexical.h"

#include "program/parser.h"


void get_expr(t_expr_rpn *expr, const t_prog_token_list *list, unsigned int *i) {
    t_prog_token token = ptl_get(list, *i);
    if (token.token_type != PT_EXPR) {
        printf("Expression expected\n");
        *i = (unsigned int) (-1);
        return;
    }
    *expr = token.content.expr_rpn;
    (*i)++;
}

t_ast *parse_aux(const t_prog_token_list *list, unsigned int *i) {
    if (*i >= list->size)
        return NULL;

    t_ast *prog = malloc(sizeof(t_ast)); // Current node of the AST

    const t_prog_token token = ptl_get(list, *i);
    switch (token.token_type) {
        case PT_VAR: {
            t_assignment_statement st;
            st.var = token.content.var;
            (*i)++;
            if(ptl_get(list, *i).token_type!=Assignment){
                printf("Variable non suivie par une affectation !\n");
                return NULL;
            }
            (*i)++;
            get_expr(&st.expr, list, i);
            prog->command = Assignment;
            prog->statement.assignment_st = st;
            break;
        }
        case PT_KEYWORD: {
            switch (token.content.keyword) {
                case KW_PRINT: {
                    t_print_statement st;
                    (*i)++;
                    if(ptl_get(list, *i).token_type != PT_EXPR){
                        printf("Print n'est pas suivi d'une expression !\n");
                        return NULL;
                    }
                    (*i)++;
                    get_expr(&st.expr,list , i);
                    prog->command = Print;
                    prog->statement.print_st = st;
                    break;
                }
                case KW_RETURN: {
                    t_return_statement st;
                    (*i)++;
                    if(ptl_get(list, *i).token_type != PT_EXPR){
                        printf("Return n'est pa suivi d'une expression !\n");
                        return NULL;
                    }
                    (*i)++;
                    get_expr(&st.expr,list , i);
                    prog->command = Return;
                    prog->statement.return_st = st;
                    break;
                }
                case KW_IF: {
                    t_if_statement st;
                    st.if_true  = NULL;
                    st.if_false = NULL;
                    (*i)++;
                    if(ptl_get(list, *i).token_type != PT_EXPR){
                        printf("IF n'est pas suivi d'une expression !\n");
                        return NULL;
                    }
                    (*i)++;
                    get_expr(&st.cond,list , i);
                    st.if_true = parse_aux(list, i);
                    st.if_false = parse_aux(list, i);
                    prog->statement.if_st = st;
                    prog->command = If;
                    break;
                }
                case KW_WHILE: {
                    t_while_statement st;
                    st.block = NULL;
                    (*i)++;
                    if(ptl_get(list, *i).token_type != PT_EXPR){
                        printf("While n'est pas suivi d'une expression !\n");
                        return NULL;
                    }
                    (*i)++;
                    get_expr(&st.cond, list, i);
                    st.block = parse_aux(list, i);
                    prog->command = While;
                    prog->statement.while_st = st;
                    break;
                }
                case KW_ENDBLOCK:
                case KW_ELSE: {
                    (*i)++;
                    return NULL;
                }
                default:
                    printf("Syntax error: wrong keyword ");
                    print_keyword(token.content.keyword);
                    printf("\n");
                    break;
            }
            break;
        }
        default: {
            printf("Syntax error: wrong token type\n");
            break;
        }
    }
    if (*i == (unsigned int) (-1)) {
        //free(prog);
        return NULL;
    }
    prog->next = parse_aux(list, i);
    return prog;
}

t_ast *parse(const t_prog_token_list *list) {
    if (list->size == 0) return NULL;

    unsigned int i = 0; // index in the list
    return parse_aux(list, &i);
}