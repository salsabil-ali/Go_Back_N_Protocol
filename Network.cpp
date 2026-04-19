#include <iostream>
#include <cstdlib> // for rand()

#include <thread>
#include <chrono>
#include "frame.h"

using namespace std;

// Prints E or D event
void logEvent(char type, string msg)
{
    cout << "  [" << type << "] " << msg << "\n";
}

// Flip a random bit in the frame
void corruptFrame(Frame &frame)
{
    int byteIdx = rand() % frame.size(); // Pick a random byte
    int bitIdx = rand() % 8; // Pick a random bit in that byte
    frame[byteIdx] ^= (1 << bitIdx); 
    // Shift the choosen bit to the left since the index starts at 0 
    // Then XOR it with the byte to flip that bit
}

// Main chaos function
bool chaosFrame(Frame &frame, int timeoutMs)
{

    // 1. Random drop (40% chance)
    if (rand() % 10 < 4)
    {
        logEvent('D', "Frame dropped");
        return false;
    }

    // 2. Random corruption (30% chance)
    if (rand() % 10 < 3)
    {
        corruptFrame(frame);
        logEvent('E', "Bit flipped — checksum will fail");
    }

    // 3. Random latency, range determined from the sender's timeout (0 to 2x timeout)
    int delay = rand() % (timeoutMs * 2);
    cout << "  Latency: " << delay << "ms\n";
    this_thread::sleep_for(chrono::milliseconds(delay));

    return true;
}

int main() {
    srand(time(0)); // seed random

    Frame frame = {0x01, 0x02, 0x03, 0xAA};
    int timeoutMs = 100; // Example timeout value
    for (int i = 1; i <= 5; i++) {
        cout << "\n--- Frame " << i << " ---\n";

        Frame f = frame; // fresh copy each time
        if (chaosFrame(f, timeoutMs))
            cout << "  Delivered. First byte: " << hex << (int)f[0] << "\n";
        else
            cout << "  Not delivered\n";
    }

    return 0;
}
