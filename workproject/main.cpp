#include <iostream>
#include <vector>
#include <string>
#include <ctime>
#include "sender.h"
#include "Receiver.h"
#include "Network.h"

using namespace std;

// A wrapper to simulate the unreliable network path
void transmit(Frame f, Receiver& rec, FrameArchitect& arch, int timeout) {
    Frame copy = f; // Work on a copy to simulate wire changes
    if (chaosFrame(copy, timeout)) {
        rec.receiveFrame(copy, arch);
    }
}

int main() {
    srand(static_cast<unsigned>(time(0)));

    // Configuration
    int windowSize = 3;
    int timeoutMs = 200;
    string outputFile = "received_output.txt";
    
    // Clear previous output file
    remove(outputFile.c_str());

    Receiver receiver(outputFile);
    Sender sender(windowSize, timeoutMs, &receiver);

    // --- Scenario 1: Successful Sequential Transmission ---
    cout << "--- SCENARIO 1: Basic Transmission ---" << endl;
    sender.addData({"Hello ", "this ", "is ", "a ", "test."});
    
    // Process the queue
    while (!sender.isFinished()) {
        sender.tick();
        this_thread::sleep_for(milliseconds(100));
    }
    cout << "Scenario 1 Complete.\n" << endl;

    // --- Scenario 2: Heavy Network Chaos (Drops & Corruption) ---
    // Note: To truly see the resend logic, we rely on the Sender's background thread
    cout << "--- SCENARIO 2: Unreliable Network (Resends) ---" << endl;
    sender.start(); // Starts the timeout monitor thread
    sender.addData({"Data_A ", "Data_B ", "Data_C ", "Data_D "});

    int attempts = 0;
    while (!sender.isFinished() && attempts < 20) {
        sender.tick(); 
        this_thread::sleep_for(milliseconds(300));
        attempts++;
    }

    cout << "Scenario 2 Complete. Check " << outputFile << " for results." << endl;

    return 0;
}