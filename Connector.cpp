//
// Created by Mateusz Forc on 03.05.17.
//

#include <stdexcept>
#include <arpa/inet.h>
#include "Connector.h"

Connector::Connector(size_t listenPort) : m_listenPort(listenPort) {
    /*
     * ipv4 or ipv6 address resolve
     */
    struct sockaddr_in6 name;

    m_listenSocket = socket(AF_INET6, SOCK_DGRAM, 0);
    if (m_listenSocket == -1)
        throw std::runtime_error("Socket creation failed");

    int ipV6OnlyFalse = 0;
    if (setsockopt(m_listenSocket, IPPROTO_IPV6, IPV6_V6ONLY, (void *) &ipV6OnlyFalse, sizeof(int)) == -1)
        throw std::runtime_error("Setting IPv4 compatibility set failed");

    name.sin6_family = AF_INET6;
    name.sin6_addr = in6addr_any;
    name.sin6_port = htons(static_cast<uint16_t>(m_listenPort));

    if (bind(m_listenSocket, (struct sockaddr *) &name, sizeof name) == -1)
        throw std::runtime_error("Binding socket failed");
}

void Connector::send(const std::string &address, size_t port, const Message &msg) {
    struct sockaddr_in6 sa6;
    struct sockaddr_in sa;

    sa.sin_family = AF_INET;
    sa6.sin6_family = AF_INET6;
    sa.sin_port = htons(static_cast<uint16_t>(port));
    sa6.sin6_port = htons(static_cast<uint16_t>(port));

    if (inet_pton(AF_INET, address.data(), &(sa.sin_addr)) != 1) {
        if (inet_pton(AF_INET6, address.data(), &(sa6.sin6_addr)) != 1)
            throw std::runtime_error("Unrecognized IP address");

        if (sendto(m_listenSocket, (void*)&msg, sizeof msg, 0,(struct sockaddr *) &sa6, sizeof sa6) == -1)
            throw std::runtime_error("Sending message failed");
    }

    std::string ipv4mappedipv6addr = "::ffff:" + address;
    if (inet_pton(AF_INET6, ipv4mappedipv6addr.data(), &(sa6.sin6_addr)) != 1)
        throw std::runtime_error("Unrecognized IP address");

    if (sendto(m_listenSocket, (void*)&msg, sizeof msg, 0,(struct sockaddr *) &sa6, sizeof sa6) == -1)
        throw std::runtime_error("Sending message failed");
}

void Connector::listen(MessageBuffer &msgBuffer) {
    int recvMsgSize = 0;
    std::unique_ptr<Message> msg(new Message);
    struct sockaddr_in6 sa6;

    sa6.sin6_family = AF_INET6;
    sa6.sin6_addr = in6addr_any;
    sa6.sin6_port = htons(static_cast<uint16_t>(m_listenSocket));
    ssize_t size = sizeof sa6;

    while(true) {
        if ((recvMsgSize = recvfrom(m_listenSocket, msg.get(), sizeof(*msg.get()), 0, (struct sockaddr *) &sa6, &size)) == -1)
            throw std::runtime_error("Receiving message failed");

        //msgBuffer.insert(msg);
    }
}