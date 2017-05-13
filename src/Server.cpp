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

    //TODO: throw an exception when ip vector is empty

    m_addrHalfRing1.insert(m_addrHalfRing1.begin(),
                           conf.m_ipVec.begin(),
                           conf.m_ipVec.begin() + m_pos + 1);

    m_addrHalfRing2.insert(m_addrHalfRing2.begin(),
                           conf.m_ipVec.rbegin(),
                           conf.m_ipVec.rbegin() + conf.m_ipVec.size() - m_pos);
}

Server::~Server() {
    handleFinishing();
    std::cout << "flet" << std::endl;
}

void Server::run() {
    std::thread listenThread(&Connector::listen, connector.get());


    HalfRing firstHalf(connector, msgBuffer, msgBufferHalfRing1, m_addrHalfRing1);
    HalfRing secondHalf(connector, msgBuffer, msgBufferHalfRing2, m_addrHalfRing2);

    std::thread half1Thread(firstHalf);
    std::thread half2Thread(secondHalf);

    m_state = ServerState::WAITING_FOR_FIRST_HALF;

    while (true) {
        MessagePair messagePair = msgBuffer.pop();

        if (messagePair.second.m_type == MessageType::Finish) {
            throw std::runtime_error("Server should not get that type of message from anyone.");
        } else if (messagePair.second.m_type == MessageType::Terminate) {
            stop(msgBufferHalfRing1);
            stop(msgBufferHalfRing2);
            break;
        }

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

    Message msg;
    msg.m_type = MessageType::Run;
    msgBufferHalfRing1.push({"127.0.0.1", msg});
    msgBufferHalfRing2.push({"127.0.0.1", msg});

    m_state = ServerState ::WAITING_FOR_RESULTS;
}

void Server::handleStateWaitingForResults(const MessagePair &messagePair) {
    if (messagePair.second.m_type != MessageType::Measurement) {
        //ERROR
        return;
    }

    std::cout << messagePair.first
              << " : "
              << messagePair.second.m_measureValue
              << std::endl;
}

void Server::handleFinishing() {
    Message msg;
    msg.m_type = MessageType::Finish;
    msgBufferHalfRing1.push({"127.0.0.1", msg});
    msgBufferHalfRing2.push({"127.0.0.1", msg});
    m_state = ServerState::FINISHED;
}

void Server::stop() {
    //push terminate message to main buffer
    stop(msgBuffer);
}

void Server::stop(MessageBuffer &msgBuf) {
    Message msg;
    msg.m_type = MessageType::Terminate;
    msgBuf.push({"127.0.0.1", msg});
}
