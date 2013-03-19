/*
GMLAN.cpp - Source file for GMLAN Library

GMLAN is a Controller Area Network Bus used in General Motors vehicles from
roughly 2007-onwards. Its purpose is to allow various Electronic Control Units
(aka ECUs) within a modern vehicle to share information and enact procedures.

An example of this would be communication between the HU (Head unit) and the
DIC (Dashboard Information Cluster), when you adjust the volume up / down, this
is reported to the cluster to be displayed.

It is the function of this library to "crack open" this world to allow anyone
with only as little as a few hours of C++ programming under their belt to get
started in what can sometimes seem a daunting world.

Jason Gaunt, 18th Feb 2013
*/

#include "mbed.h"
#include "GMLAN.h"
#include <vector>

void CANHeader::decode(int _header) {
    if (_header < 0x800)
    {
        // 11-bit header
        arbitrationID = (_header >> 0) & 0x7FF;
    } else {
        // 29-bit header
        priorityID = (_header >> 26) & 0x7;
        arbitrationID = (_header >> 13) & 0x1FFF;
        senderID = (_header >> 0) & 0x1FFF;
    }
}
int CANHeader::encode29bit(void) {
    long int buffer = 0;
    buffer = (buffer << 3) | 0x0; // 3 bit padding
    buffer = (buffer << 3) | priorityID;
    buffer = (buffer << 13) | arbitrationID;
    buffer = (buffer << 13) | senderID;
    return buffer;
}
int CANHeader::encode11bit(void) {
    short int buffer = 0;
    buffer = (buffer << 5) | 0x0; // 5 bit padding
    buffer = (buffer << 11) | arbitrationID;
    return buffer;
}

    
GMLAN_Message::GMLAN_Message(int _priority, int _arbitration, int _sender,
int _b0, int _b1, int _b2, int _b3, int _b4, int _b5, int _b6, int _b7) {
    priority = _priority;
    arbitration = _arbitration;
    sender = _sender;
    if (_b0 != -1) data.push_back(_b0);
    if (_b1 != -1) data.push_back(_b1);
    if (_b2 != -1) data.push_back(_b2);
    if (_b3 != -1) data.push_back(_b3);
    if (_b4 != -1) data.push_back(_b4);
    if (_b5 != -1) data.push_back(_b5);
    if (_b6 != -1) data.push_back(_b6);
    if (_b7 != -1) data.push_back(_b7);
}
CANMessage GMLAN_Message::generate(void) {
    CANHeader hdr;
    hdr.priority(priority);
    hdr.arbitration(arbitration);
    hdr.sender(sender);
    
    char datatochars [data.size()];
    for (int i = 0; i < data.size(); i++) datatochars[i] = data[i];
    
    if (sender > 0x0)
        return CANMessage(hdr.encode29bit(), datatochars, data.size(), CANData, CANExtended);
    else
        return CANMessage(arbitration, datatochars, data.size(), CANData, CANStandard);
}