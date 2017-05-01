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
    int             fd = 0;     // File descriptor
    struct termios  options;    // Terminal options
    int             rc;         // Return value

    fd = openPort();

    if ((rc = tcgetattr(fd, &options)) < 0) {
        perror("failed to get attr.");
        return -1;
    }

    cfsetispeed(&options, B115200);
    cfsetospeed(&options, B115200);

    cfmakeraw(&options);
    options.c_cflag     |= (CLOCAL | CREAD);   // Enable the receiver and set local mode
    options.c_cflag     &= ~CSTOPB;            // 1 stop bit
    options.c_cflag     &= ~CRTSCTS;           // Disable hardware flow control
    options.c_cc[VMIN]   = 1;
    options.c_cc[VTIME]  = 2;

    // Set new attributes
    if ((rc = tcsetattr(fd, TCSANOW, &options)) < 0) {
        perror("faiuled to set attr.");
        return -1;
    }

    close(fd);
    return 0;
}