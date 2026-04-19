#include "Receiver.h"
#include "sender.h"

Receiver::Receiver(string filename) : expected_seq_num(0), output_filename(filename) {}

Receiver::~Receiver()
{
    this->running = false;
}

void Receiver::receiveFrame(Frame f)
{
    if (!this->arch.isValid(f))
    {
        cout << "Error (Checksum failed) \n";
        return;
    }

    if (f.seq_no == expected_seq_num)
    {
        cout << "Accepted frame \n";
        deliverData(f.payload);
        sendAck(expected_seq_num);
        expected_seq_num++;
    }
    else
    {
        cout << "Frame is out of order \n";
    }
}

void Receiver::sendAck(int seq_no)
{
    FrameArchitect arch;
    Frame ackFrame = arch.createFrame(ACK, 0, seq_no, "");
    this->Receiverbuffer.push(ackFrame);
}

void Receiver::deliverData(string data)
{
    ofstream outfile(output_filename, std::ios::app);

    if (outfile.is_open())
    {
        outfile << data;
        outfile.close();
    }
    else
    {
        cerr << "Could not write to file \n";
    }
}

queue<Frame> Receiver::get_Receiverbuffer()
{
    return this->Receiverbuffer;
}

bool Receiver::get_running()
{
    return this->running.load();
}

void Receiver::set_running(bool running)
{
    this->running = running;
}

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