#include "csapp.h"

int main(int argc, char **argv) {
    int clientfd; //client file descriptor
    char *host, *port, buf[MAXLINE];
    rio_t rio;

    if (argc != 3) {
        fprintf(stderr, "usage %s <host> <port>\n", argv[0]);
        exit(0);
    }
    host = argv[1];
    port = argv[2];

    clientfd = Open_clientfd(host, port); //서버와 연결 설정
    Rio_readinitb(&rio, clientfd); //rio 구조체 초기화 -> 읽기 작업을 위한 버퍼 및 상태정보 저장

    while (Fgets(buf, MAXLINE, stdin) != NULL) { //EOF를 만나면 종료
        Rio_writen(clientfd, buf, strlen(buf)); //입력받은 문자열 전송
        Rio_readlineb(&rio, buf, MAXLINE); //rio 구조체를 통해 서버로부터 응답을 받는다.
        Fputs(buf, stdout); //출력
    }

    Close(clientfd);
    exit(0);
}