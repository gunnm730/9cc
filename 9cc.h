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
extern Token *token;
extern char *user_input;

Token *tokenize(char *p);
void error(char *fmt, ...);
void error_at(char *loc, char *fmt, ...);


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

Node *expr();

void gen(Node *node);