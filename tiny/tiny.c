/* $begin tinymain */
/*
 * tiny.c - A simple, iterative HTTP/1.0 Web server that uses the
 *     GET method to serve static and dynamic content.
 *
 * Updated 11/2019 droh
 *   - Fixed sprintf() aliasing issue in serve_static(), and clienterror().
 */
#include "csapp.h"

void doit(int fd);
void read_requesthdrs(rio_t *rp);
int parse_uri(char *uri, char *filename, char *cgiargs);
void serve_static(int fd, char *filename, int filesize, char *method);
void get_filetype(char *filename, char *filetype);
void serve_dynamic(int fd, char *filename, char *cgiargs, char *method);
void clienterror(int fd, char *cause, char *errnum, char *shortmsg, char *longmsg);

int main(int argc, char **argv) {
  int listenfd, connfd;
  char hostname[MAXLINE], port[MAXLINE];
  socklen_t clientlen;
  struct sockaddr_storage clientaddr;

  /* Check command line args */
  if (argc != 2) {
    fprintf(stderr, "usage: %s <port>\n", argv[0]);
    exit(1);
  }

  listenfd = Open_listenfd(argv[1]);
  while (1) {
    clientlen = sizeof(clientaddr);
    connfd = Accept(listenfd, (SA *)&clientaddr, &clientlen);  // line:netp:tiny:accept
    Getnameinfo((SA *)&clientaddr, clientlen, hostname, MAXLINE, port, MAXLINE, 0);
    printf("Accepted connection from (%s, %s)\n", hostname, port);
    doit(connfd);   // line:netp:tiny:doit
    Close(connfd);  // line:netp:tiny:close
  }
}

/*
 * 한 개의 HTTP Transaction 처리하는 함수
 */
void doit(int fd) {
  int is_static;
  struct stat sbuf;
  char buf[MAXLINE], method[MAXLINE], uri[MAXLINE], version[MAXLINE];
  char filename[MAXLINE], cgiargs[MAXLINE];
  rio_t rio;

  //요청 읽어오기
  Rio_readinitb(&rio, fd);
  Rio_readlineb(&rio, buf, MAXLINE);
  printf("Request headers:\n");
  printf("%s", buf);
  sscanf(buf, "%s %s %s", method, uri, version);
  
  //strcasecmp = 대소문자를 구분하지 않고 두 문자열이 같을 경우 0을 반환한다.
  //GET Method가 아닐 경우 오류 발생
  if(!(strcasecmp(method, "GET")) && !(strcasecmp(method, "HEAD"))) {
    clienterror(fd, method, "501", "Not implemented", "Tiny does not implement this method");
    return;
  }
  read_requesthdrs(&rio); //Read request header

  is_static = parse_uri(uri, filename, cgiargs); //정적인지 동적인지 확인하기
  
  //원하는 파일이 없을 경우
  if(stat(filename, &sbuf) < 0) {
    clienterror(fd, filename, "404", "Not found", "Tiny couldn't find this file");
    return;
  }

  //정적 콘텐츠일 경우
  if(is_static) {
    if(!(S_ISREG(sbuf.st_mode)) || !(S_IRUSR & sbuf.st_mode)) { //권한이 없을 경우
      clienterror(fd, filename, "403", "Forbidden", "Tiny couldn't read the file");
      return;
    }
    serve_static(fd, filename, sbuf.st_size, method);
  }
  //동적 콘텐츠일 경우
  else {
    if(!(S_ISREG(sbuf.st_mode)) || !(S_IXUSR & sbuf.st_mode)) { //권한이 없을 경우
      clienterror(fd, filename, "403", "Forbidden", "Tiny couldn't run the CGI program");
      return;
    }
    serve_dynamic(fd, filename, cgiargs, method);
  }
}

/*
 * HTTP 응답을 응답 라인에 적절한 상태코드와 상태메세지와 함께 클라이언트에게 전송
 * 에러를 설명하는 응답 본체에 HTML 파일도 함께 전송
 */
void clienterror(int fd, char *cause, char *errnum, char *shortmsg, char *longmsg) {
  char buf[MAXLINE], body[MAXBUF];

  //응답 본체(body) 생성 - HTML 형식의 오류 페이지 작성
  sprintf(body, "<html><title>Tiny Error</title>");
  sprintf(body, "%s<body bgcolor=""ffffff"">\r\n", body);
  sprintf(body, "%s%s: %s\r\n", body, errnum, shortmsg);
  sprintf(body, "%s<p>%s: %s\r\n", body, longmsg, cause);
  sprintf(body, "%s<hr><em>The Tiny Web server</em>\r\n", body);

  //응답 헤더(buf) 생성 후 헤더와 본체 모두 클라이언트에게 전송
  sprintf(buf, "HTTP/1.1 %s %s\r\n", errnum, shortmsg);
  Rio_writen(fd, buf, strlen(buf));
  sprintf(buf, "Content-type: text/html\r\n");
  Rio_writen(fd, buf, strlen(buf));
  sprintf(buf, "Content-length: %d\r\n\r\n", (int)strlen(body));
  Rio_writen(fd, buf, strlen(buf));
  Rio_writen(fd, body, strlen(body));
}

