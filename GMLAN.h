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

#include "mbed.h"
#include "GMLAN_29bit.h"
#include "GMLAN_11bit.h"
#include <vector>

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

        // Function to encode stored values as 11-bit header and return header packet as int
        int encode11bit(void);
};

class GMLAN_Message {
    /*
    Wrapper for CANMessage that automatically parses settings
    */
    private:
        vector<char> data;
        int priority, arbitration, sender;
    
    public:
        // Main function
        GMLAN_Message(int _priority = -1, int _arbitration = -1, int _sender = -1,
        int _b0 = -1, int _b1 = -1, int _b2 = -1, int _b3 = -1, int _b4 = -1, int _b5 = -1, int _b6 = -1, int _b7 = -1);
    
        // Return result
        CANMessage generate(void);
};

class GMLAN_11Bit_Request {
    /*
    Class to allow easier handling of sending and receiving 11-bit messages
    */
    private:
        vector<char> request_data, response_data;
        int id, request_state;
        int tx_frame_counter, tx_bytes;
        int rx_frame_counter, rx_bytes;
        bool await_response;
        char frame_padding [8];
    
    public:
        // (Main function) Create message and send it
        GMLAN_11Bit_Request(int _id, vector<char> _request, bool _await_response = true);
        
        // Process each frame to transmit and flow control frame if needed
        CANMessage getNextFrame(void);
        CANMessage getFlowControl(void);
        // Process each received frame
        void processFrame(CANMessage msg);
        
        // Handle starting and flow control
        void start(void) { request_state = GMLAN_STATE_SEND_DATA; }
        void continueFlow(void) { request_state = GMLAN_STATE_SEND_DATA; }
        
        // Return request_state to confirm status
        int getState(void) { return request_state; }
        // Return ID
        int getID(void) { return id; }
        // Return rx_bytes
        int getRXcount(void) { return rx_bytes; }
        // Return response
        vector<char> getResponse(void) { return response_data; }
};

#endif