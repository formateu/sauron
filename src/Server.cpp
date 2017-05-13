//
// Created by Mateusz Forc and Wiktor Franus on 13.05.17.
//
#include "Server.h"

Server::Server(MessageBuffer &msgBuffer,
               const std::string &address,
               size_t port,
               const Config &conf,
               Connector *inputConnector,
               ServerState state)
    : mAddress(address)
    , msgBuffer(msgBuffer)
    , m_pos(conf.m_ipVec.size() / 2)
    , m_clientWorkSeconds(conf.clientWorkSeconds)
    , m_clientSleepSeconds(conf.clientSleepSeconds)
    , m_state(state)
{
    if (inputConnector == nullptr) {
        connector.reset(new InternetConnector(msgBuffer, port));
    } else {
        connector.reset(inputConnector);
    }

    m_addrHalfRing1.insert(m_addrHalfRing1.begin(),
                           conf.m_ipVec.begin(),
                           conf.m_ipVec.begin() + m_pos + 1);

    m_addrHalfRing2.insert(m_addrHalfRing2.begin(),
                           conf.m_ipVec.rbegin(),
                           conf.m_ipVec.rbegin() + conf.m_ipVec.size() - m_pos);
}

void Server::run() {
    std::thread listenThread(&Connector::listen, connector.get());

    std::unique_ptr<MessageBuffer> msgBuf1Ptr(new MessageBuffer());
    std::unique_ptr<MessageBuffer> msgBuf2Ptr(nullptr);

    //if (m_addrList.size() > 1) {
    msgBuf2Ptr.reset(new MessageBuffer());

    HalfRing firstHalf(connector, msgBuffer, msgBuf1Ptr, m_addrHalfRing1);
    HalfRing secondHalf(connector, msgBuffer, msgBuf1Ptr, m_addrHalfRing2);

    std::thread half1Thread(firstHalf);
    std::thread half2Thread(secondHalf);

    m_state = ServerState::WAITING_FOR_FIRST_HALF;

    while (m_state != ServerState::FINISHED) {
        MessagePair messagePair = msgBuffer.pop();

//        if (messagePair.second.m_type == MessageType::Finish) {
//            handleFinishing(messagePair);
//        } else if (messagePair.second.m_type == MessageType::Terminate){
//            break;
//        }

        auto handler = stateRouter.find(m_state);

        if (handler != stateRouter.end()) {
            handler->second(messagePair);
        } else {
            throw std::runtime_error("[ERROR]: Unkown state. ");
        }
    }

    listenThread.join();
    half1Thread.join();
    half2Thread.join();
}

void Server::handleStateWaitingForFirstHalf(const MessagePair &messagePair) {
    if (messagePair.second.m_type != MessageType::OneHalfInitFinish) {
        //ERROR
        return;
    }

    m_state = ServerState ::WAITING_FOR_SECOND_HALF;

}

void Server::handleStateWaitingForSecondHalf(const MessagePair &messagePair) {
    if (messagePair.second.m_type != MessageType::OneHalfInitFinish) {
        //ERROR
        return;
    }

    m_state = ServerState ::WAITING_FOR_RESULTS;
}

void Server::handleStateWaitingForResults(const MessagePair &messagePair) {

}

void Server::handleFinishing(const MessagePair &messagePair) {

}

void Server::stop() {

}