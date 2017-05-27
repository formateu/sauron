//
// Created by Mateusz Forc and Wiktor Franus on 13.05.17.
//
#include "Server.h"

Server::Server(SharedMsgBufPtr mainBuffer,
               size_t port,
               ConfigBase *conf,
               Connector *inputConnector,
               ServerState state)
    : m_mainBuffer(mainBuffer)
    , m_halfRing1Buffer(std::make_shared<MessageBuffer>())
    , m_halfRing2Buffer(std::make_shared<MessageBuffer>())
    , m_state(state)
{
    if (conf == nullptr) {
        throw std::runtime_error("Invalid pointer to config file");
    }

    std::unique_ptr<ConfigBase> confPtr(conf);

    m_pos = (conf->m_ipVec.size() / 2) + 1;
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
                conf->m_ipVec.rbegin() + m_pos);
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
        m_connector->shutdownListenThread();
        listenThread.join();
        throw e;
    }

    m_connector->shutdownListenThread();
    listenThread.join();
}

void Server::runOneHalfRing() {
    HalfRing firstHalf(m_connector, m_mainBuffer, m_halfRing1Buffer, m_addrHalfRing1,
                       m_clientWorkSeconds, m_clientSleepSeconds);

    std::thread half1Thread([&firstHalf]() { firstHalf(); });

    mainLoop();

    half1Thread.join();
}

void Server::runTwoHalfRings() {
    HalfRing firstHalf(m_connector, m_mainBuffer, m_halfRing1Buffer, m_addrHalfRing1,
                       m_clientWorkSeconds, m_clientSleepSeconds);
    HalfRing secondHalf(m_connector, m_mainBuffer, m_halfRing2Buffer, m_addrHalfRing2,
                        m_clientWorkSeconds, m_clientSleepSeconds);

    std::thread half1Thread([&firstHalf]() { firstHalf(); });
    std::thread half2Thread([&secondHalf]() { secondHalf(); });

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
    std::string normalized1stRingAddr = normalizeAddress(m_addrHalfRing1[0]);
          << " " << normalized1stRingAddr << std::endl;

    while (true) {
        MessagePair messagePair = m_mainBuffer->pop();
        if (messagePair.first != "127.0.0.1" && messagePair.second.m_type != MessageType::Ack)
            m_connector->send(messagePair.first, Message(MessageType::Ack));

        if (messagePair.second.m_type == MessageType::Finish) {
            throw std::runtime_error("Server should not get that type of message from anyone.");
        } else if (messagePair.second.m_type == MessageType::Terminate) {
            stop(m_halfRing1Buffer);
            stop(m_halfRing2Buffer);
            break;
        }

        // dispatch messages to halfring
        if (messagePair.second.m_type != OneHalfInitFinish
            && messagePair.second.m_type != Measurement) {
            if (messagePair.first == normalized1stRingAddr) {
                m_halfRing1Buffer->push(messagePair);
            } else if (!m_addrHalfRing2.empty()) {
                m_halfRing2Buffer->push(messagePair);
            }
        } else {
            auto handler = m_stateRouter.find(m_state);

            if (handler != m_stateRouter.end()) {
                handler->second(messagePair);
            } else {
                throw std::runtime_error("[ERROR]: Unkown state.");
            }
        }
    }
}

void Server::handleStateWaitingForFirstHalf(const MessagePair &messagePair) {
    if (messagePair.second.m_type == MessageType::OneHalfInitFinish) {
        if (!m_addrHalfRing2.empty()) {
            m_state = ServerState::WAITING_FOR_SECOND_HALF;
        } else {
            handleStateWaitingForSecondHalf(messagePair);
        }
    }
}

void Server::handleStateWaitingForSecondHalf(const MessagePair &messagePair) {
    if (messagePair.second.m_type == MessageType::OneHalfInitFinish) {
        Message msg;
        msg.m_type = MessageType::Run;
        m_halfRing1Buffer->push({"127.0.0.1", msg});
        m_halfRing2Buffer->push({"127.0.0.1", msg});

        m_state = ServerState::WAITING_FOR_RESULTS;
    }
}

void Server::handleStateWaitingForResults(const MessagePair &messagePair) {
    if (messagePair.second.m_type == MessageType::Measurement) {
        std::unique_ptr<char> tmp(new char[INET6_ADDRSTRLEN]());
        inet_ntop(AF_INET6, messagePair.second.m_pipeAddress, tmp.get(), INET6_ADDRSTRLEN);
        std::cout << std::string(tmp.get())
                  << " : "
                  << messagePair.second.m_measureValue
                  << std::endl;
    }
}

void Server::handleFinishing() {
    Message msg;
    msg.m_type = MessageType::Finish;
    m_halfRing1Buffer->push({"127.0.0.1", msg});
    m_halfRing2Buffer->push({"127.0.0.1", msg});
}

void Server::stop() {
    //push terminate message to main buffer
    std::cout << "Stopping..." << std::endl;
    stop(m_mainBuffer);
}

void Server::stop(std::shared_ptr<MessageBufferBase> msgBuf) {
    Message msg;
    msg.m_type = MessageType::Terminate;
    msgBuf->push({"127.0.0.1", msg});
}

std::string Server::normalizeAddress(std::string address) {
    if (address.find('.') != std::string::npos) {
        address = "::ffff:" + address;
    }

    struct sockaddr_in6 addr;
    char straddr[INET6_ADDRSTRLEN];

    memset(&addr, 0, sizeof(addr));
    addr.sin6_family = AF_INET6;

    inet_pton(AF_INET6, address.data(), &(addr.sin6_addr));
    inet_ntop(AF_INET6, &addr.sin6_addr, straddr, sizeof(straddr));

    return std::string(straddr);
}
