//
// Created by Mateusz Forc and Wiktor Franus on 18.05.17.
//
#ifndef SAURON_HALFRING_H
#define SAURON_HALFRING_H

#include <unordered_map>
#include <memory>

#include "Message.h"
#include "Connector.h"
#include "MessageBuffer.h"

class HalfRing {
    public:
        using ConnectorPtr = std::unique_ptr<Connector>;
        using AddressVector = std::vector<std::string>;

        enum HalfRingState {
            WAITING_FOR_ACK,
            WAITING_FOR_INIT_OK,
            INITIALIZATION_FINISHED,
            FINISHED,
            IDLE
        };

        HalfRing(ConnectorPtr &cntrPtr, MessageBuffer &mainBuffer,
                MessageBuffer &msgBuf, AddressVector& addressVector,
                int clientWorkSeconds, int clientSleepSeconds);

        ~HalfRing();

        void operator()();

    protected:
        HalfRingState m_state;
        ConnectorPtr &m_connector;
        MessageBuffer &m_mainBuf;
        MessageBuffer &m_halfRingBuf;
        AddressVector &m_addrVec;
        size_t m_currAddr;
        unsigned m_clientWorkSeconds;
        unsigned m_clientSleepSeconds;

        const std::unordered_map<HalfRingState , std::function<void(const MessagePair &)>> m_stateRouter = {
            {
                HalfRingState::WAITING_FOR_ACK,
                [this](const auto& messagePair) { handleStateWaitingForAck(messagePair); }
            },
            {
                HalfRingState::WAITING_FOR_INIT_OK,
                [this](const auto &messagePair) { handleStateWaitingForInitOK(messagePair); }
            },
            {
                HalfRingState::INITIALIZATION_FINISHED,
                [this](const auto &messagePair) { handleStateInitializationFinished(messagePair); }
            },
            {
                HalfRingState::FINISHED,
                [this](const auto& messagePair) { handleFinished(messagePair); }
            }
        };

        void handleStateWaitingForAck(const MessagePair &messagePair);

        void handleStateWaitingForInitOK(const MessagePair &messagePair);

        void handleStateInitializationFinished(const MessagePair &messagePair);

        void handleFinished(const MessagePair &messagePair);

        void convertStringIPv6ToBSD(const std::string &address, std::string &output);

        void convertStringIPv4ToBSD(std::string address, std::string &output);

        void convertAddress(const std::string &address, std::string &output);
};

#endif //SAURON_HALFRING_H
