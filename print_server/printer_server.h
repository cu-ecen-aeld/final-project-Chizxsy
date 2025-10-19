#ifndef PRINTER_SERVER_H
#define PRINTER_SERVER_H

#include <termios.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <syslog.h>
#include <string.h>
#include <time.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <signal.h>
#include <sys/select.h>
#include <sys/wait.h>
#include <errno.h>

int setup_server(const char *port);

#endif // PRINTER_SERVER_H