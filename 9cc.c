#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Token的类型
typedef enum {
    TK_RESERVED, // 关键字、运算符
    TK_NUM,      // 整数
    TK_EOF,      // 文件结束符
} TokenKind;

typedef struct Token Token;

// Token 结构体
struct Token {
    TokenKind kind; // Token类型
    Token *next;    // 指向下一个Token的指针
    int val;        // 如果kind是TK_NUM，其数值
    char *str;      // Token字符串
    int len;
};

// 当前正在处理的Token
Token *token;

// 函数声明
bool consume(char* op);
void expect(char* op);
int expect_number();

// 抽象语法树节点的类型
typedef enum {
    ND_ADD, // +
    ND_SUB, // -
    ND_MUL, // *
    ND_DIV, // /
    ND_NUM, // 整数
    ND_EQ,  // ==
    ND_NE,  // !=
    ND_NL,  // >=
    ND_NG,  // <=
    ND_GR,  // >
    ND_LE   // <
} NodeKind;

typedef struct Node Node;

// 抽象语法树节点的定义
struct Node {
    NodeKind kind; // 节点类型
    Node *lhs;     // 左子节点 (left-hand side)
    Node *rhs;     // 右子节点 (right-hand side)
    int val;       // kind为ND_NUM时使用
};

// 创建一个新的运算符节点
Node *new_node(NodeKind kind, Node *lhs, Node *rhs) {
    Node *node = calloc(1, sizeof(Node));
    node->kind = kind;
    node->lhs = lhs;
    node->rhs = rhs;
    return node;
}

// 创建一个新的数字节点
Node *new_node_num(int val) {
    Node *node = calloc(1, sizeof(Node));
    node->kind = ND_NUM;
    node->val = val;
    return node;
}

/*
表达式的巴科斯范式（BNF）定义如下：
expr       = equality
equality   = relational ("==" relational | "!=" relational)*
relational = add ("<" add | "<=" add | ">" add | ">=" add)*
add        = mul ("+" mul | "-" mul)*
mul        = unary ("*" unary | "/" unary)*
unary      = ("+" | "-")? primary
primary    = num | "(" expr ")"
 */
Node *expr();
Node *equality();
Node *relational();
Node *add();
Node *mul();
Node *unary();
Node *primary();

Node *expr() {
    Node *node = equality();
    return node;
}

Node *equality(){
    Node *node=relational();
    for(;;){
        if(consume("=="))
            node=new_node(ND_EQ,node,relational());
        else if(consume("!="))
            node=new_node(ND_NE,node,relational());
        else
            return node;
    }
}

Node *relational(){
    Node *node=add();
    for(;;){
        if(consume(">="))
            node=new_node(ND_NL,node,add());
        else if(consume(">"))
            node=new_node(ND_GR,node,add());
        else if(consume("<="))
            node=new_node(ND_NG,node,add());
        else if(consume("<"))
            node=new_node(ND_LE,node,add());
        else
            return node;
    }
}

Node *add(){
    Node* node=mul();
    for(;;){
        if(consume("+"))
            node=new_node(ND_ADD,node,mul());
        else if(consume("-"))
            node=new_node(ND_SUB,node,mul());
        else
            return node;
    }
}

Node *mul() {
    Node *node = unary();

    for (;;) {
        if (consume("*"))
            node = new_node(ND_MUL, node, unary()); // 修正：调用 unary()
        else if (consume("/"))
            node = new_node(ND_DIV, node, unary()); // 修正：调用 unary()
        else
            return node;
    }
}

Node *unary() {
    if (consume("+"))
        return primary();
    if (consume("-"))
        return new_node(ND_SUB, new_node_num(0), primary()); // 转换为 0 - primary
    return primary();
}

Node *primary() {
    if (consume("(")) {
        Node *node = expr();
        expect(")");
        return node;
    }
    return new_node_num(expect_number());
}

