/*
 * adder.c - a minimal CGI program that adds two numbers together
 */
/* $begin adder */
#include "../csapp.h"

int main(void) {
  char *buf, *p;
  char arg1[MAXLINE], arg2[MAXLINE], content[MAXLINE];
  int n1 = 0, n2 = 0;

  //환경변수 - QUERY_STRING에서 인자 추출
  if((buf = getenv("QUERY_STRING")) != NULL) {
    p = strchr(buf, '&'); //인자 구분자 = &
    *p = '\0';
    strcpy(arg1, buf);
    strcpy(arg2, p + 1);
    n1 = atoi(arg1);
    n2 = atoi(arg2);
  }

  //content에 응답 본체 작성 - HTML 형식
  sprintf(content, "QUERY_STRING=%s", buf);
  sprintf(content, "Welcome to add.com: ");
  sprintf(content, "%s THE Internet addition portal.\r\n<p>", content);
  sprintf(content, "%sThe answer is %d + %d = %d\r\n<p>", content, n1, n2, n1 + n2);
  sprintf(content, "%sThanks for visiting!\r\n", content);

  printf("Connection: close\r\n");
  printf("Content-length: %d\r\n", (int)strlen(content));
  printf("Content-type: text/html\r\n\r\n");
  printf("%s", content);
  fflush(stdout);

  exit(0);
}
/* $end adder */