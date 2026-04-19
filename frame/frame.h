#pragma once
#include <iostream>
#include <string>
#include <vector>


enum FrameType { DATA, ACK }; 

struct Frame {
    FrameType type;
    int seq_no;
    int ack_no;
    std::string payload;
    int checksum;

   
    void display(ostream& cout) const; //done
    // Overloaded output operator
    friend ostream& operator<<(std::ostream& out, const Frame& aFrame);
};

class FrameArchitect {
public:
    //  Checksum Implementation
    // This calculates a simple sum of all integer values and characters in the payload
    //di 3a4an check  if the message changed or it as same
    int calculateChecksum(FrameType type, int seq, int ack, const std::string& data);


    // Function to verify if a received frame is corrupted
    //here if the receiver take time to send the ACK or didn't send it 
    bool isValid(const Frame& f);


    //  Ay frame sender or el receiver e3malo hnst5dmha 3a4an n check is the message changed or not ?
    Frame createFrame(FrameType type, int seq, int ack, std::string data);
};


//  Buffer 
// This stores frames  in case of a Timeout (copy of the frame and then clear it when transmit successfully :))
class SenderBuffer {
private:
    std::vector<Frame> windowBuffer;

public:
    void addToBuffer(const Frame& f);
    
    Frame getFrameFromBuffer(int seq);

    void clearBufferUpTo(int ack_no);
};