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
#include "ServerState.h"

#include "Config.h"

class Server {
public:
    Server(MessageBuffer &msgBuffer,
           const std::string &address,
           size_t port,
           const Config &conf,
           Connector *connector = nullptr,
           ServerState state = ServerState::CREATED);

    void run();

    /**
     * Helper function for testing purposes,
     * stops the main loop
     */

    void stop();

protected:
    class HalfRing {
    public:
        using MsgBufPtr = std::unique_ptr<MessageBuffer>;
        using ConnectorPtr = std::unique_ptr<Connector>;
        using AddressVector = std::vector<std::string>;

        HalfRing(ConnectorPtr &cntrPtr, MessageBuffer &mainBuffer,
                 MsgBufPtr &msgBufPtr, AddressVector& addressVector)
            : m_connector(cntrPtr)
            , m_mainBuf(mainBuffer)
            , m_msgBuf(msgBufPtr)
            , m_addrVec(addressVector)
            , m_state(HalfRingState::WAITING_FOR_ACK)
        {}

        enum HalfRingState {
            WAITING_FOR_ACK,
            WAITING_FOR_INIT_OK,
            INITIALIZATION_FINISHED,
            FINISHED,
            IDLE
        };

        void operator()() {
            Message msg(MessageType::Init, m_addrVec[0].c_str());
            m_connector->send(m_addrVec[0], msg);
            m_currAddr = 0;

            while (m_state != HalfRingState::FINISHED) {
                MessagePair messagePair = m_msgBuf.get()->pop();

                if (messagePair.second.m_type == MessageType::Finish) {
                    handleFinished(messagePair);
                } else if (messagePair.second.m_type == MessageType::Terminate){
                    break;
                }

                auto handler = stateRouter.find(m_state);

                if (handler != stateRouter.end()) {
                    handler->second(messagePair);
                } else {
                    throw std::runtime_error("[ERROR]: Unkown state. ");
                }
            }
        }
    protected:
        HalfRingState m_state;
        ConnectorPtr &m_connector;
        MessageBuffer &m_mainBuf;
        MsgBufPtr &m_msgBuf;
        AddressVector &m_addrVec;
        size_t m_currAddr;

        const std::unordered_map<HalfRingState , std::function<void(const MessagePair &)>> stateRouter = {
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

        void handleStateWaitingForAck(const MessagePair &messagePair) {
            if (messagePair.second.m_type != MessageType::Ack) {
                //ERROR
                return;
            }

            if (m_currAddr == m_addrVec.size() - 1) {
                m_state = HalfRingState::IDLE;
            } else {
                m_state = HalfRingState::WAITING_FOR_INIT_OK;
            }
        };

        void handleStateWaitingForInitOK(const MessagePair &messagePair) {
            if (messagePair.second.m_type != MessageType::InitOk) {
                //ERROR
                return;
            }

            if (m_currAddr == m_addrVec.size() - 3) {
                m_connector->send(m_addrVec[m_addrVec.size() - 2],
                                  Message(MessageType::Last,
                                          m_addrVec[m_addrVec.size() - 1].c_str()));
                m_state = HalfRingState::WAITING_FOR_ACK;
            } else if (m_currAddr == m_addrVec.size() - 2){
                auto msgSenderPair = std::make_pair("127.0.0.1",
                                                    Message(MessageType::OneHalfInitFinish));
                m_mainBuf.push(msgSenderPair);
                m_state = HalfRingState::INITIALIZATION_FINISHED;
            } else {
                m_connector->send(messagePair.first, Message(MessageType::Ack));
                m_state = HalfRingState::WAITING_FOR_INIT_OK;
            }

            ++m_currAddr;
        };

        void handleStateInitializationFinished(const MessagePair &messagePair) {
            if (messagePair.second.m_type != MessageType::Run) {
                //ERROR
                return;
            }

            m_connector->send(m_addrVec[0], Message(MessageType::Run));
            m_state = HalfRingState::WAITING_FOR_ACK;

        }

        void handleFinished(const MessagePair &messagePair) {
            m_connector->send(m_addrVec[0], Message(MessageType::Finish));
            m_state = HalfRingState::FINISHED;
        }
    };

    std::unique_ptr<Connector> connector;

    const std::string &mAddress;

    MessageBuffer &msgBuffer;

    std::vector<std::string> m_addrHalfRing1;

    std::vector<std::string> m_addrHalfRing2;

    size_t m_pos;

    ServerState m_state;

    long m_clientSleepSeconds;

    long m_clientWorkSeconds;

    const std::unordered_map<ServerState , std::function<void(const MessagePair &)>> stateRouter = {
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
        },
        {
            ServerState::FINISHING,
            [this](const auto& messagePair) { handleFinishing(messagePair); }
        }
    };

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
    void handleFinishing(const MessagePair &messagePair);

};

#endif //SAURON_SERVER_H
