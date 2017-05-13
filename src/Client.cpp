//
// Created by Przemyslaw Kopanski and Grzegorz Staniszewski on 04.05.17.
//
#include "Client.h"

Client::Client(MessageBuffer &msgBuffer,
               const std::string &address,
               size_t port,
               Connector *inputConnector,
               ClientState state)
    : mAddress(address), msgBuffer(msgBuffer), state(state)
{
    if (inputConnector == nullptr) {
        connector.reset(new InternetConnector(msgBuffer, port));
    } else {
        connector.reset(inputConnector);
    }
}

void Client::run() {
    std::thread listenThread(&Connector::listen, connector.get());

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

    listenThread.join();
}

ClientState Client::getClientState() {
    return state;
}

void Client::handleStateInitPhaseFirst(const MessagePair &messagePair) {
    if (messagePair.second.m_type != MessageType::Init) {
        //ERROR
        return;
    }

    predecessor = messagePair.first;
    state = ClientState::INIT_PHASE_SECOND;
    connector->send(predecessor, Message(MessageType::Ack));
}

void Client::handleStateInitPhaseSecond(const MessagePair &messagePair) {
    if (messagePair.second.m_type != MessageType::Init) {
        //ERROR
        return;
    }

    successor = messagePair.first;
    state = ClientState::INIT_PHASE_THIRD;
    connector->send(successor, Message(MessageType::Init));
}

void Client::handleStateInitPhaseThird(const MessagePair &messagePair) {
    if (messagePair.second.m_type != MessageType::Ack) {
        //ERROR
        return;
    }

    state = ClientState::CONNECTION_ESTABLISHED;
    connector->send(predecessor, Message(MessageType::Ack));
}

void Client::handleStateConnectionEstablished(const MessagePair &messagePair) {
    // Start measurement
    auto measureTask = [this]() {
        std::default_random_engine generator;
        std::uniform_int_distribution<int> distribution(1,20);
        this->sendMeasurementInfo(distribution(generator));
    };

    measurementTimer(10, measureTask);
}

void Client::handleFinishing(const MessagePair &messagePair) {
    connector->send(successor, messagePair.second);
    state = ClientState::FINISHED;
}

void Client::stop() {
    Message msg;
    msg.m_type = MessageType::Terminate;
    // push any message to unlock the loop
    msgBuffer.push({"127.0.0.1", msg});
}


void Client::sendMeasurementInfo(int measureval) {
    // TODO: send to successor or predecessor?
    Message msg(Measurement);
    msg.val = measureval;
    connector->send(predecessor, measureval);
}
