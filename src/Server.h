//
// Created by Mateusz Forc and Wiktor Franus on 13.05.17.
//
#ifndef SAURON_SERVER_H
#define SAURON_SERVER_H

#include <thread>
#include <unordered_map>
#include <memory>

#include "Connector.h"
#include "MessageBuffer.h"
#include "ClientState.h"

#include "Config.h"

class Server {
public:
    Server(MessageBuffer &msgBuffer,
           const std::string &address,
           size_t port,
           const Config &conf,
           Connector *connector = nullptr);

    void run();

    /**
     * Helper function for testing purposes,
     * stops the main loop
     */

    void stop();

protected:
    std::unique_ptr<Connector> connector;

    const std::string &mAddress;

    MessageBuffer &msgBuffer;

    std::string predecessor;

    std::string successor;

    std::vector<std::string> m_addrList;

    size_t m_pos;

    long m_clientSleepSeconds;

    long m_clientWorkSeconds;

    /**
     * Half-ring thread run function
     * @param msgBuf
     * @param ipRangeFirst
     * @param ipRangeLast
     */
    void halfRingRun(std::unique_ptr<MessageBuffer> &msgBufPtr,
                     size_t ipRangeFirst,
                     size_t ipRangeLast);
};

#endif //SAURON_SERVER_H
