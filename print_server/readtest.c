#include <termios.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <syslog.h>
#include <string.h>
#include <time.h>
#include <sys/ioctl.h>
#include "printer_server.h"

#define BUFFER_SIZE 1024

int printer_startup(int serial_port, int timeout_sec, const char *dev_path) {
    char buffer[BUFFER_SIZE];
    memset(buffer, 0, sizeof(buffer));
    int total_read = 0;
    int read_serial = 0;
    time_t start_time = time(NULL);

    printf("----- Welcome to printer startup -----\n");
    printf("Start time: %s\n", ctime(&start_time));

    while ((time(NULL) - start_time) < timeout_sec) {
        memset(buffer, 0, sizeof(buffer));
        printf("Reading from %s...\n", dev_path);
        read_serial = read(serial_port, buffer, sizeof(buffer) - 1);

        if (read_serial > 0) {
            total_read += read_serial;
            buffer[read_serial] = '\0';
            printf("--- Printer Startup Output ---\n");
            printf("%s", buffer);
            printf("------------------------------\n");
        } else if (read_serial < 0) {
            perror("read");
        }
    }

    if (total_read == 0) {
        syslog(LOG_ERR, "Waiting for printer to become ready (no data received)");
    }

    time_t end_time = time(NULL);
    printf("----- End of printer startup -----\n");
    printf("End time: %s\n", ctime(&end_time));

    return (total_read > 0) ? 0 : -1;
}

int main(int argc, char *argv[]){

    struct termios tty;
    int dtr = TIOCM_DTR;

    // open syslog logging
    openlog("PrintrBoardUSB", LOG_PID, LOG_USER);

    // check for correct number of arguments
    if (argc < 2){
        fprintf(stderr, "Incorect number of arguments %s\n", argv[1]);
        return -1;
    }
    
    char *dev_path = argv[1];

    // open serial to 3d printer
    int serial_port = open(dev_path, O_RDWR | O_NOCTTY);

    if (serial_port < 0){
        syslog(LOG_ERR, "Failed to open serial port");
        return -1;
    }

    // get current attributes
    if (tcgetattr(serial_port, &tty) != 0){
        syslog(LOG_ERR, "Failed to store attributes");
        close(serial_port);
        return -1;
    }

    // configure baud rate
    //tx
    cfsetospeed(&tty, B115200);
    //rx
    cfsetispeed(&tty, B115200);
    
    // configure raw mode
    // void cfmakeraw(serial_port, &tty);
    tty.c_iflag &= ~(IGNBRK | BRKINT | PARMRK | ISTRIP | INLCR | IGNCR | ICRNL | IXON);
    tty.c_oflag &= ~OPOST;
    tty.c_lflag &= ~(ECHO | ECHONL | ICANON | ISIG | IEXTEN);
    tty.c_cflag &= ~(CSIZE | PARENB);
    tty.c_cflag |= CS8;

    // sets connection timeout to 10 sec
    tty.c_cc[VMIN] = 1;
    tty.c_cc[VTIME] = 0;
    
    // set attributes now
    if (tcsetattr(serial_port, TCSANOW, &tty) != 0){
            syslog(LOG_ERR, "Failed to store updated attributes");
            close(serial_port);
            return -1;
    }


    ioctl(serial_port, TIOCMBIC, &dtr);  // clear DTR
    usleep(100000);                      // 100ms delay
    ioctl(serial_port, TIOCMBIS, &dtr);  // set DTR again

    sleep(1);

    char startup_buffer[BUFFER_SIZE];
    memset(startup_buffer, '\n', sizeof(startup_buffer));

    if(printer_startup(serial_port, 10, dev_path) != 0){
        syslog(LOG_ERR,"Failed to monitor startup");
        return -1;
    }

    // ----- for testing -----
    // report settings with M503
    /*
    char *gcode_cmd = "M503\n";
    syslog(LOG_INFO, "Writing gcode command: %s\n", gcode_cmd);
    serial_writen(serial_port, gcode_cmd, strlen(gcode_cmd));
*/  
    close(serial_port);
    closelog();
    return 0;

}
