/*
GMLAN.h - Header file for GMLAN Library

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

#include "GMLAN_29bit.h"
// #include "GMLAN_11bit.h"

#ifndef GMLAN_H
#define GMLAN_H

/* Baud rates of various services */
#define GMLAN_BAUD_LS_NORMAL 33333
#define GMLAN_BAUD_LS_FAST 83333
#define GMLAN_BAUD_MS 95200
#define GMLAN_BAUD_HS 500000

class CANHeader {
    /*
    CANHeader was designed solely for 29-bit frames but supports 11-bit too by just setting the ArbID
    
    Example 29-bit header packet from Steering Wheel Switches:
    
    Hexadecimal:    0x10     0x0D     0x00     0x60
    Binary:       00010000 00001101 00000000 01100000
    Priority:        ---
    Arbitration:        -- -------- ---
    Sending ECU:                       ----- --------
    
    Example 11-bit header packet from Head Unit:
    
    Hexadecimal:    0x02     0x44
    Binary:       00000010 01000100
    Identifier:        --- --------
    
    */

    private:
        int priorityID, arbitrationID, senderID;
    
    public:
        // Main function
        CANHeader() { }
        
        // Methods for getting / setting priority, both integers
        int priority(void) { return priorityID; }
        void priority(int _priority) { priorityID = _priority; }
        
        // Method for getting / setting arbitration id aka arbid, both integers
        int arbitration(void) { return arbitrationID; }
        void arbitration(int _arbitration) { arbitrationID = _arbitration; }
        
        // Method for getting / setting sender id, both integers
        int sender(void) { return senderID; }
        void sender(int _sender) { senderID = _sender; }
    
        // Function to decode either an 11-bit or 29-bit header packet and store values in respective variables
        void decode(int _header);
        
        // Function to encode stored values as 29-bit header and return header packet as int
        int encode29bit(void);
};

#endif