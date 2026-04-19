#include <iostream>
#include <cstdlib> // for rand()
#include <thread>
#include <chrono>
#include "frame.h"

using namespace std;

// Prints E or D event
void logEvent(char type, string msg);

// Flip a random bit in the frame
void corruptFrame(Frame &frame);

// Main chaos function
bool chaosFrame(Frame &frame, int timeoutMs);