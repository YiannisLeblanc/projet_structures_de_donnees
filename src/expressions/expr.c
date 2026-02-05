#include "expressions/expr.h"
#include "structures/stack.h"
#include <stdio.h>
#include <stdlib.h>

// Returns true if the expression has no token left
bool is_empty_expr(const t_expr *expr) {
    return expr->list.size == 0;
}

// Adds a token at the back of the expression
void add_token(t_expr *expr, T token) {
    push_back(&expr->list, token);
}

// Returns the token at the front of the expression and deletes it from the expression 
T get_next_token(t_expr *expr) {
    T elt = expr->list.head->value;
    expr->list.head = expr->list.head->next;
    expr->list.size--;
    return elt;
}

// Prints the content of the expression
void print_expr(const t_expr *expr) {
    print_list(&expr->list);
}

void print_expr_file(FILE *file, const t_expr *expr) {
    print_list_file(file, &expr->list);
}

// Returns the int at the beginning of the string pointed at by p_s
// Moves p_s past the int
int parse_int(const char **p_s) {

    const char *s = *p_s;
    int n = (*s) - '0';
    s++;
    while (*s >= '0' && *s <= '9') {
        n = 10 * n + (*s - '0');
        s++;
    }
    *p_s = s - 1;
    return n;
}

// Converts the string s to an expression of type t_expr
t_expr parse_expr(const char **s) {

    t_expr expr;
    expr.list = create_empty_list();

    const char *p = *s;

    bool parsed_number = false;

    while (*p != '\0') {

        if (*p == ' ') {
            p++;
            continue;
        }

        t_expr_token token;
        if (*p >= 'a' && *p <= 'z') {
            token = token_of_variable(*p);
            parsed_number = true;
        }
        else if (*p >= '0' && *p <= '9') {
            int n = parse_int(&p);
            token = token_of_int(n);
            parsed_number = true;
        }
        // Unary minus
        else if (!parsed_number && *p == '-') {
            p++;
            if (*p >= '0' && *p <= '9') {
                int n = parse_int(&p);
                token = token_of_int(-n);
                parsed_number = true;
            }
            else {
                printf("parse_expr: wrong syntax (\"%s\")\n", p);
                exit(EXIT_FAILURE);
            }
        }
        // Operator
        else if (*p == '+' || *p == '-' || *p == '*' || *p == '/' || *p == '^') {
            token = token_of_operator(operator_of_char(*p));
            parsed_number = false;
        }
        else if (*p == '(' || *p == ')') {
            token = token_of_parenthesis(*p);
        }
        // CORRECTION du code de base, p++; à la fin du if "=" mangeait un caractère si < ou > seul.
        else if (*p == '<' || *p == '>') {
            if (*(p+1) == '=') {
                token = token_of_operator(*p == '<' ? LEQ : GEQ);
                p++;
            }
            else
                token = token_of_operator(*p == '<' ? LESS : GREATER);
        }
        else if ((*p == '!' || *p == '=') && *(p+1) == '=') {
            token = token_of_operator(*p == '=' ? EQUAL : DIFF);
            p++;
        }
        else {
            break;
        }

        add_token(&expr, token);
        p++;
    }

    *s = p;
    return expr;
}

void error_rpn() {
    printf("Error: expression is not in Reverse Polish notation\n");
    exit(EXIT_FAILURE);
}

// ADDED BY STUDENT
// Evaluates a sub-expression between parentheses recursively
int eval_parenthesis_recursive(const int var_table[], t_expr_rpn *expr_rpn) {
    t_expr_rpn sub_expr = {.expr.list = create_empty_list()};
    while (!is_empty_expr(&expr_rpn->expr)){
        t_expr_token token = get_next_token(&expr_rpn->expr);
        if(token.type == PARENTHESIS){
            if(!token.content.paren_type){
                return eval_rpn(var_table, &sub_expr);
            }else{
                T val = {.type = NUMBER, .content.val = eval_parenthesis_recursive(var_table, expr_rpn)};
                add_token(&sub_expr.expr, val);
            }
        }else{
            add_token(&sub_expr.expr, token);
        }
    }
    return 0;
}

