//
// Created by Grzegorz Staniszewski on 11.05.17.
//
#include <iostream>

#include "MessageBuffer.h"
#include "Client.h"

int main(int argc, char **argv) {
    MessageBuffer msgBuffer;
    Client client(msgBuffer, "127.0.0.1", 3000);
    client.run();
    return 0;
}
