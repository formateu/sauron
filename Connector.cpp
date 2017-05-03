//
// Created by Mateusz Forc on 03.05.17.
//

#include <stdexcept>
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
    if (setsockopt(m_listenSocket, IPPROTO_IPV6, IPV6_V6ONLY, (void *)&ipV6OnlyFalse, sizeof(int)) == -1)
        throw std::runtime_error("Setting IPv4 compatibility set failed");

    name.sin6_family = AF_INET6;
    name.sin6_addr = in6addr_any;
    name.sin6_port = htons(static_cast<uint16_t>(m_listenPort));

    if (bind(m_listenSocket, (struct sockaddr *) &name, sizeof name) == -1)
       throw std::runtime_error("Binding socket failed");
}

void Connector::send(const std::string &address, size_t port, const Message &msg)
{

}