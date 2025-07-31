#include "9cc.h"

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

