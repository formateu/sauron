//
// Created by Mateusz Forc and Wiktor Franus on 13.05.17.
//
#include "Server.h"

Server::Server(MessageBuffer &mainBuffer,
               size_t port,
               ConfigBase *conf,
               Connector *inputConnector,
               ServerState state)
    : m_mainBuffer(mainBuffer)
    , m_state(state)
{
    if (conf == nullptr) {
        throw std::runtime_error("Invalid pointer to config file");
    }

    std::unique_ptr<ConfigBase> confPtr(conf);

    m_pos = conf->m_ipVec.size() / 2;
    m_clientWorkSeconds = conf->clientWorkSeconds;
    m_clientSleepSeconds = conf->clientSleepSeconds;

    if (inputConnector == nullptr) {
        m_connector.reset(new InternetConnector(m_mainBuffer, port));
    } else {
        m_connector.reset(inputConnector);
    }

    m_addrHalfRing1.insert(m_addrHalfRing1.begin(),
            conf->m_ipVec.begin(),
            conf->m_ipVec.begin() + m_pos);

    if (conf->m_ipVec.size() > 1) {
        m_addrHalfRing2.insert(m_addrHalfRing2.begin(),
                conf->m_ipVec.rbegin(),
                conf->m_ipVec.rbegin() + conf->m_ipVec.size() - m_pos);
    }
}

Server::~Server() {
    handleFinishing();
}

void Server::run() {
    std::thread listenThread(&Connector::listen, m_connector.get());

    try {
        if(!m_addrHalfRing2.empty()) {
            runTwoHalfRings();
        } else {
            runOneHalfRing();
        }
    } catch (std::runtime_error &e) {
        listenThread.detach();
        throw e;
    }

    listenThread.join();
}

void Server::runOneHalfRing() {
    HalfRing firstHalf(m_connector, m_mainBuffer, m_halfRing1Buffer, m_addrHalfRing1,
                       m_clientWorkSeconds, m_clientSleepSeconds);

    std::thread half1Thread(firstHalf);

    mainLoop();

    half1Thread.join();
}

void Server::runTwoHalfRings() {
    HalfRing firstHalf(m_connector, m_mainBuffer, m_halfRing1Buffer, m_addrHalfRing1,
                       m_clientWorkSeconds, m_clientSleepSeconds);
    HalfRing secondHalf(m_connector, m_mainBuffer, m_halfRing2Buffer, m_addrHalfRing2,
                        m_clientWorkSeconds, m_clientSleepSeconds);

    std::thread half1Thread(firstHalf);
    std::thread half2Thread(secondHalf);

    try {
        mainLoop();
    } catch (std::runtime_error &e) {
        handleFinishing();
        half1Thread.join();
        half2Thread.join();
        throw e;
    }

    half1Thread.join();
    half2Thread.join();
}

void Server::mainLoop() {
    m_state = ServerState::WAITING_FOR_FIRST_HALF;

    while (true) {
        MessagePair messagePair = m_mainBuffer.pop();

        if (messagePair.second.m_type == MessageType::Finish) {
            throw std::runtime_error("Server should not get that type of message from anyone.");
        } else if (messagePair.second.m_type == MessageType::Terminate) {
            stop(m_halfRing1Buffer);
            stop(m_halfRing2Buffer);
            break;
        }

        auto handler = m_stateRouter.find(m_state);

        if (handler != m_stateRouter.end()) {
            handler->second(messagePair);
        } else {
            throw std::runtime_error("[ERROR]: Unkown state.");
        }
    }
}

void Server::handleStateWaitingForFirstHalf(const MessagePair &messagePair) {
    if (messagePair.second.m_type == MessageType::OneHalfInitFinish) {
        if(!m_addrHalfRing2.empty()) {
            m_state = ServerState ::WAITING_FOR_SECOND_HALF;
        } else {
            handleStateWaitingForSecondHalf(messagePair);
        }
    }
}

void Server::handleStateWaitingForSecondHalf(const MessagePair &messagePair) {
    if (messagePair.second.m_type == MessageType::OneHalfInitFinish) {
        Message msg;
        msg.m_type = MessageType::Run;
        m_halfRing1Buffer.push({"127.0.0.1", msg});
        m_halfRing2Buffer.push({"127.0.0.1", msg});

        m_state = ServerState ::WAITING_FOR_RESULTS;
    }
}

void Server::handleStateWaitingForResults(const MessagePair &messagePair) {
    if (messagePair.second.m_type == MessageType::Measurement) {
        std::cout << messagePair.first
                  << " : "
                  << messagePair.second.m_measureValue
                  << std::endl;
    }
}

void Server::handleFinishing() {
    Message msg;
    msg.m_type = MessageType::Finish;
    m_halfRing1Buffer.push({"127.0.0.1", msg});
    m_halfRing2Buffer.push({"127.0.0.1", msg});
}

void Server::stop() {
    //push terminate message to main buffer
    stop(m_mainBuffer);
}

void Server::stop(MessageBuffer &msgBuf) {
    Message msg;
    msg.m_type = MessageType::Terminate;
    msgBuf.push({"127.0.0.1", msg});
}
