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

bool consume(char op);
void expect(char op);
int expect_number();

// kind of tree node
typedef enum {
  ND_ADD, // +
  ND_SUB, // -
  ND_MUL, // *
  ND_DIV, // /
  ND_NUM, // 整数
} NodeKind;

typedef struct Node Node;

// define node of tree
struct Node{
    NodeKind kind;
    Node* lhs;
    Node* rhs;
    int val;
};

// add a new node of operator
Node* new_node(NodeKind kind,Node* lhs,Node* rhs){
    Node* node=calloc(1,sizeof(Node));
    node->kind=kind;
    node->lhs=lhs;
    node->rhs=rhs;
    return node;
}

// add a new node of number
Node* new_node_num(int val){
    Node* node=calloc(1,sizeof(Node));
    node->kind=ND_NUM;
    node->val=val;
    return node;
}

/*
* The expression generation formula considering only addition, 
* subtraction, multiplication, and division operations is as follows:
* 
* expr    = mul ("+" mul | "-" mul)*
* mul     = primary ("*" primary | "/" primary)*
* primary = num | "(" expr ")"
* 
*/
Node* expr();
Node* mul();
Node* primary();


Node* expr(){
    Node* node=mul();

    while(true){
        if(consume('+'))
            node=new_node(ND_ADD,node,mul());
        else if(consume('-'))
            node=new_node(ND_SUB,node,mul());
        else
            return node;
    }
}

Node* mul(){
    Node* node=primary();

    while(true){
        if(consume('*'))
            node=new_node(ND_MUL,node,primary());
        else if(consume('/'))
            node=new_node(ND_DIV,node,primary());
        else
            return node;
    }
}

Node* primary(){
    if(consume('(')){
        Node* node=expr();
        expect(')');
        return node;
    }
    return new_node_num(expect_number());
}

void gen(Node* node){
    if(node->kind==ND_NUM){
        printf("  push %d\n",node->val);
        return;
    }

    gen(node->lhs);
    gen(node->rhs);

    printf("  pop rdi\n");
    printf("  pop rax\n");

    switch (node->kind)
    {
    case ND_ADD:
        printf("  add rax, rdi\n");
        break;
    case ND_SUB:
        printf("  sub rax, rdi\n");
        break;
    case ND_MUL:
        printf("  imul rax, rdi\n");
        break;
    case ND_DIV:
        printf("  cqo\n");
        printf("  idiv rdi\n");
        break;
    default:
        break;
    }

    printf("  push rax\n");
}


char* user_input;

void error_at(char* loc,char* fmt,...){
    va_list ap;
    va_start(ap,fmt);

    int pos = loc - user_input;
    fprintf(stderr, "%s\n", user_input);
    fprintf(stderr, "%*s", pos, "");
    fprintf(stderr, "^ ");
    vfprintf(stderr, fmt, ap);
    fprintf(stderr, "\n");
    exit(1);
}

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
        error_at(token->str,"%c expected here",op);
    }
    token=token->next;
}

// If the next Token is a numerical value, then read the Token forward by one and return the numerical value.
// In other cases, an error will be reported.
int expect_number(){
    if(token->kind!=TK_NUM){
        error_at(token->str,"Number expected here");
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
        if(*p == '+' || *p == '-' || *p == '*' || *p == '/' || *p == '(' || *p == ')'){
            cur=new_token(TK_RESERVED,cur,p++);
            continue;
        }
        if(isdigit(*p)){
            cur=new_token(TK_NUM,cur,p);
            cur->val=strtol(p,&p,10);
            continue;
        }
        error_at(p,"%c unexpected.",*p);
    }
    new_token(TK_EOF, cur, p);
    return head.next;
}

int main(int argc,char** argv){
    if(argc!=2){
        error("Incorrect command format. Please enter something like : ./9cc (expression)");
        return 1;
    }

    user_input = argv[1];
    token=tokenize(argv[1]);
    Node* node=expr();

    printf(".intel_syntax noprefix\n");
    printf(".globl main\n");
    printf("main:\n");

    gen(node);

    printf("  pop rax\n");
    printf("  ret\n");
    return 0;
}




