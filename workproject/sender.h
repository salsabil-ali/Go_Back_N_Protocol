#ifndef __senderh
#define __senderh
#include <iostream>
#include <thread>
#include <chrono>
#include <queue>
#include <vector>
#include <algorithm>
#include <mutex>
#include <fstream>
#include "frame.h"
#include "Receiver.h"

using namespace std;
using namespace std::chrono;

class Sender
{
private:
    FrameArchitect arch;
    vector<Frame> slidingWindow;
    queue<string> dataQueue;
    SenderBuffer senderBuffer;

    int windowSize, send_base, next_seq_num, timeoutMs;
    bool running = true;
    Receiver *receiver;

    mutex windowMutex;

public:
    Sender(int N, int timeout, Receiver *r);
    ~Sender();
    void addData(vector<string> msgs);
    void start();
    void tick();
    void receiveACK(Frame ackFrame);
    bool isFinished();

private:
    void sendNewFrames();
    void resendAll();
    void sendFrame(Frame frame);
};
#endif
