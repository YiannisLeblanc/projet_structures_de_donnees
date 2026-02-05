#include "program/lexical.h"
#include "program/program.h"
#include "program/lexer.h"
#include <string.h>
#include <stdio.h>

bool str_eq(const char* s1, const char *s2, const unsigned int length) {
    for (unsigned int i = 0; i < length; i++) {
        if (s1[i] == '\0' || s2[i] == '\0' || s1[i] != s2[i])
            return false;
    }
    return true;
}

// Returns true if s (= *p_s) contains the keyword of type keyword_type as a prefix
// If so, fills the token so that it is a token of type keyword
// Moves p_s forward to skip past the keyword
bool process_keyword(const char **p_s, const e_keyword keyword_type, t_prog_token *token) {
    #define KW_MAX_LEN 7
    char kwStr[KW_MAX_LEN];
    switch(keyword_type){
        case KW_ASSIGN:
        strcpy(kwStr, "=");
        break;
        case KW_IF:
        strcpy(kwStr, "if");
        break;
        case KW_ELSE:
        strcpy(kwStr, "else");
        break;
        case KW_WHILE:
        strcpy(kwStr, "while");
        break;
        case KW_RETURN:
        strcpy(kwStr, "return");
        break;
        case KW_PRINT:
        strcpy(kwStr, "print");
        break;
        default:
        return false;
    }
    unsigned int size = strlen(kwStr);
    if(str_eq(*p_s, kwStr, size)){
        token->token_type = PT_KEYWORD;
        token->content.keyword = keyword_type;
        *p_s += size;
        return true;
    }
    return false;
}

#define TABULATION_SIZE 4

// ADDED BY STUDENT
int process_indentation(const char **s){
    if(**s == '\t'){
        (*s)++;
        return 1+process_indentation(s);
    } else if(**s == ' '){
        (*s)++;
        for(int i = 0 ; i < TABULATION_SIZE-1 ; i++){
            if(**s != ' ') return 0;

            (*s)++;
        }
        return 1+process_indentation(s);
    } else {
        return 0;
    }
}

t_prog_token_list lex(const char *s) {
    t_prog_token_list list = ptl_create_empty_list();

#define NB_KEYWORDS 7
    static const e_keyword keywords[NB_KEYWORDS] = { KW_ASSIGN, KW_IF, KW_ELSE, KW_WHILE, KW_ENDBLOCK, KW_RETURN, KW_PRINT };
    unsigned int previousIndentation = 0;
    unsigned int currentIndentation;
    bool expectingExpression = false;
    unsigned int ligne = 1;
    while(*s != '\0') {
        currentIndentation = 0;
        if (*s == '#'){
            while(*s != '\n' && *s != '\0'){
                s++;
            }
        } else if(*s == '\n'){
            s++;
            ligne++;
            currentIndentation = process_indentation(&s);
            if(currentIndentation <= previousIndentation) {
                t_prog_token token;
                token.token_type = PT_KEYWORD;
                token.content.keyword = KW_ENDBLOCK;
                for(int i = 0 ; i < (previousIndentation-currentIndentation) ; i++){
                    ptl_push_back(&list, token);
                }
            previousIndentation = currentIndentation;
            } else {
                printf("WARNING mauvaise indentation ligne : %u\n", ligne);

            }
        } else if(*s==' '){
            s++;
        } else if((*s >= 'a' && *s <= 'z') && !(s[1] >= 'a' && s[1] <= 'z') && !expectingExpression){
            t_prog_token token = {.token_type = PT_VAR, .content.var = *s};
            
                ptl_push_back(&list, token);
            

            s++;
        } else if(expectingExpression){
            t_prog_token token;
            token.token_type = PT_EXPR;
            t_expr expr = parse_expr(&s);
            token.content.expr_rpn = shunting_yard(&expr);
            ptl_push_back(&list, token);
            expectingExpression = false;
        } else {
            t_prog_token token;
            for(int i = 0 ; i < NB_KEYWORDS && !expectingExpression; i++){
                if(process_keyword(&s, keywords[i], &token)){
                    if(token.content.keyword == KW_ELSE){
                        t_prog_token last_token = ptl_get(&list, list.size-1);
                        if (last_token.token_type == PT_KEYWORD && last_token.content.keyword == KW_ENDBLOCK){
                            ptl_set(&list, list.size-1, token);
                        } else {
                            ptl_push_back(&list, token);
                        }
                        previousIndentation++;
                    } else {
                        if(token.content.keyword == KW_IF || token.content.keyword == KW_WHILE){
                            previousIndentation ++;
                        }
                        ptl_push_back(&list, token);
                        expectingExpression = true;
                    }
                }
            }
        }
        
    }
    t_prog_token token;
    token.token_type = PT_KEYWORD;
    token.content.keyword = KW_ENDBLOCK;
    for(int i = 0 ; i < previousIndentation ; i++){
        ptl_push_back(&list, token);
    }
    return list;
}