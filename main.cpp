#include <iostream>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <termios.h>

const std::string SERIAL_PORT = "/dev/ttyPS0";

int openPort(void) {
    int fd = open("/dev/ttyPS0", O_RDWR | O_NOCTTY | O_NDELAY);
    if (fd == -1)
        perror("openPort: Unable to open " + SERIAL_PORT);
    else {
        fcntl(fd, F_SETFL, 0);
        std::cout << "Success connecting to " << SERIAL_PORT << std::endl;
    }
    return(fd);
}

int main() {
    openPort();
    return 0;
}