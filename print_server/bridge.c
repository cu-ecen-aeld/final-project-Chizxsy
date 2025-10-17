#include "printer_server.h"

/**
 * @brief Bridges data between the TCP client and the serial port.
 * Assistance from Google Gemini was used for the select command and writing all of the syslogs. 
 */
void handle_client_printer_bridge(int client_sockfd, const char *serial_dev_path) {
    int serial_fd;
    char buffer[BUFFER_SIZE];
    ssize_t bytes_read;
    fd_set read_fds;
    int max_fd;

    syslog(LOG_INFO, "Setting up serial port: %s", serial_dev_path);
    serial_fd = setup_serial_port(serial_dev_path);
    if (serial_fd < 0) {
        syslog(LOG_ERR, "Could not initialize serial port. Closing connection.");
        send(client_sockfd, "Error: Could not open printer connection.\n", 42, 0);
        return;
    }

    syslog(LOG_INFO, "Bridging connection between client and printer.");

    max_fd = (client_sockfd > serial_fd) ? client_sockfd : serial_fd;

    while (1) {
        FD_ZERO(&read_fds);
        FD_SET(client_sockfd, &read_fds);
        FD_SET(serial_fd, &read_fds);

        if (select(max_fd + 1, &read_fds, NULL, NULL, NULL) < 0) {
            if (errno == EINTR) continue; // Interrupted by signal, just restart
            syslog(LOG_ERR, "select() error: %m");
            break;
        }

        if (FD_ISSET(client_sockfd, &read_fds)) {
            bytes_read = recv(client_sockfd, buffer, sizeof(buffer), 0);
            if (bytes_read <= 0) {
                syslog(LOG_INFO, "Client disconnected or recv error.");
                break;
            }
            if (write(serial_fd, buffer, bytes_read) < 0) {
                 syslog(LOG_ERR, "write() to serial port failed: %m");
                 break;
            }
        }

        if (FD_ISSET(serial_fd, &read_fds)) {
            bytes_read = read(serial_fd, buffer, sizeof(buffer));
            if (bytes_read <= 0) {
                syslog(LOG_ERR, "Serial device disconnected or read error.");
                break;
            }
            if (send(client_sockfd, buffer, bytes_read, 0) < 0) {
                 syslog(LOG_ERR, "send() to client failed: %m");
                 break;
            }
        }
    }

    syslog(LOG_INFO, "Closing bridge for client.");
    close(serial_fd);
}