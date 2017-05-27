//
// Created by Mateusz Forc 19.05.17
//
#include <boost/test/unit_test.hpp>

#include "HalfRingTestObj.h"

BOOST_AUTO_TEST_CASE(test_halfring_ipv4_address_conversion) {
    // set up environment
    MessageBuffer msgBuffer;
    std::vector<std::string> addressVector;
    addressVector.emplace_back("192.168.1.1");
    auto connPtr = std::unique_ptr<Connector>(new MockConnector(msgBuffer));
    HalfRingTestObj halfRing(connPtr, msgBuffer, msgBuffer, addressVector, 0, 0);

    //tested conversion
    std::string output;
    halfRing.convertAddressTest(addressVector[0], output);

    //convert given ipv4 address to binary format without halfring conversion
    std::vector<int> addrTmp = {
        0,0,0,0,
        0,0,0,0,
        0,0,255,255,
        192,168,1,1
    };

    std::stringstream ss;
    std::move(addrTmp.begin(), addrTmp.end(), std::ostream_iterator<unsigned char>(ss, ""));

    BOOST_REQUIRE_EQUAL(output, ss.str());
}

BOOST_AUTO_TEST_CASE(test_halfring_ipv6_address_conversion) {
    // set up environment
    MessageBuffer msgBuffer;
    std::vector<std::string> addressVector;
    addressVector.emplace_back("0000:0000:0000:0000:0000:ffff:c0a8:0101");
    auto connPtr = std::unique_ptr<Connector>(new MockConnector(msgBuffer));
    HalfRingTestObj halfRing(connPtr, msgBuffer, msgBuffer, addressVector, 0, 0);

    //tested conversion
    std::string output;
    halfRing.convertAddressTest(addressVector[0], output);

    //convert given ipv4 address to binary format without halfring conversion
    std::vector<int> addrTmp = {
        0,0,0,0,
        0,0,0,0,
        0,0,255,255,
        192,168,1,1
    };

    std::stringstream ss;
    std::move(addrTmp.begin(), addrTmp.end(), std::ostream_iterator<unsigned char>(ss, ""));

    BOOST_REQUIRE_EQUAL(output, ss.str());
}
