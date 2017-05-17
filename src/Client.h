//
// Created by Przemyslaw Kopanski and Grzegorz Staniszewski on 04.05.17.
//
#ifndef SAURON_CLIENT_H
#define SAURON_CLIENT_H

#include <thread>
#include <chrono>
#include <unordered_map>
#include <memory>
#include <random>

#include "Connector.h"
#include "MessageBuffer.h"
#include "ClientState.h"
#include "Timer.h"

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

protected:
    std::unique_ptr<Connector> connector;

    const std::string &mAddress;

    MessageBuffer &msgBuffer;

    ClientState state;

    std::string predecessor;

    std::string successor;

    std::unique_ptr<Timer> measurementTimer;

    /**
     * Am I the last node in halfring
     */
    bool amILast;

    /**
     * Main loop running bool
     */
    bool isActive;

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
            ClientState::CONNECTION_ESTABLISHED,
            [this](const auto &messagePair) { handleStateConnectionEstablished(messagePair); }
        },
        {
            ClientState::MEASURE_TIME,
            [this](const auto &messagePair) { handleMeasureTime(messagePair); }
        },
        {
            ClientState::FINISH,
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
     * Connection has been established properly.
     * @param messagePair
     */
    void handleStateConnectionEstablished(const MessagePair &messagePair);

    /**
     * Handle messages while running
     */
    void handleMeasureTime(const MessagePair &messagePair);

    /**
     * Handle error message in the first place.
     */
    void handleFinishing(const MessagePair &messagePair);

    /**
     * Starts measurement thread
     */
    void startMeasurement(int activePeriod, int inactivePeriod);

    /**
     * Stops measurement thread
     */
    void stopMeasurement();

    /**
     * Sends measurement result
     */
    void sendMeasurementInfo(int measureval);

    /**
     * Sends ACK
     */
    void sendAck(std::string address);

    /**
     * Send any message
     */
    void sendMessage(std::string address, const Message& msg);
};


#endif //SAURON_CLIENT_H
