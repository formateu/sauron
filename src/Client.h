//
// Created by Przemyslaw Kopanski and Grzegorz Staniszewski on 04.05.17.
//
#ifndef SAURON_CLIENT_H
#define SAURON_CLIENT_H

#include <thread>
#include <unordered_map>
#include <memory>

#include "Connector.h"
#include "MessageBuffer.h"
#include "ClientState.h"

class Client {
public:
    Client(MessageBuffer &msgBuffer,
           const std::string &address,
           size_t port,
           Connector *connector = nullptr,
           ClientState state = ClientState::INIT_PHASE_FIRST);

    void run();

    ClientState getClientState();

    /**
     * Helper function for testing purposes,
     * stops the main loop
     */
    void stop();

private:
    std::unique_ptr<Connector> connector;

    const std::string &mAddress;

    MessageBuffer &msgBuffer;

    ClientState state;

    std::string predecessor;

    std::string successor;

    const std::unordered_map<ClientState, std::function<void(const MessagePair &)>> stateRouter = {
        {
            ClientState::INIT_PHASE_FIRST,
            [this](const auto& messagePair) { handleStateInitPhaseFirst(messagePair); }
        },
        {
            ClientState::INIT_PHASE_SECOND,
            [this](const auto& messagePair) { handleStateInitPhaseSecond(messagePair); }
        },
        {
            ClientState::INIT_PHASE_THIRD,
            [this](const auto &messagePair) { handleStateInitPhaseThird(messagePair); }
        },
        {
            ClientState::CONNECTION_ESTABLISHED,
            [this](const auto &messagePair) { handleStateConnectionEstablished(messagePair); }
        },
        {
            ClientState::FINISHING,
            [this](const auto& messagePair) { handleFinishing(messagePair); }
        }
    };

    /**
     * Awating for predecessor's initialization.
     * @param messagePair
     */
    void handleStateInitPhaseFirst(const MessagePair &messagePair);

    /**
     * Awating for successor's initialization.
     * @param messagePair
     */
    void handleStateInitPhaseSecond(const MessagePair &messagePair);

    /**
     * Awating for ack after successor's initialization.
     * @param messagePair
     */
    void handleStateInitPhaseThird(const MessagePair &messagePair);

    /**
     * Connection has been established properly.
     * @param messagePair
     */
    void handleStateConnectionEstablished(const MessagePair &messagePair);

    /**
     * Handle error message in the first place.
     */
    void handleFinishing(const MessagePair &messagePair);
};


#endif //SAURON_CLIENT_H
