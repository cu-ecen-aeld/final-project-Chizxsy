#include <terminos.h>
#include <stdio.h>
#include <unstd.h>
#include <fnctl.h>
#include <stdlib.h>


int read_serial(){

}

int write_serial(){

}


int main(int argc, char *argv[]){
    char dev_path;
    struct terminos tty;
    // check for correct number of arguments
    if (argc < 2){
        fprintf(stderr, "Incorect number of arguments %s\n", argv[1]);
        return 1;
    }
    
    dev_path = argv[1];

    // open serial to 3d printer
    int fd = open(dev_path, O_RDWR | O_NOCTTY | O_NDELAY);

    if (fd < 0){
        fprintf("Failed to open file");
        return 1;
    }

    if (tcgetattr(fd, tty)){
        fprintf("Failed to store attributes");
        close(fd);
    }

    
    
}