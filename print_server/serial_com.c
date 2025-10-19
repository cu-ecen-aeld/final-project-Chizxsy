#include <termios.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <syslog.h>
#include <string.h>
#include <time.h>
#include <sys/ioctl.h>
//#include "printer_server.h"

#define BUFFER_SIZE 1024

int serial_read(int serial_port, char *buffer, size_t bufsize) {

    printf("---- Serial Read ----\n");

    ssize_t read_serial = read(serial_port, buffer, bufsize - 1);

    if (read_serial > 0) {
        buffer[read_serial] = '\0';
        printf("--- Printer Startup Output ---\n");
        printf("%s", buffer);
        return (int)read_serial;

    } else if (read_serial < 0) {
        perror("read");
        return -1;
    } else {
        return 0;
    }
    
   // return (read_serial > 0) ? (int)read_serial : -1;
}

int serial_write(int serial_port, const char *buffer){
    printf("---- Serial Write ----\n");

    ssize_t write_serial = write(serial_port, buffer, strlen(buffer));

    if (write_serial < 0){
        perror("write");
        return -1;
    }

    printf("--- Writing %zd bytes ---\n", write_serial);
    printf("%s", buffer);

    return (int)write_serial;
}

int serial_setup(const char *dev_path){

    struct termios tty;
    int dtr = TIOCM_DTR;


    // open serial to 3d printer
    int serial_port = open(dev_path, O_RDWR | O_NOCTTY);
    if (serial_port < 0){
        perror("serial port");
        return -1;
    }

    // get current attributes
    if (tcgetattr(serial_port, &tty) != 0){
        perror("tcgetattr");
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

    // sets connection timeout to 1 sec
    tty.c_cc[VMIN] = 0;
    tty.c_cc[VTIME] = 10;
    
    // set attributes now
    if (tcsetattr(serial_port, TCSANOW, &tty) != 0){
        perror("tcsetattr");
        close(serial_port);
        return -1;
    }

    ioctl(serial_port, TIOCMBIC, &dtr);  // clear DTR
    usleep(100000);                      // 100ms delay
    ioctl(serial_port, TIOCMBIS, &dtr);  // set DTR again

    printf("Serial port %s init 115200 baud\n", dev_path);
    return serial_port;

}
