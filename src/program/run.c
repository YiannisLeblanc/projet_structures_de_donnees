#include <stdio.h>
#include <stdbool.h>

#include "program/program.h"

// Recursive function, evaluates the program
// Returns true if a Return statement was reached, stop the execution
// Returns false if the end of a block was reached
bool run_aux(int var_value[], const t_ast *prog) {
    bool returnValue = false;
    t_ast* tempProg = prog;
    while(tempProg!=NULL && !returnValue){
    switch (tempProg->command) {

        case Return: {
            // prog->command == Return, so we assume that the statement has type t_return_statement
            t_return_statement st = tempProg->statement.return_st;
            printf("-> %d\n",eval_rpn(var_value, &st.expr));
            returnValue = true;
        }
            break;
        case Assignment:
            var_value[tempProg->statement.assignment_st.var-'a'] = eval_rpn(var_value, &tempProg->statement.assignment_st.expr);
            break;
        case If:
            if(eval_rpn(var_value, &tempProg->statement.if_st.cond)){
                returnValue = run_aux(var_value, tempProg->statement.if_st.if_true);
            }else{
                returnValue = run_aux(var_value, tempProg->statement.if_st.if_false);
            }
            break;
        case While:
            while(eval_rpn(var_value, &tempProg->statement.while_st.cond) && !returnValue){
                returnValue = run_aux(var_value, tempProg->statement.while_st.block);
            }
            break;
        case Print:
            printf("%d\n",eval_rpn(var_value, &tempProg->statement.print_st.expr));
            break;
        default:
            break;
        }
        tempProg = tempProg->next;
    }
    return returnValue;
}

void run(t_ast *prog) {
    int var_value['z'-'a'+1];
    run_aux(var_value, prog);
}