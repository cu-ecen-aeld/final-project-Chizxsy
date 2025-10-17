#include "printer_server.h"

int setup_serial_port(const char *dev_path) {
    struct termios tty;
    int serial_port;
    int dtr = TIOCM_DTR;

    serial_port = open(dev_path, O_RDWR | O_NOCTTY);
    if (serial_port < 0) {
        syslog(LOG_ERR, "Failed to open serial port %s: %m", dev_path);
        return -1;
    }

    // Get the current attributes of the serial port.
    if (tcgetattr(serial_port, &tty) != 0) {
        syslog(LOG_ERR, "Failed to get attributes for %s: %m", dev_path);
        close(serial_port);
        return -1;
    }

    // Serial baud rate
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

    if (tcsetattr(serial_port, TCSANOW, &tty) != 0) {
        syslog(LOG_ERR, "Failed to set attributes for %s: %m", dev_path);
        close(serial_port);
        return -1;
    }

    // --- Reset the Printer Board ---
    syslog(LOG_INFO, "Toggling DTR on %s to reset printer board...", dev_path);
    // Clear the DTR line.
    ioctl(serial_port, TIOCMBIC, &dtr);
    usleep(100000); // Wait 100ms.
    // Set the DTR line again.
    ioctl(serial_port, TIOCMBIS, &dtr);
    
    sleep(2);

    syslog(LOG_INFO, "Serial port %s is configured and ready.", dev_path);

    return serial_port;
}