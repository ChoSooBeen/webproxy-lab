#include "../csapp.h"

int main(int argc, char **argv) {
    int clientfd; //client file descriptor
    char *host, *port, buf[MAXLINE];
    rio_t rio;

    // 호스트와 포트 정보가 주어지지 않은 경우
    if (argc != 3) {
        fprintf(stderr, "usage %s <host> <port>\n", argv[0]);
        exit(0);
    }

    host = argv[1];
    port = argv[2];

    clientfd = Open_clientfd(host, port); //지정된 호스트와 포트로 서버에 연결하는 클라이언트 소켓 생성
    Rio_readinitb(&rio, clientfd); //rio 구조체 초기화 -> 읽기 작업을 위한 버퍼 및 상태정보 저장

    while (Fgets(buf, MAXLINE, stdin) != NULL) { //EOF를 만나면 종료
        Rio_writen(clientfd, buf, strlen(buf)); //입력받은 문자열 서버로 전송
        Rio_readlineb(&rio, buf, MAXLINE); //rio 구조체를 통해 서버로부터 응답을 읽어들이다.
        Fputs(buf, stdout); //출력
    }

    Close(clientfd);
    exit(0);
}