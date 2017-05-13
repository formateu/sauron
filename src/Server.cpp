//
// Created by Mateusz Forc and Wiktor Franus on 13.05.17.
//
#include "Server.h"

Server::Server(MessageBuffer &msgBuffer,
               const std::string &address,
               size_t port,
               const Config &conf,
               Connector *inputConnector,
               ClientState state)
    : mAddress(address)
    , msgBuffer(msgBuffer)
    , m_addrList(conf.m_ipVec)
    , m_pos(conf.m_ipVec.size() / 2)
    , m_clientWorkSeconds(conf.clientWorkSeconds)
    , m_clientWorkSeconds(conf.clientSleepSeconds)
    , state(state)
{
    if (inputConnector == nullptr) {
        connector.reset(new InternetConnector(msgBuffer, port));
    } else {
        connector.reset(inputConnector);
    }
}

void Server::run() {
    if (m_ipVec.size()) {
        std::thread listenThread(&Connector::listen, connector.get());
    } else {
        return;
    }

    std::unique_ptr<MessageBuffer> msgBuf1Ptr(new MessageBuffer());
    std::unique_ptr<MessageBuffer> msgBuf2Ptr(nullptr);

    if (m_ipVec.size() > 1) {
        msgBuf2Ptr.reset(new MessageBuffer());
        std::thread half1Thread(&Server::halfRingRun, this, msgBuf1Ptr, 0, m_pos);
        std::thread half2Thread(&Server::halfRingRun, this, msgBuf2Ptr, m_pos + 1, m_addrList.size() - 1);
    } else {
        std::thread half1Thread(&Server::halfRingRun, this, msgBuf1Ptr, 0, 0);
    }


    while (true) {
    }

    listenThread.join();
    half1Thread.join();
    half2Thread.join();
}

void Server::halfRingRun(size_t ipRangeFirst, size_t ipRangeLast) {

    while (state != ClientState::FINISHED) {
        MessagePair messagePair = msgBuffer.pop();

        if (messagePair.second.m_type == MessageType::Finish) {
            handleFinishing(messagePair);
        } else if (messagePair.second.m_type == MessageType::Terminate){
            break;
        }

        auto handler = stateRouter.find(state);

        if (handler != stateRouter.end()) {
            handler->second(messagePair);
        } else {
            throw std::runtime_error("[ERROR]: Unkown state. ");
        }
    }
}

ClientState Server::getClientState() {
    return state;
}

void Server::handleStateInitPhaseFirst(const MessagePair &messagePair) {
    if (messagePair.second.m_type != MessageType::Init) {
        //ERROR
        return;
    }

    predecessor = messagePair.first;
    state = ClientState::INIT_PHASE_SECOND;
    connector->send(predecessor, Message(MessageType::Ack));
}

void Server::handleStateInitPhaseSecond(const MessagePair &messagePair) {
    if (messagePair.second.m_type != MessageType::Init) {
        //ERROR
        return;
    }

    successor = messagePair.first;
    state = ClientState::INIT_PHASE_THIRD;
    connector->send(successor, Message(MessageType::Init));
}

void Server::handleStateInitPhaseThird(const MessagePair &messagePair) {
    if (messagePair.second.m_type != MessageType::Ack) {
        //ERROR
        return;
    }

    state = ClientState::CONNECTION_ESTABLISHED;
    connector->send(predecessor, Message(MessageType::Ack));
}

void Server::handleStateConnectionEstablished(const MessagePair &messagePair) {

}

void Server::handleFinishing(const MessagePair &messagePair) {
    connector->send(successor, messagePair.second);
    state = ClientState::FINISHED;
}

void Server::stop() {
    Message msg;
    msg.m_type = MessageType::Terminate;
    // push any message to unlock the loop
    msgBuffer.push({"127.0.0.1", msg});
}
