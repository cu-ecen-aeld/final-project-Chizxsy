#ifndef TCPSERVER_H
#define TCPSERVER_H

#include <stdio.h>
#include <syslog.h>
#include <sys/socket.h>

// Define constants used by the server
#define PORT "54321"
#define BUFFER_SIZE 1024

/**
 * @brief Receives data from a socket and writes it to a file.
 * * @param sockfd The socket file descriptor to receive data from.
 * @param filename The name of the file to save the received data to.
 * @return 0 on success, -1 on failure.
 */
int receive_data(int sockfd, const char *filename);
int send_data(int sockfd, const char *filename);



#endif // TCPSERVER_H
