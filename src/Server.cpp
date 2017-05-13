//
// Created by Mateusz Forc and Wiktor Franus on 13.05.17.
//
#include "Server.h"

Server::Server(MessageBuffer &msgBuffer,
               const std::string &address,
               size_t port,
               const Config &conf,
               Connector *inputConnector)
    : mAddress(address)
    , msgBuffer(msgBuffer)
    , m_addrList(conf.m_ipVec)
    , m_pos(conf.m_ipVec.size() / 2)
    , m_clientWorkSeconds(conf.clientWorkSeconds)
    , m_clientSleepSeconds(conf.clientSleepSeconds)
{
    if (inputConnector == nullptr) {
        connector.reset(new InternetConnector(msgBuffer, port));
    } else {
        connector.reset(inputConnector);
    }
}

void Server::run() {

    if (m_addrList.size() < 2) {
        return;
    }

    std::thread listenThread(&Connector::listen, connector.get());

    std::unique_ptr<MessageBuffer> msgBuf1Ptr(new MessageBuffer());
    std::unique_ptr<MessageBuffer> msgBuf2Ptr(nullptr);

    //if (m_addrList.size() > 1) {
    msgBuf2Ptr.reset(new MessageBuffer());

    auto threadRunner = [&](){ halfRingRun(msgBuf1Ptr, 0, m_pos); };
    auto thread2Runner = [&](){ halfRingRun(msgBuf2Ptr, m_pos + 1, m_addrList.size() - 1); };

    std::thread half1Thread(threadRunner);
    std::thread half2Thread(thread2Runner);

    /**
     * Place for main loop
     */

    listenThread.join();
    half1Thread.join();
    half2Thread.join();
}

void Server::halfRingRun(std::unique_ptr<MessageBuffer> &msgBufPtr,
                         size_t ipRangeFirst, size_t ipRangeLast) {

}

void Server::stop() {

}
