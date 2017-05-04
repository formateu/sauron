//
// Created by Mateusz Forc on 03.05.17.
//

#include "Connector.h"
#include "Client.h"
#include <unistd.h>

extern char *optarg;

void showUsage(const char* prog) {
    std::cout << "Usage: "
              << prog
              << " [-r configfile] | [-t unit|int] | -h"
              << std::endl;
}

int main(int argc, char** argv)
{
    int opt;
    std::string stringParam;
    opt = getopt(argc, argv, "r:t:h");

    switch (opt) {
        case 'r':
            stringParam = optarg;
            break;
        case 't':
            stringParam = optarg;
            break;
        default:
            showUsage(argv[0]);
            exit(EXIT_FAILURE);
            break;
    }

    MsgBuffer msgBuffer;
    Client client(msgBuffer, "127.0.0.1", 3000);
    client.run();
    return 0;
}
