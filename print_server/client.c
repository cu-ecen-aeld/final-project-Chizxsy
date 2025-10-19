#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <stdlib.h>
#include <syslog.h>

#define PORT "54321"
#define BUFFER_SIZE 1024


int main(int argc, char *argv[]) {
    int client_sockfd, status; 
    int sockarg;
    struct addrinfo hints, *res, *p;
    struct sockaddr_storage serveraddr;
    socklen_t serveraddr_len;
    struct linger opt;

    if (argc != 2) {
        fprintf(stderr, "Usage: %s <hostname> <filepath>\n", argv[0]);
        return -1;
    }

    const char *hostname = argv[1];

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    if ((status = getaddrinfo(hostname, PORT, &hints, &res)) != 0) {
        syslog(LOG_ERR, "getaddrinfo: %s", gai_strerror(status));
        return -1;
    }

    opt.l_onoff = 1;
    opt.l_linger = 0;
    sockarg = 1;

    for (p=res; p!=NULL; p=p->ai_next){
        if((client_sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) continue;

        setsockopt(client_sockfd, SOL_SOCKET, SO_REUSEADDR, (char*)&sockarg, sizeof(sockarg));
        setsockopt(client_sockfd, SOL_SOCKET, SO_LINGER, (char*)&opt, sizeof(opt));

        if (connect(client_sockfd, p->ai_addr, p->ai_addrlen) == -1){
            syslog(LOG_ERR, "Client connect");
            continue;
        }

        break;

    }
    freeaddrinfo(res);

    if (p == NULL){
        syslog(LOG_ERR, "Client connection failed");
        return -1;
    }

    syslog(LOG_INFO, "Connected to %s\n", hostname);

    char buffer[BUFFER_SIZE];

    while (1) {
        memset(buffer, 0, BUFFER_SIZE);
        ssize_t bytes_recv = recv(client_sockfd, buffer, BUFFER_SIZE -1, 0);

        if (bytes_recv > 0){
            printf("%s", buffer);
        } else if {
            syslog(LOG_INFO, "Server disconnected during startup");
            close(client_sockfd);
            return -1;
        }
    }

    // printer handling code
    const char *gcode_cmd = "G28";
    syslog(LOG_INFO, "Sending %s", gcode_cmd);
    if (send(client_sockfd, gcode_cmd, strlen(gcode_cmd), 0) < 0) {
        syslog(LOG_ERR, "send command");
        close(client_sockfd);
        return -1;
    }



    close(client_sockfd);

}
