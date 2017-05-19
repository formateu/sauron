//
// Created by Przemyslaw Kopanski and Grzegorz Staniszewski on 04.05.17.
//
#include "Client.h"

Client::Client(MessageBuffer &msgBuffer,
               const std::string &address,
               size_t port,
               Connector *inputConnector,
               ClientState state)
    : mAddress(address), msgBuffer(msgBuffer),
      state(state), amILast(0), isActive(1),
      predecessor(""), successor("")
{
    if (inputConnector == nullptr) {
        connector.reset(new InternetConnector(msgBuffer, port));
    } else {
        connector.reset(inputConnector);
    }
}

void Client::run() {
    std::thread listenThread(&Connector::listen, connector.get());

    while (state != ClientState::FINISH) {
        MessagePair messagePair = msgBuffer.pop();

        // awaiting for being active again
        if (!isActive) {
            // ignoring packets that sent when client is inactive
            continue;
        }

        // send ack, as we received message
        if (messagePair.second.m_type != MessageType::Ack) {
            sendAck(messagePair.first);
        }

        if (messagePair.second.m_type == MessageType::Finish) {
            handleFinishing(messagePair);
        } else if (messagePair.second.m_type == MessageType::Terminate) {
            break;
        }

        auto handler = stateRouter.find(state);

        if (handler != stateRouter.end()) {
            handler->second(messagePair);
        } else {
            throw std::runtime_error("[ERROR]: Unkown state. ");
        }
    }

    listenThread.join();
}

ClientState Client::getClientState() {
    return state;
}

void Client::handleStateInitPhaseFirst(const MessagePair &messagePair) {
    if (messagePair.second.m_type == MessageType::Init) {
        predecessor = messagePair.first;
        state = ClientState::INIT_PHASE_SECOND;
        sendMessage(predecessor, Message(MessageType::InitOk));
    }
}

void Client::handleStateInitPhaseSecond(const MessagePair &messagePair) {
    switch (messagePair.second.m_type) {
        case MessageType::Init: case MessageType::InitLast:
            successor = messagePair.first;
            //successor = messagePair.second.m_pipeAddress;
            if (messagePair.second.m_type == MessageType::InitLast) {
                amILast = 1;
            }
            state = ClientState::CONNECTION_ESTABLISHED;
            break;
        case MessageType::Run:
            amILast = 1; // but successor undefined
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
            sendMessage(successor, messagePair.second);
            break;
        case MessageType::InitOk:
            sendMessage(predecessor, messagePair.second);
            break;
        case MessageType::Run:
            handleIncomingRun(messagePair.second);
            break;
        default:
            break;
    }
}

void Client::handleIncomingRun(const Message& msg) {
     if (!amILast) {
         sendMessage(successor, msg);
     }
     state = ClientState::MEASURE_TIME;
     // TODO: extract values from message
     startMeasurement(2, 1);
}

void Client::handleMeasureTime(const MessagePair& messagePair) {
    switch (messagePair.second.m_type) {
        case MessageType::Measurement:
            // resend messege in opposing direction
            if (messagePair.first == successor) {
                sendMessage(predecessor, messagePair.second);
            } else {
                sendMessage(successor, messagePair.second);
            }
            break;
        case MessageType::Finish:
            if (!amILast) {
                sendMessage(successor, messagePair.second);
            }
            state = ClientState::FINISH;
            break;
        defaut:
            break;
    }
}

void Client::handleFinishing(const MessagePair &messagePair) {
    if (!amILast) {
        sendMessage(successor, messagePair.second);
    }
    state = ClientState::FINISH;
}

void Client::stop() {
    Message msg;
    msg.m_type = MessageType::Terminate;
    // push any message to unlock the loop
    msgBuffer.push({"127.0.0.1", msg});
}

void Client::startMeasurement(int activePeriod, int inactivePeriod) {
    auto measureTask = [this, activePeriod]() {
        isActive = true;
        std::default_random_engine generator;
        std::uniform_int_distribution<int> distribution(1,20);
        this->sendMeasurementInfo(distribution(generator));
        std::this_thread::sleep_for(std::chrono::seconds(activePeriod));
        isActive = false;
    };

    measurementTimer = std::make_unique<Timer>(inactivePeriod*1000, measureTask);
}

void Client::stopMeasurement() {
    // doesn't block
    measurementTimer.reset(nullptr);
}

void Client::sendMeasurementInfo(int measureval) {
    // TODO: provide criteria to whom should message be sent
    // (either predecessor or successor)
    Message msg(Measurement);
    msg.m_measureValue = measureval;
    sendMessage(predecessor, msg);
}

void Client::sendAck(std::string address) {
    Message msg(Ack);
    connector->send(address, msg);
}

void Client::sendMessage(std::string address, const Message& msg) {
    connector->send(address, msg);
    // await for ACK
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    MessagePair rmsg;
    if (!msgBuffer.tryPop(rmsg)) {
        // ack didn't come in time, stop the client
        stop();
    } else if (rmsg.second.m_type != MessageType::Ack) {
        // popped message is not ack, then ack didnt come neither, repush it
        msgBuffer.push(rmsg);
        stop();
    }
}
