//
// Created by Przemyslaw Kopanski and Grzegorz Staniszewski on 04.05.17.
//
#include "Client.h"
#define DEBUG (std::cout << "client: ")

Client::Client(MessageBuffer &msgBuffer,
               size_t port,
               Connector *inputConnector,
               ClientState state)
    : m_msgBuffer(msgBuffer),
      m_state(state), m_amILast(0), m_isActive(1),
      m_predecessor(""), m_successor("")
{
    if (inputConnector == nullptr) {
        m_connector.reset(new InternetConnector(msgBuffer, port));
    } else {
        m_connector.reset(inputConnector);
    }
}

void Client::run() {
    std::thread listenThread(&Connector::listen, m_connector.get());

    while (m_state != ClientState::FINISH) {
        if (m_state == ClientState::CONNECTION_ESTABLISHED)
          DEBUG << "connection established..\n";
        MessagePair messagePair = m_msgBuffer.pop();
        //DEBUG << "popping message: " << messagePair.first << " " << messagePair.second.m_type << std::endl;

        // awaiting for being active again
        if (!m_isActive) {
            // ignoring packets that sent when client is inactive
            continue;
        }

        // send ack, as we received message
        if (messagePair.first != "127.0.0.1" && messagePair.second.m_type != MessageType::Ack) {
            sendAck(messagePair.first);
        }
        if (messagePair.second.m_type == MessageType::Ack)
            m_msgBuffer.push(messagePair);

        if (messagePair.second.m_type == MessageType::Finish) {
            handleFinish(messagePair);
            break;
        } else if (messagePair.second.m_type == MessageType::Terminate) {
            break;
        }

        auto handler = m_stateRouter.find(m_state);

        if (handler != m_stateRouter.end()) {
            handler->second(messagePair);
        } else {
            throw std::runtime_error("[ERROR]: Unkown state. ");
        }
    }

    m_connector->shutdownListenThread();
    listenThread.join();
}

ClientState Client::getClientState() {
    return m_state;
}

void Client::handleStateInitPhaseFirst(const MessagePair &messagePair) {
    switch (messagePair.second.m_type) {
        case MessageType::Init:
            m_predecessor = messagePair.first;
            //DEBUG << "handling 1st state..\n";

            // client was recipient of this message, so we can extract
            // our own ip address
            std::copy(std::begin(messagePair.second.m_pipeAddress),
                std::end(messagePair.second.m_pipeAddress),
                std::begin(m_address));
            m_state = ClientState::INIT_PHASE_SECOND;
            //DEBUG << "sending initok..\n";
            sendMessage(m_predecessor, Message(MessageType::InitOk));
            break;

        case MessageType::InitLast:
            m_predecessor = messagePair.first;
            //DEBUG << "handling 1st state, initlast..\n";
            m_amILast = 1;

            // client was recipient of this message, so we can extract
            // our own ip address
            std::copy(std::begin(messagePair.second.m_pipeAddress),
                std::end(messagePair.second.m_pipeAddress),
                std::begin(m_address));
            m_state = ClientState::INIT_PHASE_LAST_SECOND;
            sendMessage(m_predecessor, Message(MessageType::InitOk));
            break;
    }
}

void Client::handleStateInitPhaseSecond(const MessagePair &messagePair) {
    switch (messagePair.second.m_type) {
        case MessageType::Init: case MessageType::InitLast:
            m_successor = convertAddrToString(messagePair.second.m_pipeAddress);
            //DEBUG << "assigning successor addr - " << m_successor << std::endl;
            m_state = ClientState::CONNECTION_ESTABLISHED;
            sendMessage(m_successor, messagePair.second);
            break;
        case MessageType::Run:
            m_amILast = 1;
            handleIncomingRun(messagePair.second);
            break;
    }
}

void Client::handleStateInitPhaseLastSecond(const MessagePair &messagePair) {
    if (messagePair.second.m_type == MessageType::InitLast) {
        // extracting node's address after bar
        m_successor = convertAddrToString(messagePair.second.m_pipeAddress);
        //DEBUG << "assigning successor addr (initlast)- " << m_successor << std::endl;
        m_state = ClientState::CONNECTION_ESTABLISHED;
        sendMessage(m_predecessor, Message(MessageType::InitOk));
    }
}

