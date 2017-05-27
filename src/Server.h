//
// Created by Mateusz Forc and Wiktor Franus on 13.05.17.
//
#ifndef SAURON_SERVER_H
#define SAURON_SERVER_H

#include <csignal>
#include <thread>
#include <unordered_map>
#include <memory>
#include <netdb.h>

#include "HalfRing.h"
#include "Connector.h"
#include "MessageBuffer.h"
#include "ServerState.h"
#include "Utils.h"

#include "Config.h"

class Server {
public:
    using SharedMsgBufPtr = std::shared_ptr<MessageBuffer>;

    Server(SharedMsgBufPtr mainBuffer,
           size_t port,
           ConfigBase *conf,
           Connector *connector = nullptr,
           ServerState state = ServerState::CREATED);

    ~Server();

    void run();

    /**
     * Helper function for testing purposes,
     * stops the main loop
     */

    void stop();

protected:
    std::unique_ptr<Connector> m_connector;

    SharedMsgBufPtr m_mainBuffer;

    SharedMsgBufPtr m_halfRing1Buffer;

    SharedMsgBufPtr m_halfRing2Buffer;

    std::vector<std::string> m_addrHalfRing1;

    std::vector<std::string> m_addrHalfRing2;

    size_t m_pos;

    ServerState m_state;

    unsigned m_clientSleepSeconds;

    unsigned m_clientWorkSeconds;

    const std::unordered_map<ServerState , std::function<void(const MessagePair &)>> m_stateRouter = {
        {
            ServerState::WAITING_FOR_FIRST_HALF,
            [this](const auto& messagePair) { handleStateWaitingForFirstHalf(messagePair); }
        },
        {
            ServerState::WAITING_FOR_SECOND_HALF,
            [this](const auto &messagePair) { handleStateWaitingForSecondHalf(messagePair); }
        },
        {
            ServerState::WAITING_FOR_RESULTS,
            [this](const auto &messagePair) { handleStateWaitingForResults(messagePair); }
        }
    };

    void mainLoop();

    void runOneHalfRing();

    void runTwoHalfRings();

    /**
     *
     * @param messagePair
     */
    void handleStateWaitingForFirstHalf(const MessagePair &messagePair);

    /**
     *
     * @param messagePair
     */
    void handleStateWaitingForSecondHalf(const MessagePair &messagePair);

    /**
     *
     * @param messagePair
     */
    void handleStateWaitingForResults(const MessagePair &messagePair);

    /**
     *
     */
    void handleFinishing();

    /**
     * Push Terminate Message to given buffer
     */
    void stop(std::shared_ptr<MessageBufferBase>);

    /**
     * Normalizes IPv6 address
     */
    std::string normalizeAddress(std::string address);
};

#endif //SAURON_SERVER_H
