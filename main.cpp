#include <iostream>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <termios.h>

const char SERIAL_PORT[] = "/dev/ttyPS0";

int openPort(void) {
    int fd = open("/dev/ttyPS0", O_RDWR | O_NOCTTY | O_NDELAY);
    if (fd == -1)
        perror("openPort: Unable to serial port.");
    else {
        fcntl(fd, F_SETFL, 0);
        std::cout << "Success connecting to " << "serial port." << std::endl;
    }
    return(fd);
}

int main() {
    openPort();
    return 0;
}