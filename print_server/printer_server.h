#ifndef PRINTER_SERVER_H
#define PRINTER_SERVER_H

// --- Standard Library Includes ---
#include <termios.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <syslog.h>
#include <string.h>
#include <time.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <signal.h>
#include <sys/select.h>
#include <sys/wait.h>
#include <errno.h>

// --- Global Constants ---
#define PORT "54321"
#define BUFFER_SIZE 2048

// --- Function Prototypes ---

/**
 * @brief Configures and opens the serial port for communication.
 * @param dev_path The file system path to the serial device (e.g., "/dev/ttyACM0").
 * @return The file descriptor for the serial port on success, -1 on failure.
 */
int setup_serial_port(const char *dev_path);

/**
 * @brief Bridges data between the TCP client and the serial port.
 * @param client_sockfd The socket file descriptor for the connected client.
 * @param serial_dev_path The path to the serial device.
 */
void handle_client_printer_bridge(int client_sockfd, const char *serial_dev_path);

#endif // PRINTER_SERVER_H