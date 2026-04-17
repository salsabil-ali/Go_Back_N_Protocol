#include <iostream>
#include <thread>
#include <chrono>
#include <queue>
#include <vector>
#include <algorithm>
#include <mutex>
#include <fstream>
#include "frame.h"

using namespace std;
using namespace std::chrono;





class Sender {
private:
    FrameArchitect arch;
    vector<Frame> slidingWindow;
    queue<string> dataQueue;
    SenderBuffer senderBuffer;

    int windowSize, send_base, next_seq_num, timeoutMs;
    bool running = true;
    Receiver* receiver;


    mutex windowMutex;

public:
    Sender(int N, int timeout, Receiver* r) : receiver(r) {
        windowSize = N; timeoutMs = timeout; send_base = 0; next_seq_num = 0;
    }

    ~Sender() { running = false; }

    void addData(vector<string> msgs) {
        for (auto x : msgs) dataQueue.push(x);
    }

    void start() {
        // Thread timeout
        thread timeoutThread([this]() {
            while (running) {
                this_thread::sleep_for(milliseconds(timeoutMs));
                if (!slidingWindow.empty() && send_base < next_seq_num) {
                    lock_guard<mutex> lock(windowMutex);
                    cout << " THREAD TIMEOUT! Resending from " << send_base << endl;
                    resendAll();
                }
            }
        });
        timeoutThread.detach();
    }

    void tick() {
        lock_guard<mutex> lock(windowMutex);
        sendNewFrames();
    }

    void receiveACK(Frame ackFrame) {
        lock_guard<mutex> lock(windowMutex);
        if (ackFrame.type != ACK) return;

        int ackNum = ackFrame.ack_no;
        cout << " ACK RECEIVED = " << ackNum << endl;

        if (ackNum < send_base) return;

        send_base = ackNum + 1;
        senderBuffer.clearBufferUpTo(ackNum);

        slidingWindow.erase(
            remove_if(slidingWindow.begin(), slidingWindow.end(),
                [this](Frame f) { return f.seq_no < send_base; }),
            slidingWindow.end()
        );
    }

    bool isFinished() {
        lock_guard<mutex> lock(windowMutex);
        return dataQueue.empty() && slidingWindow.empty() && send_base == next_seq_num;
    }

private:
    void sendNewFrames() {
        while (next_seq_num < send_base + windowSize && !dataQueue.empty()) {
            string msg = dataQueue.front(); dataQueue.pop();
            Frame frame = arch1.createFrame(DATA, next_seq_num, 0, msg);

            slidingWindow.push_back(frame);
            senderBuffer.addToBuffer(frame);
            sendFrame(frame);

            next_seq_num++;
        }
    }

    void resendAll() {
        for (auto f : slidingWindow) {
            if (f.seq_no >= send_base) {
                sendFrame(f);
            }
        }
    }

    void sendFrame(Frame frame) {
        cout << " [" << frame.seq_no << "] " << frame.payload << endl;
        receiver->receiveFrame(frame, arch);
    }
};

