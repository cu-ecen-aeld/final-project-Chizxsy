CC = gcc
CFLAGS = -Wall -Wextra -g

TARGET = printer_server

SRCS = printer_server.c port_handler.c bridge.c

# generate object files from source
OBJS = $(SRCS:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(TARGET)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(TARGET) $(OBJS)