void Client::handleStateConnectionEstablished(const MessagePair &messagePair) {
    switch (messagePair.second.m_type) {
        case MessageType::Init:
        case MessageType::InitLast:
            sendMessage(m_successor, messagePair.second);
            break;
        case MessageType::InitOk:
            sendMessage(m_predecessor, messagePair.second);
            break;
        case MessageType::Run:
            handleIncomingRun(messagePair.second);
            break;
        default:
            break;
    }
}

void Client::handleIncomingRun(const Message& msg) {
  DEBUG << "starting measurement..\n";
    startMeasurement(msg.m_activePeriod, msg.m_inactivePeriod);
    if (!m_amILast) {
        DEBUG << "piping run to " << m_successor << std::endl;
        sendMessage(m_successor, msg);
    }
    m_state = ClientState::MEASURE_TIME;
}

void Client::handleMeasureTime(const MessagePair& messagePair) {
    switch (messagePair.second.m_type) {
        case MessageType::Measurement:
            // resend messege in opposing direction
            if (messagePair.first == m_successor) {
                sendMessage(m_predecessor, messagePair.second);
            } else {
                sendMessage(m_successor, messagePair.second);
            }
            break;
        case MessageType::Finish:
            if (!m_amILast) {
                sendMessage(m_successor, messagePair.second);
            }
            m_state = ClientState::FINISH;
            break;
        defaut:
            break;
    }
}

void Client::handleFinish(const MessagePair &messagePair) {
    if (!m_amILast) {
        sendMessage(m_successor, messagePair.second);
    }
    m_state = ClientState::FINISH;
}

void Client::stop() {
    std::cout << "Stopping client..." << std::endl;
    Message msg;
    msg.m_type = MessageType::Terminate;
    // push any message to unlock the loop
    m_msgBuffer.push({"127.0.0.1", msg});
}

void Client::startMeasurement(unsigned activePeriod, unsigned inactivePeriod) {
    DEBUG << activePeriod << "--" << inactivePeriod << std::endl;
    auto measureTask = [this, activePeriod]() {
        m_isActive = true;
        std::random_device rd;
        std::default_random_engine generator(rd());
        std::uniform_int_distribution<int> distribution(1,20);
        this->sendMeasurementInfo(distribution(generator));
        DEBUG << "sleeping..\n";
        std::this_thread::sleep_for(std::chrono::seconds(activePeriod));
        m_isActive = false;
    };

    m_measurementTimer = std::make_unique<Timer>(inactivePeriod*1000, measureTask);
}

void Client::stopMeasurement() {
    // doesn't block
    m_measurementTimer.reset(nullptr);
}

void Client::sendMeasurementInfo(int measureval) {
    Message msg(Measurement);
    std::copy(std::begin(m_address), std::end(m_address), std::begin(msg.m_pipeAddress));
    msg.m_measureValue = measureval;
    DEBUG << "sending ready measurement..\n";
    sendMessage(m_predecessor, msg);
}

void Client::sendAck(std::string address) {
    Message msg(Ack);
    DEBUG << "sending ack to " << address << std::endl;
    m_connector->send(address, msg);
}

void Client::sendMessage(std::string address, const Message& msg) {
    m_connector->send(address, msg);
    std::future<MessagePair> rmsg = std::async(std::launch::async, [this]() {
        return m_msgBuffer.pop();
    });
    std::future_status status;
    status = rmsg.wait_for(std::chrono::milliseconds(1000));

    if (status != std::future_status::ready) {
        if (rmsg.get().second.m_type != MessageType::Ack) {
            // silently ignore..
            m_msgBuffer.push(rmsg.get());
        } else {
            DEBUG << "lol where iz ack from " << address << "..\n";
            stop();
        }
    }
}

std::string Client::convertAddrToString(const unsigned char* addr) {
    std::unique_ptr<char> tmp(new char[INET6_ADDRSTRLEN]());
    inet_ntop(AF_INET6, (void*)addr,
        tmp.get(), INET6_ADDRSTRLEN);
    return std::string(tmp.get());
}
