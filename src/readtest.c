#define _GNU_SOURCE
#include <termios.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <syslog.h>
#include <string.h>
#include <time.h>
#include <errno.h>

#define BUFFER_SIZE 2048 // Increased buffer size just in case

/**
 * @brief Reads from the serial port for a fixed duration and prints all output.
 */
void monitor_startup(int serial_port, int duration_sec) {
    char buffer[BUFFER_SIZE];
    memset(buffer, '\0', sizeof(buffer));
    int total_read = 0;
    int read_serial = 0;
    time_t start_time = time(NULL);

    printf("--- Monitoring printer output for %d seconds ---\n", duration_sec);

    // Loop for the total duration
    while ((time(NULL) - start_time) < duration_sec) {
        // FIX: Read into the correct position in the buffer to *append* data
        read_serial = read(serial_port, &buffer[total_read], BUFFER_SIZE - total_read - 1);
        
        if (read_serial > 0) {
            total_read += read_serial;
        }
        // No need to print inside the loop, we'll print it all at the end.
        // A small sleep prevents this loop from consuming 100% CPU.
        usleep(10000); 
    }

    printf("--- Total Bytes Read: %d ---\n", total_read);
    printf("--- Full Startup Output ---\n");
    printf("%s\n", buffer);
    printf("---------------------------\n");
    
    syslog(LOG_INFO, "Finished monitoring startup. Bytes read: %d", total_read);
}


int main(int argc, char *argv[]) {
    struct termios tty;
    openlog("PrintrBoardUSB", LOG_PID, LOG_USER);

    // FIX: Corrected argument check and message
    if (argc < 2) {
        fprintf(stderr, "Incorrect number of arguments. Usage: %s /path/to/device\n", argv[0]);
        return -1;
    }

    char *dev_path = argv[1];
    int serial_port = open(dev_path, O_RDWR | O_NOCTTY | O_NDELAY);

    if (serial_port < 0) {
        syslog(LOG_ERR, "Failed to open serial port %s: %s", dev_path, strerror(errno));
        return -1;
    }

    if (tcgetattr(serial_port, &tty) != 0) {
        syslog(LOG_ERR, "Failed to get attributes: %s", strerror(errno));
        close(serial_port);
        return -1;
    }

    cfsetospeed(&tty, B115200);
    cfsetispeed(&tty, B115200);

    tty.c_cflag |= (CLOCAL | CREAD);
    tty.c_cflag &= ~CSIZE;
    tty.c_cflag |= CS8;
    tty.c_cflag &= ~PARENB;
    tty.c_cflag &= ~CSTOPB;
    tty.c_cflag &= ~CRTSCTS;
    tty.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
    tty.c_iflag &= ~(IXON | IXOFF | IXANY);
    tty.c_oflag &= ~OPOST;

    // This sets the timeout for each individual read() call to 1 second.
    // Your main loop controls the total duration.
    tty.c_cc[VMIN] = 0;
    tty.c_cc[VTIME] = 10; 

    if (tcsetattr(serial_port, TCSANOW, &tty) != 0) {
        syslog(LOG_ERR, "Failed to set attributes: %s", strerror(errno));
        close(serial_port);
        return -1;
    }

    tcflush(serial_port, TCIOFLUSH);

    // Call the simplified monitoring function. It doesn't need an if check.
    monitor_startup(serial_port, 10);

    close(serial_port);
    closelog();
    return 0;
}#include <termios.h>
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
