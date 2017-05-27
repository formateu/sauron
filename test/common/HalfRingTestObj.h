//
// Created by Mateusz Forc 27.05.17
//
#ifndef _HALFRING_TEST_OBJ_H_
#define _HALFRING_TEST_OBJ_H_

#include "HalfRing.h"

/**
 * Wrapper for Server class to access protected fields
 */
class HalfRingTestObj : public HalfRing {
public:
    HalfRingTestObj(ConnectorPtr &cntrPtr,
            std::shared_ptr<MessageBuffer> mainBuffer,
            std::shared_ptr<MessageBuffer> msgBuf,
            AddressVector& addressVector,
            int clientWorkSeconds,
            int clientSleepSeconds)
        : HalfRing(cntrPtr,
                mainBuffer,
                msgBuf,
                addressVector,
                clientWorkSeconds,
                clientSleepSeconds)
    {}

    void convertAddressTest(const std::string &input, std::string &output) {
        convertAddress(input, output);
    }
};

#endif // _HALFRING_TEST_OBJ_H_
