#include <termios.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <syslog.h>
#include <string.h>
#include <time.h>

#define BUFFER_SIZE 1024

//https://stackoverflow.com/questions/23999797/implementing-strnstr

char *strnstr(const char *haystack, const char *needle, size_t len){
    if (*needle == '\0'){
        return (char *)haystack;
    }

    size_t needle_len = strlen(needle);
    if (needle_len == 0 || needle_len > len) {
        return NULL;
    }

    for (size_t i = 0; i <= len - needle_len; i++) {
        if (haystack[i] == *needle && strncmp(&haystack[i], needle, needle_len) == 0) {
            return (char *)&haystack[i];
        }
    }

    return NULL;
}

/* Right out of Steven's */
ssize_t serial_writen(int fd, const void *vptr, size_t n)
{
  ssize_t nleft;
  ssize_t nwritten;
  const char *ptr;
 
  ptr=vptr;
  nleft=n;
 
  while(nleft > 0)
  {
    if((nwritten=write(fd, ptr, nleft)) <=0)
      return nwritten;
 
    nleft -= nwritten;
    ptr += nwritten;
  }
  return n;
}

int printer_startup(int serial_port, char *output_buffer, size_t buffer_size, const char *ready_str, int timeout_sec){
    char buffer[BUFFER_SIZE];
    memset(buffer, '\0', sizeof(buffer));
    int total_read = 0;
    int read_serial = 0;
    time_t start_time = time(NULL);

    // On startup Prusa prints to the console:
    /* start
    echo: 3.13.3-7094
    SpoolJoin is Off
    echo: Last Updated: Feb 27 2024 18:21:24 | Author: (none, default config)
    echo: Free Memory: 2809 
    PlannerBufferBytes: 1760
    echo:Stored settings retrieved
    adc_init
    Sending 0xFF
    echo:SD card ok */

    while ((time(NULL) - start_time) < timeout_sec){
        read_serial = read(serial_port, &buffer[total_read], sizeof(buffer) - total_read - 1);
        if (read_serial > 0){
            total_read += read_serial;
            if (strnstr(buffer, ready_str, total_read)){
                syslog(LOG_INFO, "Printer is ready!");
                tcflush(serial_port, TCIFLUSH);

                // copy to output buffer
                strncpy(output_buffer, buffer, buffer_size -1);
                output_buffer[buffer_size - 1] = '\0';
                return 0;
            }
        }
        usleep(10000); 
    }

    syslog(LOG_ERR, "Waiting for printer to become ready");
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
    memset(startup_buffer, '\0', sizeof(startup_buffer));

    if(printer_startup(serial_port, startup_buffer, sizeof(startup_buffer), "SD card ok", 10) != 0){
        syslog(LOG_ERR,"Failed to monitor startup");
        return -1;
    }

    printf("--- Printer Startup Output ---\n");
    printf("%s\n", startup_buffer);
    printf("------------------------------\n");



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
