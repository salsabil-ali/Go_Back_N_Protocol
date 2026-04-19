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
#include <atomic>

using namespace std;
using namespace std::chrono;

class Sender
{
private:
    FrameArchitect arch;
    vector<Frame> slidingWindow;
    queue<string> dataQueue;
    std::atomic<bool> running{true};
    
    int windowSize, send_base, next_seq_num, timeoutMs;
    Receiver *receiver;
    
    mutex windowMutex;
    
    public:
    SenderBuffer senderBuffer;
    Sender(int N, int timeout, Receiver *r);
    ~Sender();
    void addData(vector<string> msgs);
    void start();
    void tick();
    void receiveACK(Frame *ackFrame);
    bool isFinished();
    FrameArchitect get_arch() const;
    int get_send_base() const;
    int get_next_seq_num() const;
    bool get_running();
    void set_running(bool running);
    void run();

private:
    void sendNewFrames();
    void resendAll();
    void sendFrame(Frame *frame);
};
#endif
