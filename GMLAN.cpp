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

#include "GMLAN.h"

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