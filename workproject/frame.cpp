#include "frame.h"

int Frame::size()
{
    return this->payload.size() + 4 * 3;
}

void Frame::display(ostream &out) const
{
    string typeStr = (type == DATA) ? "DATA" : "ACK";
    out << "[" << typeStr << " | Seq: " << seq_no
        << " | Ack: " << ack_no << " | Checksum: " << checksum << "]";
}

ostream &operator<<(std::ostream &out, const Frame &aFrame)
{
    aFrame.display(cout);
    return cout;
}

int FrameArchitect::calculateChecksum(FrameType type, int seq, int ack, const std::string &data)
{
    int sum = (int)type + seq + ack;
    for (char c : data)
    {
        sum += (int)c;
    }

    return sum;
}

bool FrameArchitect::isValid(const Frame &f)
{
    return f.checksum == calculateChecksum(f.type, f.seq_no, f.ack_no, f.payload);
}

Frame FrameArchitect::createFrame(FrameType type, int seq, int ack, std::string data)
{
    Frame newFrame;
    newFrame.type = type;
    newFrame.seq_no = seq;
    newFrame.ack_no = ack;
    newFrame.payload = data;
    newFrame.checksum = calculateChecksum(type, seq, ack, data); // call checksum to see if the integrs here as the stored or not
    return newFrame;
}

//
void SenderBuffer::addToBuffer(const Frame &f)
{
    windowBuffer.push_back(f);
}

Frame SenderBuffer::getFrameFromBuffer(int seq)
{
    for (const auto &f : windowBuffer)
    {
        if (f.seq_no == seq)
            return f;
    }
    return {}; // Return empty
}

void SenderBuffer::clearBufferUpTo(int ack_no)
{
    // Removes frames that have been successfully acknowledged
    windowBuffer.erase(
        remove_if(windowBuffer.begin(), windowBuffer.end(), //:) AI ,don't ask me
                  [ack_no](const Frame &f)
                  { return f.seq_no <= ack_no; }),
        windowBuffer.end());
}