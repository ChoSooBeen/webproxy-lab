####################################################################
# CS:APP Proxy Lab
#
# Student Source Files
####################################################################

## :hourglass: 2023.05.19 ~ 2023.05.25
## :bulb: 네트워크 및 웹서버 학습

:bookmark: 블로그 정리
- 네트워크 개념 : https://soo-note.tistory.com/75
- echo server : https://soo-note.tistory.com/76
- web server : https://soo-note.tistory.com/77
- tiny server : https://soo-note.tistory.com/78
- proxy server : https://soo-note.tistory.com/79

---
This directory contains the files you will need for the CS:APP Proxy
Lab.

proxy.c
csapp.h
csapp.c
    These are starter files.  csapp.c and csapp.h are described in
    your textbook. 

    You may make any changes you like to these files.  And you may
    create and handin any additional files you like.

    Please use `port-for-user.pl' or 'free-port.sh' to generate
    unique ports for your proxy or tiny server. 

Makefile
    This is the makefile that builds the proxy program.  Type "make"
    to build your solution, or "make clean" followed by "make" for a
    fresh build. 

    Type "make handin" to create the tarfile that you will be handing
    in. You can modify it any way you like. Your instructor will use your
    Makefile to build your proxy from source.

port-for-user.pl
    Generates a random port for a particular user
    usage: ./port-for-user.pl <userID>

free-port.sh
    Handy script that identifies an unused TCP port that you can use
    for your proxy or tiny. 
    usage: ./free-port.sh

driver.sh
    The autograder for Basic, Concurrency, and Cache.        
    usage: ./driver.sh

nop-server.py
     helper for the autograder.         

tiny
    Tiny Web server from the CS:APP text

