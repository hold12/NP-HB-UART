#include <iostream>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <termios.h>

const char SERIAL_PORT[] = "/dev/ttyPS0";
const int BUFFER_SIZE = 1024;

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
    //options.c_cflag     |= (CLOCAL | CREAD);   // Enable the receiver and set local mode
    //options.c_cflag     &= ~CSTOPB;            // 1 stop bit
    //options.c_cflag     &= ~CRTSCTS;           // Disable hardware flow control
    //options.c_cc[VMIN]   = 1;
    //options.c_cc[VTIME]  = 2;
    options.c_cflag     &=  ~PARENB;            // no-parity
    options.c_cflag     &=  ~CSTOPB;            // 1-stop-bit
    options.c_cflag     &=  ~CSIZE;             // set size to below
    options.c_cflag     |=  CS8;                // 8 bits

    options.c_cc[VMIN]   =  0;                  // read blocks
    options.c_cc[VTIME]  =  3;                  // 0.3 seconds read timeout
    options.c_cflag     |=  CREAD | CLOCAL;     // turn on READ & ignore ctrl lines

    options.c_lflag = ECHO;                     // Enable Echo
    options.c_lflag = ECHOK;                    // Enable Echo KILL
    options.c_lflag = ICANON;                   // Canonical input (erase and kill processing)

    options.c_iflag = IXON;                     // Enable start
    options.c_iflag = ICRNL;                    // Map CR to NL on input

    options.c_oflag = OPOST;                    // Post-process output.
    options.c_oflag = ONLCR;                    // Map NL to CR-NL on output

    // Set new attributes
    if ((rc = tcsetattr(fd, TCSANOW, &options)) < 0) {
        perror("faiuled to set attr.");
        return -1;
    }

    // Write
    //unsigned char cmd[] = {'l', 'o', 's', 'e', 'r', 'v', 'i', 'l', 'l', 'e', '\0'};
    //int n_written = write(fd, cmd, sizeof(cmd) - 1);
    unsigned char cmd[] = "loserville\r";
    int n_written = 0, spot = 0;
    do {
        n_written = write(fd, &cmd[spot], 1);
        spot += n_written;
    } while(cmd[spot-1] != '\r' && n_written > 0);

    // Allocate memory for the read buffer
    char buf{BUFFER_SIZE};
    memset(&buf, '\0', sizeof buf);


    // Read message
    int n = 0, spot_read = 0;
    char buf_read = '\0';

    char response[BUFFER_SIZE];
    memset(response, '\0', sizeof response);

    do {
        n = read(fd, &buf, 1);
        sprintf(&response[spot_read], "%c", buf);
        spot += n;
    } while(buf != '\r' && n > 0);

    if (n < 0)
        std::cout << "Error reading: " << strerror(errno) << std::endl;
    else if (n == 0)
        std::cout << "Read nothing!" << std::endl;
    else
        std::cout << "Response: " << response << std::endl;
    //int n = read(fd, &buf, sizeof buf);

    //if (n < 0)
    //    std::cout << "Error reading: " << strerror(errno) << std::endl;

    // Print the message:
    //std::cout << "Read: " << buf << std::endl;

    close(fd);
    return 0;
}