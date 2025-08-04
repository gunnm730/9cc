#include "9cc.h"

// 创建一个新的Token并链接到cur之后
Token *new_token(TokenKind kind, Token *cur, char *str, int len) {
  Token *tok = calloc(1, sizeof(Token));
  tok->kind = kind;
  tok->str = str;
  tok->len = len;
  cur->next = tok;
  return tok;
}

// 将输入的字符串p进行词法分析，并返回Token链表的头
// 修正：函数签名接收一个 char *p 参数
Token *tokenize(char *p) { 
    Token head;
    head.next = NULL;
    Token *cur = &head;

    // 删除了 char* p = user_input; 这一行

    while (*p) {
        if (isspace(*p)) {
            p++;
            continue;
        }

        if (strncmp(p, "==", 2) == 0 || strncmp(p, "!=", 2) == 0 ||
            strncmp(p, "<=", 2) == 0 || strncmp(p, ">=", 2) == 0) {
            cur = new_token(TK_RESERVED, cur, p, 2);
            p += 2;
            continue;
        }

        // 修正：加入了分号 ';'
        if (strchr("+-*/()<>=;", *p)) {
            cur = new_token(TK_RESERVED, cur, p++, 1);
            continue;
        }

        if (isalpha(*p)) {
            char *q = p; // 1. 标记起点
            while (isalnum(*p)) { // 2. 贪心循环
                p++;
            }
            int len = p - q; // 3. 计算长度
            cur = new_token(TK_IDENT, cur, q, len); // 4. 创建Token
            continue;
        }

        if (isdigit(*p)) {
            cur = new_token(TK_NUM, cur, p, 0);
            char *q = p;
            cur->val = strtol(p, &p, 10);
            cur->len = p - q;
            continue;
        }

        error_at(p, "无法识别的词法单元");
    }

    new_token(TK_EOF, cur, p, 0);
    return head.next;
}
