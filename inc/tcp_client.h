#ifndef TCPCLIENT_H
#define TCPCLIENT_H

#include <stdio.h>
#include <syslog.h>

// Define constants for the client
#define PORT "54321"
#define BUFFER_SIZE 1024

/**
 * @brief Sends the contents of a file over a socket.
 *
 * @param sockfd The socket file descriptor to send data to.
 * @param filename The path to the file to be sent.
 * @return 0 on success, -1 on failure.
 */
int send_data(int sockfd, const char *filename);

/**
 * @brief Receives a response message from the server and logs it.
 *
 * @param sockfd The socket file descriptor to receive data from.
 * @return 0 on success, -1 on failure.
 */
int receive_data(int sockfd);

#endif // TCPCLIENT_H