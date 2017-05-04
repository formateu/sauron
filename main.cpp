//
// Created by Mateusz Forc on 03.05.17.
//

#include <unistd.h>
#include <iostream>
#include "Connector.h"

extern char *optarg;

using namespace std;


void showUsage(const char* prog) {
    cout << "Usage: " << prog
         << " [-r configfile] | [-t unit|int] | -h" << endl;
}


int main(int argc, char** argv)
{
    int opt;
    string stringParam;
    opt = getopt(argc, argv, "r:t:h");
    switch (opt) {
      case 'r': {
        stringParam = optarg;
        break;
      }
      case 't': {
        stringParam = optarg;
        break;
      }
      default: {
        showUsage(argv[0]);
        exit(EXIT_FAILURE);
        break;
      }
    }

    Connector connector;
    return 0;
}

