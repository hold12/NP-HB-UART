#include <iostream>
#include <string.h>
#include "Serial/SerialStream.h"
#include "Serial/SerialStreamBuf.h"

using namespace LibSerial;

const std::string STREAM = "/dev/ttyPS0";
SerialStream serialStream;
const int BUFFER_SIZE = 256;

int main() {

    std::cout << "Serial Stream: " << STREAM << std::endl;
    std::cout << "Baud Rate: " << "115200 " << std::endl;

    serialStream.Open(STREAM);
    serialStream.SetBaudRate(SerialStreamBuf::BAUD_115200);

    serialStream.SetCharSize(SerialStreamBuf::CHAR_SIZE_8);
    serialStream.SetNumOfStopBits(1);
    serialStream.SetParity(SerialStreamBuf::PARITY_ODD);
    serialStream.SetFlowControl(SerialStreamBuf::FLOW_CONTROL_HARD);

    // Send message:
    serialStream << "loserville" << std::endl;

    // Receive
    char input_buffer[BUFFER_SIZE];
    serialStream.read(input_buffer, BUFFER_SIZE);

    std::cout << "Received: " << std::endl << input_buffer << std::endl;
    std::cout << std::endl << "END" << std::endl;

    serialStream.Close();
    return 0;
}