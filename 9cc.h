#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Token Token;
typedef struct Node Node;
typedef struct LVar LVar;

// ローカル変数の型
struct LVar {
  LVar *next; // 次の変数かNULL
  char *name; // 変数の名前
  int len;    // 名前の長さ
  int offset; // RBPからのオフセット
};

// Token的类型
typedef enum {
    TK_RESERVED, // 关键字、运算符
    TK_NUM,      // 整数
    TK_IDENT,    // identifier
    TK_EOF,      // 文件结束符
} TokenKind;



// Token 结构体
struct Token {
    TokenKind kind; // Token类型
    Token *next;    // 指向下一个Token的指针
    int val;        // 如果kind是TK_NUM，其数值
    char *str;      // Token字符串
    int len;
};

// 当前正在处理的Token
extern Token *token;
extern char *user_input;
extern Node *code[100];
extern LVar *locals;

Token *tokenize(char *p);
void error(char *fmt, ...);
void error_at(char *loc, char *fmt, ...);
LVar *find_lvar(Token *tok);


// 抽象语法树节点的类型
typedef enum {
    ND_ADD, // +
    ND_SUB, // -
    ND_MUL, // *
    ND_DIV, // /
    ND_NUM, // 整数
    ND_LVAR,   // ローカル変数
    ND_EQ,  // ==
    ND_NE,  // !=
    ND_NL,  // >=
    ND_NG,  // <=
    ND_GR,  // >
    ND_LE,   // <
    ND_ASSIGN // =
} NodeKind;



// 抽象语法树节点的定义
struct Node {
    NodeKind kind; // 节点类型
    Node *lhs;     // 左子节点 (left-hand side)
    Node *rhs;     // 右子节点 (right-hand side)
    int val;       // kind为ND_NUM时使用
    int offset;
};

void program();

void gen(Node *node);