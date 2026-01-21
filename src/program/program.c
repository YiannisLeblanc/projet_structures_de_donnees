#include "program/program.h"
#include "structures/prog_token_list.h"
#include "program/lexer.h"
#include "program/parser.h"
#include "program/run.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void print_type_of_statement(e_statement_type type) {

    switch (type) {
        case Assignment:
            printf("Assignment");
            break;
        case If:
            printf("If");
            break;
        case While:
            printf("While");
            break;
        case Return:
            printf("Return");
            break;
        case Print:
            printf("Print");
            break;
    }
}

void print_prog_node(FILE *file, const t_ast *prog) {
    const t_expr_rpn *e;
    switch (prog->command) {
        case Return: {
            const t_return_statement *st = &prog->statement.return_st;
            e = &st->expr;
            fprintf(file, "Return (");
            print_expr_file(file, &e->expr);
            fprintf(file, ")");
            break;
        }
        case Print: {
            const t_print_statement *st = &prog->statement.print_st;
            e = &st->expr;
            fprintf(file, "Print (");
            print_expr_file(file, &e->expr);
            fprintf(file, ")");
            break;
        }
        case Assignment: {
            const t_assignment_statement *st = &prog->statement.assignment_st;
            e = &st->expr;
            fprintf(file, "%c ← ", st->var);
            print_expr_file(file, &e->expr);
            break;
        }
        case If: {
            const t_if_statement *st = &prog->statement.if_st;
            e = &st->cond;
            fprintf(file, "If (");
            print_expr_file(file, &e->expr);
            fprintf(file, ")");
            break;
        }
        case While: {
            const t_while_statement *st = &prog->statement.while_st;
            e = &st->cond;
            fprintf(file, "While (");
            print_expr_file(file, &e->expr);
            fprintf(file, ")");
            break;
        }
    }
}

// Returns true if the current program stops (reaches a final state)
bool print_mermaid_aux(FILE *file, const t_ast *prog, int *cpt) {
    if (prog == NULL)
        return false;

    const int current_index = *cpt;

//#define FLOWCHART
#ifdef FLOWCHART
    fprintf(file, "\tA%d[\"", current_index);
    print_prog_node(file, prog);
    fprintf(file, "\"]\n");
#else
    fprintf(file, "\tA%d: ", current_index);
    print_prog_node(file, prog);
    fprintf(file, "\n");
#endif

    switch (prog->command) {
        case Return:
            fprintf(file, "\tA%d --> [*]\n", current_index);
            return true;
        case Print:
        case Assignment: {
            if (prog->next != NULL) {
                (*cpt)++;
                const int next_token_index = *cpt;
                fprintf(file, "\tA%d --> A%d\n", current_index, next_token_index);
            }
            break;
        }
        case If: {
            const t_if_statement *st = &prog->statement.if_st;
            (*cpt)++;
            const int cpt_if_true = *cpt;
            int cpt_if_false;
#ifdef FLOWCHART
            fprintf(file, "\tA%d -- then --> A%d\n", current_index, cpt_if_true);
#else
            fprintf(file, "\tA%d --> A%d: then\n", current_index, cpt_if_true);
#endif
            const bool then_final = print_mermaid_aux(file, st->if_true, cpt);
            bool else_final;
            const int index_ret_true = *cpt;
            int index_ret_else;
            
            if (st->if_false != NULL) {
                (*cpt)++;
                cpt_if_false = *cpt;
#ifdef FLOWCHART
                fprintf(file, "\tA%d -- else --> A%d\n", current_index, cpt_if_false);
#else
                fprintf(file, "\tA%d --> A%d: else\n", current_index, cpt_if_false);
#endif
                else_final = print_mermaid_aux(file, st->if_false, cpt);
                index_ret_else = *cpt;
            }
            (*cpt)++;
            const int next_token_index = *cpt;//((st->if_false != NULL) && (then_final || else_final)) ? *cpt + 1 : *cpt;
            //if (prog->next == NULL && (!then_final && !else_final)) {
#ifdef FLOWCHART
                fprintf(file, "\tA%d[\" \"]\n", next_token_index);
#else
                fprintf(file, "\tstate A%d <<choice>>\n", next_token_index);
#endif
            //}
            if (!then_final)
                fprintf(file, "\tA%d --> A%d\n", index_ret_true, next_token_index);
            if (st->if_false != NULL && !else_final)
                fprintf(file, "\tA%d --> A%d\n", index_ret_else, next_token_index);
            else {
                if (st->if_false == NULL) {
#ifdef FLOWCHART
                    fprintf(file, "\tA%d -- else --> A%d\n", current_index, next_token_index);
#else
                    fprintf(file, "\tA%d --> A%d: else\n", current_index, next_token_index);
#endif
                }
            }
            break;
        }
        case While: {
            const t_while_statement *st = &prog->statement.while_st;
            (*cpt)++;
            fprintf(file, "\tA%d --> A%d: then\n", current_index, *cpt);
            print_mermaid_aux(file, st->block, cpt);
            const int index_ret_block = *cpt;
            fprintf(file, "\tA%d --> A%d\n", index_ret_block, current_index);
            (*cpt)++;
            const int next_token = *cpt;
            fprintf(file, "\tA%d --> A%d: next\n", current_index, next_token);
            break;
        }
    }
    return print_mermaid_aux(file, prog->next, cpt);
}

