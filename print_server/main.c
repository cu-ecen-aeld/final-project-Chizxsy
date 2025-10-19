#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <syslog.h>
#include "serial_lib.h"

#define BUFFER_SIZE 1024


int main (int argc, char *argv[]){

    openlog("serial-rdwr", LOG_PID, LOG_USER);

    // check for correct number of arguments
    if (argc < 2){
        syslog(LOG_ERR, "Usage: Incorrect number of arguments %s\n", argv[0]);
        return -1;
    }
    
    const char *dev_path = argv[1];

    // initialize the serial port at 115200 baudrate
    int serial_port = serial_setup(dev_path);
    if (serial_port < 0){
        syslog(LOG_ERR, "Failed to initialize serial port %s\n", dev_path);
        return -1;
    }

    syslog(LOG_INFO, "Serial port init %s with fd = %d\n", dev_path, serial_port);

    // ---- Read Prusa startup data ----
    char buffer[BUFFER_SIZE];

    while (1){
        memset(buffer, 0, sizeof(buffer));

        int read_data = serial_read(serial_port, buffer, sizeof(buffer));
        if (read_data < 0){
            syslog(LOG_ERR, "Failed to ready serial_data");
            return -1;
        }
        printf("Printer reponse: %s", buffer);

        if (read_data == 0){
            syslog(LOG_INFO, "Response received");
            break;
        }
    }

    // ---- Send G28 Home command to printer ----
    char write_buffer[BUFFER_SIZE];
    memset(write_buffer, 0, sizeof(write_buffer));

    char* gcode_cmd = "G28";

    snprintf(write_buffer, sizeof(write_buffer), "%s\n", gcode_cmd);
    syslog(LOG_INFO, "Sending command: %s", gcode_cmd);

    int write_data = serial_write(serial_port, write_buffer);
    if (write_data < 0){
        syslog(LOG_ERR, "Failed to write %s to printer", gcode_cmd);
        close(serial_port);
        return -1;
    }

    // ---- Read response from G28 ----

    while (1){
        memset(buffer, 0, sizeof(buffer));

        int read_data = serial_read(serial_port, buffer, sizeof(buffer));
        if (read_data < 0){
            syslog(LOG_ERR, "Failed to read printer response");
            return -1;
        }

        if (read_data == 0){
            syslog(LOG_INFO, "Timmed out");
            break;
        }

        if (strstr(buffer, "ok") != NULL){
            syslog(LOG_INFO, "Success!");
            break;
        }
    }

    close(serial_port);
    closelog();
    return 0;
}