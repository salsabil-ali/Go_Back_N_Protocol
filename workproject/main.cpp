#include <iostream>
#include <vector>
#include <string>
#include <ctime>
#include "sender.h"
#include "Receiver.h"
#include "Network.h"
#include <thread>

// int main()
// {
//     Frame* tempframe;
//     FrameArchitect arch;
//     Receiver receiver("output.txt");
//     Sender sender(3, 3000, &receiver);

//     sender.start();
//     sender.addData({"P1", "P2", "P3", "P4", "P5"});

//     cout << "Window=3, Timeout=3s\n\n";

//     int tick = 0;
//     while (!sender.isFinished() && tick++ < 60)
//     { // ← 60 tick بدل 40
//         sender.tick();
        
//         // ACKs تدريجياً
//         if (tick == 8)
//         {
//             tempframe = &(receiver.get_Receiverbuffer().at(1));
//             receiver.get_Receiverbuffer().pop_back();
//             sender.receiveACK(tempframe);
//             cout << "\n--- ACK[1] ---\n";
//             Frame ack1 = arch.createFrame(ACK, 0, 1, "");
//         }
//         if (tick == 20)
//         {
//             cout << "\n--- ACK[3] ---\n";
//             Frame ack2 = arch.createFrame(ACK, 0, 3, "");
//         }
//         if (tick == 35)
//         {
//             cout << "\n--- ACK[4] ---\n";
//             Frame ack3 = arch.createFrame(ACK, 0, 4, "");
//         }

//         cout << "Tick " << tick << " | base=" << sender.get_send_base()
//              << " next=" << sender.get_next_seq_num() << endl;
//         this_thread::sleep_for(milliseconds(500));
//     }
//     return 0;
// }

int main()
{
    mutex mainmutex;
    Receiver receiver("output.txt");
    Sender sender(3, 3000, &receiver);

    sender.start();

    thread senderThread([&sender, &receiver] {
        while (sender.get_running())
        {
            sender.tick();
            if (!receiver.get_Receiverbuffer().empty())
            {
                sender.receiveACK(&(receiver.get_Receiverbuffer().front()));
                receiver.get_Receiverbuffer().pop();
            }
        }
    });

    thread receiverThread([&receiver, &sender] {
        while (receiver.get_running())
        {
            receiver.receiveFrame(sender.senderBuffer.getFrameFromBuffer());
        }
    });

    vector<string> data;

    data = {"We", "Love", "Doctor", "Hossam"};

    {
        lock_guard<mutex> lock(mainmutex);
        sender.addData(data);
    }
    char reply = 'a';

    cout << "to quit the program enter x";

    while (reply != 'x')
    {
        cin>>reply;
    }

    {
        lock_guard<mutex> lock(mainmutex);

        sender.set_running(false);
        receiver.set_running(false);
    }
}

// int main()
// {
//     Receiver receiver("Received_output");
//     Sender sender(5, 3000, &receiver);

//     vector<string> data = {"Mahmoud", "haydrab", "ely", "3amalo", "el", "sender", "w", "el", "receiver"};

//     sender.start();
//     sender.addData(data);

// }