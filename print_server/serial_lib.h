#ifndef SERIAL_LIB_H
#define SERIAL_LIB_H

#include <string.h>

int serial_read(int serial_port, char *buffer, size_t bufsize);
int serial_write(int serial_port, const char *buffer, size_t bufsize);
int serial_setup(const char *dev_path);

#endif