/*
 * Tiny 서버는 요청 헤더 내의 어떤 정보도 사용하지 않는다.
 * 헤더를 읽기만 하고 무시한다.
 */
void read_requesthdrs(rio_t *rp) {
  char buf[MAXLINE];

  Rio_readlineb(rp, buf, MAXLINE); //첫 번째 요청 헤더 읽기
  while(strcmp(buf, "\r\n")) { //추가적으로 요청 헤더가 있을 경우
    Rio_readlineb(rp, buf, MAXLINE);
    printf("%s", buf);
  }
  return;
}

/*
 * Tiny server 정책 가정
 * 정적 콘텐츠 = 자신의 현재 디렉토리 = webproxy-lab/tiny
 * return 1
 * 실행 파일(동적 콘텐츠) = webproxy-lab/tiny/cgi-bin
 * return 0
 * 이 함수에서 위의 정책을 구현
 */
int parse_uri(char *uri, char *filename, char *cgiargs) {
  char *ptr;

  //strstr() 함수 = 문자열 내의 특정 문자열이 존재하는지 확인하는 함수
  //uri에 cgi-bin가 포함되어 있는지 확인
  if(!strstr(uri, "cgi-bin")) { //정적 콘텐츠일 경우 = cgi-bin이 포함되어 있지 않다.
    strcpy(cgiargs, "");
    strcpy(filename, "."); //현재 directory 경로(?) 복사
    strcat(filename, uri);
    if (uri[strlen(uri)-1] == '/') {
      strcat(filename, "home.html");
    }
    return 1;
  }
  else { //동적 콘텐츠일 경우 = cgi-bin이 포함되어 있다.
    ptr = index(uri, '?'); //파일이름과 인자를 구분하는 구분자 = ?
    if(ptr) {
      strcpy(cgiargs, ptr + 1);
      *ptr = '\0';
    }
    else {
      strcpy(cgiargs, "");
    }
    strcpy(filename, ".");
    strcat(filename, uri);
    return 0;
  }
}

/*
 * Tiny server static content = HTML file, Text file, GIF, PNG, JPEG
 */
void serve_static(int fd, char *filename, int filesize, char *method) {
  int srcfd;
  char *srcp, filetype[MAXLINE], buf[MAXBUF];

  get_filetype(filename, filetype); //파일 확장자로 type찾기

  //응답 헤더 생성
  sprintf(buf, "HTTP/1.1 200 OK\r\n");
  sprintf(buf, "%sServer: Tiny Web Server\r\n", buf);
  sprintf(buf, "%sConnection : close\r\n", buf);
  sprintf(buf, "%sContent-length: %d\r\n", buf, filesize);
  sprintf(buf, "%sContent-type: %s\r\n\r\n", buf, filetype);
  Rio_writen(fd, buf, strlen(buf));
  printf("Response header:\n");
  printf("%s", buf);

  if(strcasecmp(method, "GET") == 0) {
    srcfd = Open(filename, O_RDONLY, 0); //읽기 전용으로 파일 열기
    //Mamp() = 메모리 매핑 함수
    //파일 안의 내용을 메모리 매핑하여 srcp에 저장
    // srcp = Mmap(0, filesize, PROT_READ, MAP_PRIVATE, srcfd, 0);
    srcp = (char *)malloc(filesize); //malloc사용하기
    rio_readn(srcfd, srcp, filesize);
    Close(srcfd);
    Rio_writen(fd, srcp, filesize);
    // Munmap(srcp, filesize);
    free(srcp); //malloc 해제하기
  }
}

/*
 * 파일의 확장자로 파일 타입을 구하는 함수
 */
void get_filetype(char *filename, char *filetype) {
  if (strstr(filename, ".html")) {
    strcpy(filetype, "text/html");
  }
  else if(strstr(filename, ".gif")) {
    strcpy(filetype, "image/gif");
  }
  else if(strstr(filename, ".png")) {
    strcpy(filetype, "image/png");
  }
  else if(strstr(filename, ".jpg")) {
    strcpy(filetype, "image/jpeg");
  }
  else if(strstr(filename, ".mp4")) { //mp4영상을 위한 타입 설정
    strcpy(filetype, "video/mp4");
  }
  else {
    strcpy(filetype, "text/plain");
  }
}

/*
 * 동적 콘텐츠 제공
 */
void serve_dynamic(int fd, char *filename, char *cgiargs, char *method) {
  char buf[MAXLINE], *emptylist[] = { NULL };

  //응답 헤더 생성
  sprintf(buf, "HTTP/1.1 200 OK\r\n");
  Rio_writen(fd, buf, strlen(buf));
  sprintf(buf, "Server: Tiny Web Server\r\n");
  Rio_writen(fd, buf, strlen(buf));

  //자식 프로세서 생성
  if(Fork() == 0) {
    setenv("QUERY_STRING", cgiargs, 1); //환경변수 설정
    setenv("REQUEST_METHOD", method, 1);
    Dup2(fd, STDOUT_FILENO); //표준 출력을 클라이언트 소켓으로 redirection
    Execve(filename, emptylist, environ); //CGI 프로그램 실행
  }
  //부모 프로세스는 자식 프로세스가 종료될 때까지 대기
  Wait(NULL);
}