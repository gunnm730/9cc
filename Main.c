#include "9cc.h"

// 全局变量的定义 (Definition)
Token *token;
char *user_input;
Node* code[100];
LVar *locals;

// 错误处理函数
void error(char *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);
  vfprintf(stderr, fmt, ap);
  fprintf(stderr, "\n");
  exit(1);
}

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

// 主函数
int main(int argc, char **argv) {
  if (argc != 2) {
    error("参数数量错误。用法: ./9cc \"<表达式>\"");
    return 1;
  }

  user_input=argv[1];
  token = tokenize(user_input);
  program();
  
  printf(".intel_syntax noprefix\n");
  printf(".globl main\n");
  printf("main:\n");

  


  printf("  push rbp\n");
  printf("  mov rbp, rsp\n");
  int offset = locals ? locals->offset : 0;
  printf("  sub rsp, %d\n", offset);

  for (int i = 0; code[i]; i++) {
    gen(code[i]);

    printf("  pop rax\n");
  }

  printf("  mov rsp, rbp\n");
  printf("  pop rbp\n");
  printf("  ret\n");
  return 0;
}