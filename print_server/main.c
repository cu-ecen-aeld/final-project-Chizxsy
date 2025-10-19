#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <syslog.h>
#include <sys/select.h>
#include <netinet/in.h>

#include "serial_lib.h"
#include "printer_server.h"

#define PORT "54321"
#define BUFFER_SIZE 1024

int main (int argc, char *argv[]){
    int serial_port, listen_fd, client_fd;
    struct sockaddr_storage client_addr;
    socklen_t addr_size;
    fd_set read_fds;

    char buffer[BUFFER_SIZE];

    openlog("serial-rdwr", LOG_PID, LOG_USER);

    // check for correct number of arguments
    if (argc < 2){
        syslog(LOG_ERR, "Usage: Incorrect number of arguments %s\n", argv[0]);
        return -1;
    }
    
    const char *dev_path = argv[1];

    // initialize the serial port at 115200 baudrate
    serial_port = serial_setup(dev_path);
    if (serial_port < 0){
        syslog(LOG_ERR, "Failed to initialize serial port %s\n", dev_path);
        return -1;
    }

    syslog(LOG_INFO, "Serial port initialized %s with fd = %d\n", dev_path, serial_port);

    listen_fd = setup_server(PORT);
    if (listen_fd < 0){
        syslog(LOG_ERR, "Failed to setup server port %s with fd = %d\n", PORT, listen_fd);
        close(serial_port);
        return -1;
    }
    
    syslog(LOG_INFO, "TCP Server initialized %s with fd = %d\n", PORT, listen_fd);


    // ---- Main loop ----
    while(1){
        addr_size = sizeof(client_addr);
        syslog(LOG_INFO, "Waiting for new connection...");
        
        client_fd = accept(listen_fd, (struct sockaddr *)&client_addr, &addr_size);
        if (client_fd < 0){
            syslog(LOG_INFO, "Accept failed continuing...");
            continue;
        }
        
        syslog(LOG_INFO, "Client connected fd = %d\n", client_fd);

        // ---- send recv loop ----
        while (1){
            FD_ZERO(&read_fds);
            FD_SET(serial_port, &read_fds);
            FD_SET(client_fd, &read_fds);

            int max_fd = (serial_port > client_fd) ? serial_port : client_fd;

            int retval = select(max_fd + 1, &read_fds, NULL, NULL, NULL);
            if (retval < 0) {
                 syslog(LOG_ERR, "Select failed");
                 break;
            }

            // Handle printer FD
            if (FD_ISSET(serial_port, &read_fds)){
                memset(buffer, 0, BUFFER_SIZE);
                ssize_t bytes_read = serial_read(serial_port, buffer, BUFFER_SIZE);

                if (bytes_read > 0){
                    if (send(client_fd, buffer, bytes_read, 0) < 0){
                        syslog(LOG_ERR, "Send failed");
                        break;
                    }
                }
            }

            // Handle client FD
            if (FD_ISSET(client_fd, &read_fds)){
                memset(buffer, 0, BUFFER_SIZE);
                ssize_t bytes_recv = recv(client_fd, buffer, BUFFER_SIZE -1, 0);

                if (bytes_recv < 0 ){
                    syslog(LOG_ERR, "client recv error");
                    break;
                }

                if (bytes_recv == 0){
                    syslog(LOG_INFO, "Client disconnected");
                    break;
                }

                if (bytes_recv > 0){
                    serial_write(serial_port, buffer, bytes_recv);
                }
            }

        }

    }
    close(listen_fd);
    close(serial_port);
    closelog();
    return 0;
}