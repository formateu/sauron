//
// Created by Mateusz Forc on 03.05.17.
//

#include <stdexcept>
#include <arpa/inet.h>

#include "Connector.h"

InternetConnector::InternetConnector(MessageBuffer &msgBuffer, size_t listenPort)
        : Connector(msgBuffer)
        , m_port(listenPort) {
    struct sockaddr_in6 name;

    m_listenSocket = socket(AF_INET6, SOCK_DGRAM, 0);

    if (m_listenSocket == -1) {
        throw std::runtime_error("Socket creation failed");
    }

    int ipV6OnlyFalse = 0;

    if (setsockopt(m_listenSocket, IPPROTO_IPV6, IPV6_V6ONLY, (void *) &ipV6OnlyFalse, sizeof(int)) == -1) {
        throw std::runtime_error("Setting IPv4 compatibility set failed");
    }

    name.sin6_family = AF_INET6;
    name.sin6_addr = in6addr_any;
    name.sin6_port = htons(static_cast<uint16_t>(m_port));

    if (bind(m_listenSocket, (struct sockaddr *) &name, sizeof name) == -1) {
        throw std::runtime_error("Binding socket failed");
    }
}

void InternetConnector::send(const std::string &address, const Message &msg) {
    struct sockaddr_in6 sa6;
    struct sockaddr_in sa;

    sa.sin_family = AF_INET;
    sa.sin_port = htons(static_cast<uint16_t>(m_port));
    sa6.sin6_family = AF_INET6;
    sa6.sin6_port = htons(static_cast<uint16_t>(m_port));

    if (inet_pton(AF_INET, address.data(), &(sa.sin_addr)) != 1) {
        if (inet_pton(AF_INET6, address.data(), &(sa6.sin6_addr)) != 1) {
            throw std::runtime_error("Unrecognized IP address");
        }

        if (sendto(m_listenSocket, (void*)&msg, sizeof msg, 0,(struct sockaddr *) &sa6, sizeof sa6) == -1) {
            throw std::runtime_error("Sending message failed");
        }

        return;
    }

    /*
     * Decimal ipv4 address is combined with hexadecimal prefix
     * to achieve ipv4 mapped ipv6 address
     */
    std::string ipv4mappedipv6addr = "::ffff:" + address;

    if (inet_pton(AF_INET6, ipv4mappedipv6addr.data(), &(sa6.sin6_addr)) != 1) {
        throw std::runtime_error("Unrecognized IP address");
    }

    if (sendto(m_listenSocket, (void*)&msg, sizeof msg, 0,(struct sockaddr *) &sa6, sizeof sa6) == -1) {
        throw std::runtime_error("Sending message failed");
    }
}

void InternetConnector::listen() {
    int recvMsgSize = 0;
    std::unique_ptr<Message> msg(new Message());
    struct sockaddr_in6 sa6;
    const size_t senderAddrLen = 16;
    std::unique_ptr<char> senderAddr(new char[senderAddrLen]);

    sa6.sin6_family = AF_INET6;
    sa6.sin6_addr = in6addr_any;
    sa6.sin6_port = htons(static_cast<uint16_t>(m_listenSocket));
    unsigned int size = sizeof sa6;

    while (true) {
        recvMsgSize = recvfrom(m_listenSocket, msg.get(), sizeof(*msg.get()), 0, (struct sockaddr *) &sa6, &size);

        if (recvMsgSize == -1) {
            throw std::runtime_error("Receiving message failed");
        }

        std::unique_ptr<char> senderAddr(const_cast<char*>(inet_ntop(AF_INET6, (void*) &sa6.sin6_addr, senderAddr.get(), senderAddrLen)));
        if (senderAddr.get() == nullptr)
            throw std::runtime_error("Getting sender address failed");

        msgBuffer.push(std::make_pair(std::string(senderAddr.get()), *msg.get()));
    }
}


void MockConnector::send(const std::string &address, const Message &msg) {
  sentMessagesStack.push_back({address, msg});
}

void MockConnector::listen() {
  // modify if needed, currently messages can be pushed to be listened via MessageBuffer
}
