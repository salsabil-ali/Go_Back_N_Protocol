#include "sender.h"

Sender::Sender(int N, int timeout, Receiver *r) : receiver(r)
{
    windowSize = N;
    timeoutMs = timeout;
    send_base = 0;
    next_seq_num = 0;
}

Sender::~Sender() { running = false; }

void Sender::addData(vector<string> msgs)
{
    for (auto x : msgs)
        dataQueue.push(x);
}

void Sender::run()
{
    while (true)
    {
        tick();
    }
}

void Sender::start()
{
    // Thread timeout
    thread timeoutThread([this]()
                         {
        while (running) {
            this_thread::sleep_for(milliseconds(timeoutMs));
            if (!slidingWindow.empty() && send_base < next_seq_num)
            {
                lock_guard<mutex> lock(windowMutex);
                cout << " THREAD TIMEOUT! Resending from " << send_base << endl;
                resendAll();
            }
        } });
    timeoutThread.detach();
}

void Sender::tick()
{
    lock_guard<mutex> lock(windowMutex);
    sendNewFrames();
}

void Sender::receiveACK(Frame *ackFrame)
{
    lock_guard<mutex> lock(windowMutex);
    if (ackFrame->type != ACK)
        return;

    int ackNum = ackFrame->ack_no;
    cout << " ACK RECEIVED = " << ackNum << endl;

    if (ackNum < send_base)
        return;

    send_base = ackNum + 1;
    senderBuffer.clearBufferUpTo(ackNum);

    slidingWindow.erase(
        remove_if(slidingWindow.begin(), slidingWindow.end(),
                  [this](Frame f)
                  { return f.seq_no < send_base; }),
        slidingWindow.end());
}

bool Sender::get_running()
{
    return this->running;
}

void Sender::set_running(bool running)
{
    this->running = running;
}

// bool Sender::isFinished()
// {
//     lock_guard<mutex> lock(windowMutex);
//     return dataQueue.empty() && slidingWindow.empty() && send_base == next_seq_num;
// }

void Sender::sendNewFrames()
{
    while (next_seq_num < send_base + windowSize && !dataQueue.empty())
    {
        string msg = dataQueue.front();
        dataQueue.pop();
        Frame frame = arch.createFrame(DATA, next_seq_num, 0, msg);

        slidingWindow.push_back(frame);
        senderBuffer.addToBuffer(frame);
        sendFrame(&frame);

        next_seq_num++;
    }
}

void Sender::resendAll()
{
    for (auto f : slidingWindow)
    {
        if (f.seq_no >= send_base)
        {
            sendFrame(&f);
        }
    }
}

void Sender::sendFrame(Frame *frame)
{
    cout << " [" << frame->seq_no << "] " << frame->payload << endl;
    // receiver->receiveFrame(&(*frame), arch);
    senderBuffer.addToBuffer(*frame);
}

FrameArchitect Sender::get_arch() const
{
    return this->arch;
}

int Sender::get_send_base() const
{
    return this->send_base;
}

int Sender::get_next_seq_num() const
{
    return this->next_seq_num;
}

// int main() {
//     Receiver receiver("output.txt");
//     Sender sender(3, 3000, &receiver);

//     sender.start();
//     sender.addData({"P1", "P2", "P3", "P4", "P5"});

//     cout << "📊 Window=3, Timeout=3s\n\n";

//     int tick = 0;
//     while (!sender.isFinished() && tick++ < 60) {  // ← 60 tick بدل 40
//         sender.tick();

//         // ACKs تدريجياً
//         if (tick == 8) {
//             cout << "\n--- ACK[1] ---\n";
//             Frame ack1 = sender.get_arch().createFrame(ACK, 0, 1, "");
//             sender.receiveACK(ack1);
//         }
//         if (tick == 20) {
//             cout << "\n--- ACK[3] ---\n";
//             Frame ack2 = sender.get_arch().createFrame(ACK, 0, 3, "");
//             sender.receiveACK(ack2);
//         }
//         if (tick == 35) {
//             cout << "\n--- ACK[4] ---\n";
//             Frame ack3 = sender.get_arch().createFrame(ACK, 0, 4, "");
//             sender.receiveACK(ack3);
//         }

//         cout << "Tick " << tick << " | base=" << sender.send_base
//              << " next=" << sender.next_seq_num << endl;
//         this_thread::sleep_for(milliseconds(500));
//     }
//     return 0;
// }

// int main() {
//     Receiver receiver("output.txt");
//     SimpleGBNSender sender(3, 3000, &receiver);

//     sender.start();
//     sender.addData({"P1", "P2", "P3", "P4", "P5"});

//     int tick = 0;
//     while (!sender.isFinished() && tick++ < 60) {
//         sender.tick();

//         // Status كل 5 ticks
//         if (tick % 5 == 0) {
//             cout << "📊 Tick=" << tick
//                  << " base=" << sender.send_base
//                  << " next=" << sender.next_seq_num
//                  << " window=" << sender.slidingWindow.size() << endl;
//         }

//         // ACKs
//         if (tick == 10) sender.receiveACK(arch.createFrame(ACK, 0, 1, ""));
//         if (tick == 25) sender.receiveACK(arch.createFrame(ACK, 0, 3, ""));
//         if (tick == 40) sender.receiveACK(arch.createFrame(ACK, 0, 4, ""));

//         this_thread::sleep_for(milliseconds(500));
//     }

//     cout << "\n✅ All packets sent! output.txt:\n";
//     ifstream file("output.txt");
//     string content((istreambuf_iterator<char>(file)), istreambuf_iterator<char>());
//     cout << content << endl;
//     return 0;
// }