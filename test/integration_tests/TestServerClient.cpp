#include <boost/test/unit_test.hpp>

#include "Message.h"
#include "MessageBuffer.h"
#include "Connector.h"
#include "ClientState.h"
#include "Client.h"
#include "Config.h"
#include "Server.h"
#include "ServerTestObj.h"
#include "../../src/Config.h"
#include "../../src/Connector.h"
#include "../common/ServerTestObj.h"
#include "../common/ClientTestObj.h"

BOOST_AUTO_TEST_CASE(asd) {
    MessageBuffer bufServerRead, bufServerWriteToClient1, bufServerWriteToClient2;

    MessageBuffer &bufClient1Read = bufServerWriteToClient1;
    MessageBuffer &bufClient1Write = bufServerRead;

    MessageBuffer &bufClient2Read = bufServerWriteToClient2;
    MessageBuffer &bufClient2Write = bufServerRead;

    Connector *connectorServerToClient1 = new MockConnector(bufServerWriteToClient1);
    Connector *connectorServerToClient2 = new MockConnector(bufServerWriteToClient2);
    Connector *connectorClient1 = new MockConnector(bufClient1Write);
    Connector *connectorClient2 = new MockConnector(bufClient2Write);

    ConfigBase *conf = new MockConfig();
    conf->m_ipVec.emplace_back("192.168.1.1");
    conf->m_ipVec.emplace_back("192.168.1.2");

    ServerTestObj server(bufServerRead, "127.0.0.1", 8000, conf, connectorServerToClient1);
    ClientTestObj client1(bufClient1Read, 8001, connectorClient1);
    ClientTestObj client2(bufClient2Read, 8002, connectorClient2);

}
