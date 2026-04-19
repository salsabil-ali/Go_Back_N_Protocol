#ifndef __ReceiverH
#define __ReceiverH
#include <iostream>
#include <string>
#include <fstream>
#include "frame.h"

using namespace std;

class Receiver
{
private:
    int expected_seq_num;
    string output_filename;

public:
    Receiver(string filename);
    void receiveFrame(Frame f, FrameArchitect &arch);

private:
    void sendAck(int seq_no);
    void deliverData(string data);
};

// int main() {
//     FrameArchitect arch;
//     Receiver myReceiver("received_output.txt");

//     Frame f0 = arch.createFrame(DATA, 0, 0, "Hello from");
//     Frame f1 = arch.createFrame(DATA, 1, 0, " receiver!");
//     Frame corruptFrame = arch.createFrame(DATA, 2, 0, "Error");
//     corruptFrame.checksum = -999;

//     myReceiver.receiveFrame(f0, arch);

//     myReceiver.receiveFrame(corruptFrame, arch);

//     myReceiver.receiveFrame(f1, arch);

//     cout << "\n Check 'received_output.txt' to see the correctly-received data.\n";

//     return 0;
// }

#endif