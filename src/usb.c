#include <termios.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <syslog.h>
#include <string.h>

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

int main(int argc, char *argv[]){

    struct terminos tty;

    // open syslog logging
    openlog("PrintrBoardUSB", LOG_PID, LOG_USER);


    // check for correct number of arguments
    if (argc < 2){
        fprintf(stderr, "Incorect number of arguments %s\n", argv[1]);
        return -1;
    }
    
    char dev_path = argv[1];

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

    tty.c_cc[VMIN] = 0;
    tty.c_cc[VTIME] = 10;
    
    // set attributes now
    if (tcsetattr(serial_port, TCSANOW, &tty) != 0){
            syslog(LOG_ERR, "Failed to store updated attributes");
            close(serial_port);
            return -1;
    }


    tcflush(serial_port, TCIOFLUSH); 

    // ----- for testing -----
    // report settings with M503
    char *gcode_cmd = "M503\n";
    syslog(LOG_INFO, "Writing gcode command: %s\n", gcode_cmd);
    serial_writen(serial_port, gcode_cmd, strlen(gcode_cmd));

    // ----- read response -----
    char read_buffer[2048];
    memset(&read_buffer, '\0', sizeof(read_buffer));
    int serial_read = 0;
    int counter = 0; 
    char buf = '\0';

    syslog(LOG_INFO, "Reading response from printer");

    do {
        serial_read = read(serial_port, &buf, 1);
        if (serial_read > 0){
            read_buffer[counter] = buf;
            counter++;
        }

    } while (strnstr(read_buffer, "ok", strlen("ok")) && counter < sizeof(read_buffer) - 1); 

    if (counter > 0){
        syslog(LOG_INFO, "Success reading from printer!");
        printf("Printer Response: \n%s\n", read_buffer);
    } else {
        syslog(LOG_ERR, "Failed to read from printer");
        printf("Failed to read fom printer");
    }

    close(serial_port);
    closelog();
    return 0;

}