/////////////////////////////////////////////////////////////////////

// Returns the value of the variable var
int look_up_variable(const int var_table[], const char var) {
    if(var >= 'a' && var <= 'z'){
        return var_table[var-'a'];
    }
    return 0; // TODO //END
}

// Returns the value of the token t (of type NUMBER or VARIABLE)
int get_value(const int var_table[], const t_expr_token *t) {
    switch(t->type){
        case VARIABLE:
        return look_up_variable(var_table, t->content.var);
        break;
        case NUMBER:
        return t->content.val;
        break;
        default: // EXCEPTION
        break;
    }
    return 0; // TODO //END
}

// Returns the result of the evaluation of the expression expr, in Reverse Polish notation
int eval_rpn(const int var_table[], const t_expr_rpn *expr_rpn) { //DONE

    t_list list_copy = expr_rpn->expr.list;
    t_expr expr = { .list = list_copy };

    t_stack stack = create_empty_stack();

    while (!is_empty_expr(&expr)) {
        t_expr_token token = get_next_token(&expr);
        
        switch (token.type) {
            case NUMBER:
            case VARIABLE:
                T value = {.type = NUMBER, .content.val = get_value(var_table, &token)};
                push(&stack, value);
                break;
            
            case OPERATOR: {
                int valB = pop(&stack).content.val;
                int valA = pop(&stack).content.val;
                T value = {.type = NUMBER, .content.val = apply_op(token.content.op, valA, valB)};
                push(&stack, value);
                break;
            }

            case PARENTHESIS:
                t_expr_rpn sub_expr_rpn = {.expr = expr};
                T value1 = {.type = NUMBER, .content.val = eval_parenthesis_recursive(var_table, &sub_expr_rpn)};
                push(&stack, value1);
                break;
        }
    }

    int result = get_top(&stack).content.val;

    destroy_stack(&stack);
    return result;
}

// Converts an expression in infix notation to Reverse Polish notation
t_expr_rpn shunting_yard(t_expr *expr) {

    t_expr_rpn expr_rpn;
    expr_rpn.expr.list = create_empty_list();
    t_expr *output = &expr_rpn.expr;

    t_stack op_stack = create_empty_stack();

    while (!is_empty_expr(expr)) {
        t_expr_token t = get_next_token(expr);
        switch (t.type) {
            case NUMBER:
            case VARIABLE:
                add_token(output, t);
                break;
            case OPERATOR:
                while (!is_empty_stack(&op_stack)) {
                    t_expr_token t2 = get_top(&op_stack);
                    if (t2.type != OPERATOR || !takes_priority(t.content.op, t2.content.op)) {
                        break;
                    }
                    add_token(output, t2);
                    pop(&op_stack);
                }
                push(&op_stack, t);
                break;
            case PARENTHESIS:
                if (is_left_parenthesis(&t)) {
                    push(&op_stack, t);
                }
                else {
                    bool parenthesis_found = false;
                    while (!is_empty_stack(&op_stack)) {
                        t_expr_token t2 = pop(&op_stack);
                        if (t2.type == PARENTHESIS && is_left_parenthesis(&t2)) {
                            parenthesis_found = true;
                            break;
                        }
                        add_token(output, t2);
                    }
                    if (!parenthesis_found) {
                        printf("Error, missing parenthesis\n");
                        exit(EXIT_FAILURE);
                    }
                }
                break;
        }
    }

    while (!is_empty_stack(&op_stack)) {
        t_expr_token t = pop(&op_stack);
        if (t.type != OPERATOR) {
            printf("Error, expression is wrongly formed\n");
            exit(EXIT_FAILURE);
        }
        add_token(output, t);
    }

    destroy_expr(expr);
    destroy_stack(&op_stack);
    return expr_rpn;
}

void destroy_expr(t_expr *expr) {
    destroy_list(&expr->list); //DONE
}

void destroy_expr_rpn(t_expr_rpn *expr_rpn) {
    destroy_expr(&expr_rpn->expr); //DONE
}