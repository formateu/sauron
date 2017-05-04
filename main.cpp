//
// Created by Mateusz Forc on 03.05.17.
//

#include "Connector.h"
#include "Client.h"

int main(int argc, char** argv)
{
    MsgBuffer msgBuffer;
    Client client(msgBuffer, "127.0.0.1", 3000);
    client.run();
    return 0;
}