// Generates a Mermaid graph representing the tree
void print_ast(const t_ast *prog, const char *file_name) {
    FILE *file = fopen(file_name, "w");
    int cpt = 0;
#ifdef FLOWCHART
    fprintf(file, "flowchart TD\n");
#else
    fprintf(file, "stateDiagram\n");
    cpt++;
    fprintf(file, "\t[*] --> A%d\n", cpt);
#endif
    print_mermaid_aux(file, prog, &cpt);
    fclose(file);
    printf("AST exported as %s\n", file_name);
}

void run_program(const char *s) {
    t_prog_token_list list = lex(s);
    
    t_ast *prog = parse(&list);
    ptl_destroy_list(&list);

    run(prog);
    
    destroy_ast(prog);
}


void export_program_ast(const char *s, const char *source_file_name) {
    t_prog_token_list list = lex(s);
    t_ast *prog = parse(&list);
    ptl_destroy_list(&list);

    const int len = strlen(source_file_name);
    char file_name[1000];
    strncpy(file_name, source_file_name, len);
    char *ext = file_name + len - (len >= 5 && source_file_name[len - 4] == '.' ? 4 : 0);
    strcpy(ext, ".mmd");
    ext[4] = '\0';

    print_ast(prog, file_name);
    destroy_ast(prog);
}


//////////////////////////////////////////////////////////////////////////

void destroy_statement(const e_statement_type type, u_statement *statement) {
    if (statement == NULL)
        return;

    switch (type) {
        case Assignment: {
            t_assignment_statement *st = &statement->assignment_st;
            // TODO
            break;
        }
        case Return: {
            t_return_statement *st = &statement->return_st;
            // TODO
            break;
        }
        case Print: {
            t_print_statement *st = &statement->print_st;
            // TODO
            break;
        }
        case If: {
            t_if_statement *st = &statement->if_st;
            // TODO
            break;
        }
        case While: {
            t_while_statement *st = &statement->while_st;
            // TODO
            break;
        }
    }
}

void destroy_ast(t_ast *prog) {
    if (prog == NULL)
        return;
    switch(prog->command){
        case If:
        destroy_ast(prog->statement.if_st.if_true);
        destroy_ast(prog->statement.if_st.if_false);
        break;
        case While:
        destroy_ast(prog->statement.while_st.block);
        break;
        case Assignment:
        case Return:
        case Print:
        default:
        break;
    }
    destroy_ast(prog->next);
    free(prog); //DONE
}