#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum{
    TK_RESERVED, // Keyword ,Operators
    TK_NUM,
    TK_EOF
}TokenKind;

typedef struct Token Token;

struct Token{
    TokenKind kind;
    Token *next;
    int val;
    char* str;
};

Token* token;

// Auxiliary function for quickly outputting error messages
void error(char* fmt,...){
    va_list ap;
    va_start(ap,fmt);
    vfprintf(stderr,fmt,ap);
    fprintf(stderr,"\n");
    exit(1);
}

// After processing a numeric token, check whether the next token is an operator
// If it is, consume it; if not, return false
bool consume(char op){
    if(token->kind!=TK_RESERVED||token->str[0]!=op){
        return false;
    }
    token=token->next;
    return true;
}

// After processing a numeric token, check whether the next token is an operator
// If it is, consume it; if not, directly report an error
void expect(char op){
    if(token->kind!=TK_RESERVED||token->str[0]!=op){
        error("%c expected here",op);
    }
    token=token->next;
}

// If the next Token is a numerical value, then read the Token forward by one and return the numerical value.
// In other cases, an error will be reported.
int expect_number(){
    if(token->kind!=TK_NUM){
        error("Number expected here");
    }
    int val=token->val;
    token=token->next;
    return val;
}

// Check if got the end
bool at_eof(){
    return token->kind==TK_EOF;
}

// Create a new Token after cur(usually token)
Token *new_token(TokenKind kind, Token *cur, char *str) {
  Token *tok = calloc(1, sizeof(Token));
  tok->kind = kind;
  tok->str = str;
  cur->next = tok;
  return tok;
}

Token* tokenize(char *p){
    Token head;
    head.next=NULL;
    Token* cur=&head;

    while(*p){
        if(isspace(*p)){
            p++;
            continue;
        }
        if(*p=='-'||*p=='+'){
            cur=new_token(TK_RESERVED,cur,p++);
            continue;
        }
        if(isdigit(*p)){
            cur=new_token(TK_NUM,cur,p);
            cur->val=strtol(p,&p,10);
            continue;
        }
        error("%c unexpected.",*p);
    }
    new_token(TK_EOF, cur, p);
    return head.next;
}

int main(int argc,char** argv){
    if(argc!=2){
        error("Incorrect command format. Please enter something like : ./9cc (expression)");
        return 1;
    }

    token=tokenize(argv[1]);

    printf(".intel_syntax noprefix\n");
    printf(".globl main\n");
    printf("main:\n");

    printf("  mov rax, %d\n", expect_number());
    while (!at_eof()) {
        if (consume('+')) {
            printf("\tadd rax, %d\n", expect_number());
            continue;
        }

        expect('-');
        printf("\tsub rax, %d\n", expect_number());
    }
    printf("\tret\n");
    return 0;
}




