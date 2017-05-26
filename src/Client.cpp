//
// Created by Przemyslaw Kopanski and Grzegorz Staniszewski on 04.05.17.
//
#include "Client.h"

Client::Client(MessageBuffer &msgBuffer,
               size_t port,
               Connector *inputConnector,
               ClientState state)
    : m_msgBuffer(msgBuffer),
      m_state(state), m_amILast(0), m_isActive(1),
      m_predecessor(""), m_successor("")
{
    if (inputConnector == nullptr) {
        m_connector.reset(new InternetConnector(m_msgBuffer, port));
    } else {
        m_connector.reset(inputConnector);
    }
}

void Client::run() {
    std::thread listenThread(&Connector::listen, m_connector.get());

    while (m_state != ClientState::FINISH) {
        MessagePair messagePair = m_msgBuffer.pop();

        // awaiting for being active again
        if (!m_isActive) {
            // ignoring packets that sent when client is inactive
            continue;
        }

        // send ack, as we received message
        if (messagePair.second.m_type != MessageType::Ack) {
            sendAck(messagePair.first);
        }

        if (messagePair.second.m_type == MessageType::Finish) {
            handleFinish(messagePair);
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

    listenThread.join();
}

ClientState Client::getClientState() {
    return m_state;
}

void Client::handleStateInitPhaseFirst(const MessagePair &messagePair) {
    if (messagePair.second.m_type == MessageType::Init) {
        m_predecessor = messagePair.first;
        m_state = ClientState::INIT_PHASE_SECOND;
        sendMessage(m_predecessor, Message(MessageType::InitOk));
    }
}

void Client::handleStateInitPhaseSecond(const MessagePair &messagePair) {
    switch (messagePair.second.m_type) {
        case MessageType::Init: case MessageType::InitLast:
            m_successor = messagePair.first;
            //m_successor = messagePair.second.m_pipeAddress;
            if (messagePair.second.m_type == MessageType::InitLast) {
                m_amILast = 1;
            }
            m_state = ClientState::CONNECTION_ESTABLISHED;
            break;
        case MessageType::Run:
            m_amILast = 1; // but successor undefined
            handleIncomingRun(messagePair.second);
            break;
        default:
            break;
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
     if (!m_amILast) {
         sendMessage(m_successor, msg);
     }
     m_state = ClientState::MEASURE_TIME;
     // TODO: extract values from message
     startMeasurement(2, 1);
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
    Message msg;
    msg.m_type = MessageType::Terminate;
    // push any message to unlock the loop
    m_msgBuffer.push({"127.0.0.1", msg});
}

void Client::startMeasurement(int activePeriod, int inactivePeriod) {
    auto measureTask = [this, activePeriod]() {
        m_isActive = true;
        std::default_random_engine generator;
        std::uniform_int_distribution<int> distribution(1,20);
        this->sendMeasurementInfo(distribution(generator));
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
    // TODO: provide criteria to whom should message be sent
    // (either predecessor or successor)
    Message msg(Measurement);
    msg.m_measureValue = measureval;
    sendMessage(m_predecessor, msg);
}

void Client::sendAck(std::string address) {
    Message msg(Ack);
    m_connector->send(address, msg);
}

void Client::sendMessage(std::string address, const Message& msg) {
    m_connector->send(address, msg);
    // await for ACK
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    MessagePair rmsg;
    if (!m_msgBuffer.tryPop(rmsg)) {
        // ack didn't come in time, stop the client
        stop();
    } else if (rmsg.second.m_type != MessageType::Ack) {
        // popped message is not ack, then ack didnt come neither, repush it
        m_msgBuffer.push(rmsg);
        stop();
    }
}
