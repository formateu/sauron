//
// Created by Mateusz Forc and Wiktor Franus on 18.05.17.
//
#include "HalfRing.h"

HalfRing::HalfRing(ConnectorPtr &cntrPtr, std::shared_ptr<MessageBuffer> mainBuffer,
        std::shared_ptr<MessageBuffer> msgBuf, AddressVector &addressVector,
        int clientWorkSeconds, int clientSleepSeconds)
    : m_connector(cntrPtr)
    , m_mainBuf(mainBuffer)
    , m_halfRingBuf(msgBuf)
    , m_addrVec(addressVector)
    , m_state(HalfRingState::WAITING_FOR_ACK)
    , m_clientWorkSeconds(clientWorkSeconds)
    , m_clientSleepSeconds(clientSleepSeconds)
{}

HalfRing::~HalfRing() {

}

void HalfRing::operator()() {
    std::string fromAddr;
    convertAddress(m_addrVec[0], fromAddr);
    Message msg(MessageType::Init, fromAddr.c_str());
    m_connector->send(m_addrVec[0], msg);
    m_currAddr = 0;

    while (m_state != HalfRingState::FINISHED) {
        MessagePair messagePair = m_halfRingBuf->pop();

        if (messagePair.second.m_type == MessageType::Finish) {
            handleFinished(messagePair);
            continue;
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
}

void HalfRing::handleStateWaitingForAck(const MessagePair &messagePair) {
    if (messagePair.second.m_type == MessageType::Ack) {
        if (m_currAddr == m_addrVec.size()) {
            m_state = HalfRingState::IDLE;
        } else {
            m_state = HalfRingState::WAITING_FOR_INIT_OK;
        }
    }
}

Message HalfRing::generateMessageTo(MessageType type, const std::string& target) {
    std::string fromAddr;
    convertAddress(target, fromAddr);
    Message msg(type, fromAddr.c_str());

    return msg;
}

void HalfRing::handleStateWaitingForInitOK(const MessagePair &messagePair) {
    if (messagePair.second.m_type == MessageType::InitOk) {
        if (m_currAddr == m_addrVec.size() - 3) {
            m_connector->send(m_addrVec[0],
                generateMessageTo(MessageType::InitLast, m_addrVec[m_addrVec.size()-2]));

            m_state = HalfRingState::WAITING_FOR_ACK;
        } else if (m_currAddr == m_addrVec.size() - 2) {
            m_connector->send(m_addrVec[0],
                generateMessageTo(MessageType::InitLast, m_addrVec[m_addrVec.size()-1]));
            m_state = HalfRingState::WAITING_FOR_ACK;
        } else if (m_currAddr == m_addrVec.size() - 1) {
            auto msgSenderPair = std::make_pair("127.0.0.1",
                    Message(MessageType::OneHalfInitFinish));
            m_mainBuf->push(msgSenderPair);
            m_state = HalfRingState::INITIALIZATION_FINISHED;
        } else {
            m_connector->send(m_addrVec[0],
                generateMessageTo(MessageType::Init, m_addrVec[m_currAddr+1]));
            m_state = HalfRingState::WAITING_FOR_ACK;
        }

        ++m_currAddr;
    }
}

void HalfRing::handleStateInitializationFinished(const MessagePair &messagePair) {
    if (messagePair.second.m_type == MessageType::Run) {
        Message msg(MessageType::Run);
        msg.m_activePeriod = m_clientWorkSeconds;
        msg.m_inactivePeriod = m_clientSleepSeconds;
        memset(&msg.m_pipeAddress, 255, 16);
        m_connector->send(m_addrVec[0], msg);
        m_state = HalfRingState::WAITING_FOR_ACK;
    }
}

void HalfRing::handleFinished(const MessagePair &messagePair) {
    m_connector->send(m_addrVec[0], Message(MessageType::Finish));
    m_state = HalfRingState::FINISHED;
}


void HalfRing::handleIdle(const MessagePair &messagePair) {
}
