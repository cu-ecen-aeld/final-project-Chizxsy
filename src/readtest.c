#include <termios.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <syslog.h>
#include <string.h>
#include <time.h>

#define BUFFER_SIZE 1024

int printer_startup(int serial_port, int timeout_sec){
    char buffer[BUFFER_SIZE];
    memset(buffer, '\0', sizeof(buffer));
    int total_read = 0;
    int read_serial = 0;
    time_t start_time = time(NULL);
    time_t time1, time2;

    time(&time1);

    printf("----- Welcome to printer startup -----\n");
    printf("Start time: %s\n", ctime(&time1));


    while ((time(NULL) - start_time) < timeout_sec){
        read_serial = read(serial_port, buffer, sizeof(buffer));
        if (read_serial > 0){
            total_read += read_serial;
            printf("--- Printer Startup Output ---\n");
            printf("%s\n", buffer);
            printf("------------------------------\n");   
        }
    }
    syslog(LOG_ERR, "Waiting for printer to become ready");
    time(&time2);
    printf("----- End of printer startup -----\n");
    printf("Start time: %s\n", ctime(&time2));

    return -1;

}

int main(int argc, char *argv[]){

    struct termios tty;

    // open syslog logging
    openlog("PrintrBoardUSB", LOG_PID, LOG_USER);

    // check for correct number of arguments
    if (argc < 2){
        fprintf(stderr, "Incorect number of arguments %s\n", argv[1]);
        return -1;
    }
    
    char *dev_path = argv[1];

    // open serial to 3d printer
    int serial_port = open(dev_path, O_RDWR | O_NOCTTY | O_NDELAY);

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
    tty.c_cc[VMIN] = 0;
    tty.c_cc[VTIME] = 10;
    
    // set attributes now
    if (tcsetattr(serial_port, TCSANOW, &tty) != 0){
            syslog(LOG_ERR, "Failed to store updated attributes");
            close(serial_port);
            return -1;
    }

    tcflush(serial_port, TCIOFLUSH); 

    char startup_buffer[BUFFER_SIZE];
    memset(startup_buffer, '\n', sizeof(startup_buffer));

    if(printer_startup(serial_port, 10) != 0){
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
