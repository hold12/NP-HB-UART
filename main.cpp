#include <iostream>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <termios.h>
#include <iostream>
#include <fstream>

const char SERIAL_PORT[] = "/dev/ttyPS0";
const int BUFFER_SIZE = 8;

int openPort(void) {
    int fd = open("/dev/ttyPS0", O_RDWR | O_NOCTTY);

    if (fd == -1)
        perror("[ERROR] openPort: Unable to serial port.");
    else {
        fcntl(fd, F_SETFL, 0);


        std::cout << "[INFO] Success connecting to " << "serial port." << std::endl;
    }
    return(fd);
}

int main() {
    int             fd = 0;     // File descriptor
    struct termios  options;    // Terminal options
    int             rc;         // Return value

    fd = openPort();

    if ((rc = tcgetattr(fd, &options)) < 0) {
        perror("[ERROR] failed to get attr.");
        return -1;
    }

    cfsetispeed(&options, B115200);
    cfsetospeed(&options, B115200);

    cfmakeraw(&options);
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

    if ((rc = tcsetattr(fd, TCSANOW, &options)) < 0) {
        perror("[ERROR] faiuled to set attr.");
        return -1;
    }

    // Write
    unsigned char cmd[] = "loserville\r";
    int n_written = 0, spot = 0;
    do {
        n_written = write(fd, &cmd[spot], 1);
        spot += n_written;
    } while(cmd[spot-1] != '\r' && n_written > 0);

    // Read incoming message
    int n = 0, spot_read = 0;
    char buf = '\0';

    // Allocating memory for the read buffer
    std::cout << "[INFO] Allocation memory." << std::endl;
    char response[BUFFER_SIZE];
    memset(response, '\0', sizeof response);
    std::cout << "[INFO] Memory allocated." << std::endl;

    std::string message = "";
    std::cout << "[INFO] Reading incoming message." << std::endl;
    
    for(int read_count = 0; read_count < 16500; read_count) {
        n = read(fd, &buf, 1);
        message += std::string(&buf);

        spot += n;
        read_count++;
    } 


    std::cout << "[INFO] " << std::endl;
    if (n < 0)
        std::cout << "[ERROR] Error reading: " << strerror(errno) << std::endl;
    else if (n == 0)
        std::cout << "[ERROR] Read nothing!" << std::endl;
    else {
        std::cout << "Message received: " << std::endl;
    }

    /*std::cout << "[INFO] Send off to sensor 2" << std::endl;
    unsigned char cmd2[] = "off 2\r";
    int n_written2 = 0, spot2 = 0;
    do {
        n_written2 = write(fd, &cmd2[spot2], 1);
        spot2 += n_written2;
    } while(cmd2[spot2-1] != '\r' && n_written2 > 0);

    std::cout << "[INFO] Message sent" << std::endl;*/

    close(fd);

    FILE *fp = fopen("/var/ftp/pub/data.csv", "w");
    fprintf(fp, message.c_str());
    fclose(fp);
    return 0;
}