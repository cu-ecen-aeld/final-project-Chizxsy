#include "printer_server.h"
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <syslog.h>
#include <signal.h>

int setup_server(const char *port){
    int listen_sockfd = 0;
    struct addrinfo hints, *res;

    
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    if ((getaddrinfo(NULL, port, &hints, &res)) != 0){
        syslog(LOG_ERR, "getaddrinfo");
        return -1;
    }

    if ((listen_sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol)) == -1){
        syslog(LOG_ERR, "socket");
        return -1;
    }

    int sockarg = 1;
    setsockopt(listen_sockfd, SOL_SOCKET, SO_REUSEADDR, (char*)&sockarg, sizeof(sockarg));

    if (bind(listen_sockfd, res->ai_addr, res->ai_addrlen) == -1){
        syslog(LOG_ERR, "bind");
        close(listen_sockfd);
        return -1;
    }

    freeaddrinfo(res);

    if (listen(listen_sockfd, 10) == -1){
        syslog(LOG_ERR, "listen");
        return -1;
    }

    return listen_sockfd;
}