// --- 代码生成 ---
void gen(Node *node) {
    if (node->kind == ND_NUM) {
        printf("  push %d\n", node->val);
        return;
    }

    gen(node->lhs);
    gen(node->rhs);

    printf("  pop rdi\n");
    printf("  pop rax\n");

    switch (node->kind) {
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
    case ND_EQ:
        printf("  cmp rax, rdi\n");
        printf("  sete al\n");
        printf("  movzb rax, al\n");
        break;
    case ND_NE:
        printf("  cmp rax, rdi\n");
        printf("  setne al\n");
        printf("  movzb rax, al\n");
        break;
    case ND_LE:
        printf("  cmp rax, rdi\n");
        printf("  setl al\n");
        printf("  movzb rax, al\n");
        break;
    case ND_NG:
        printf("  cmp rax, rdi\n");
        printf("  setle al\n");
        printf("  movzb rax, al\n");
        break;
    case ND_GR:
        printf("  cmp rax, rdi\n");
        printf("  setg al\n");
        printf("  movzb rax, al\n");
        break;
    case ND_NL:
        printf("  cmp rax, rdi\n");
        printf("  setge al\n");
        printf("  movzb rax, al\n");
        break;
    default:
        break;
    }

    printf("  push rax\n");
}

// --- 错误处理 ---
char *user_input;

void error_at(char *loc, char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);

    int pos = loc - user_input;
    fprintf(stderr, "%s\n", user_input);
    fprintf(stderr, "%*s", pos, "");
    fprintf(stderr, "^ ");
    vfprintf(stderr, fmt, ap);
    fprintf(stderr, "\n");
    exit(1);
}

void error(char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    vfprintf(stderr, fmt, ap);
    fprintf(stderr, "\n");
    exit(1);
}

// --- 解析器辅助函数 ---
bool consume(char *op) {
  if (token->kind != TK_RESERVED ||
      strlen(op) != token->len ||
      memcmp(token->str, op, token->len))
    return false;
  token = token->next;
  return true;
}

void expect(char *op) {
    if (token->kind != TK_RESERVED ||
      strlen(op) != token->len ||
      memcmp(token->str, op, token->len)) {
        error_at(token->str, "应为 '%c'", op);
    }
    token = token->next;
}

int expect_number() {
    if (token->kind != TK_NUM) {
        error_at(token->str, "应为数字");
    }
    int val = token->val;
    token = token->next;
    return val;
}

bool at_eof() {
    return token->kind == TK_EOF;
}

// --- 词法分析器 (Tokenizer) ---
Token *new_token(TokenKind kind, Token *cur, char *str, int len) {
    Token *tok = calloc(1, sizeof(Token));
    tok->kind = kind;
    tok->str = str;
    tok->len=len;
    cur->next = tok;
    return tok;
}

Token *tokenize(char *p) {
    Token head;
    head.next = NULL;
    Token *cur = &head;

    while(*p){
        if(isspace(*p)){
            p++;
            continue;
        }
        if(strncmp(p,"==",2)==0||
        strncmp(p,">=",2)==0||
        strncmp(p,"<=",2)==0||
        strncmp(p,"!=",2)==0){
            cur=new_token(TK_RESERVED,cur,p,2);
            p+=2;
            continue;
        }
        if(strchr("+-*/()<>",*p)){
            cur=new_token(TK_RESERVED,cur,p,1);
            p++;
            continue;
        }
        if(isdigit(*p)){
            cur=new_token(TK_NUM,cur,p,0);
            char* q=p;
            cur->val=strtol(p,&p,10);
            cur->len=p-q;
            continue;
        }
        error_at(p,"unknowned character");
    }
    new_token(TK_EOF, cur, p, 0);
    return head.next;
}

// --- 主函数 ---
int main(int argc, char **argv) {
    if (argc != 2) {
        error("参数数量错误。用法: ./9cc \"<表达式>\"");
        return 1;
    }

    user_input = argv[1];
    token = tokenize(argv[1]);
    Node *node = expr();

    printf(".intel_syntax noprefix\n");
    printf(".globl main\n");
    printf("main:\n");

    gen(node);

    printf("  pop rax\n");
    printf("  ret\n");
    return 0;
}




