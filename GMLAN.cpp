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

GMLAN_11Bit_Request::GMLAN_11Bit_Request(int _id, vector<char> _request, bool _await_response, bool _handle_flowcontrol) {
    id = _id;
    request_data = _request;
    await_response = _await_response;
    handle_flowcontrol = _handle_flowcontrol;
    tx_bytes = rx_bytes = 0;
    tx_frame_counter = rx_frame_counter = 1;
    const char _fp [8] = {0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA,0xAA};
    memcpy(frame_padding, _fp, 8);
    request_state = GMLAN_STATE_READY_TO_SEND;
}
CANMessage GMLAN_11Bit_Request::getNextFrame(void) {
    char datatochars [8];
    memcpy(datatochars, frame_padding, 8);
    
    if (handle_flowcontrol == true) {
        // Only run this section if we need flow control
        if (request_data.size() < 8) {
            // Unsegmented frame
            datatochars[0] = (GMLAN_PCI_UNSEGMENTED << 4) | (request_data.size() & 0xF);
            for (int i = 0; i < request_data.size(); i++) {
                datatochars[i+1] = request_data[i];
                tx_bytes++;
            }
            request_state = GMLAN_STATE_AWAITING_REPLY;
        } else if (tx_bytes == 0) {
            // First segmented frame
            datatochars[0] = (GMLAN_PCI_SEGMENTED << 4) | ((request_data.size() >> 8) & 0xF);
            datatochars[1] = request_data.size() & 0xFF;
            for (int i = 0; i < 6; i++) {
                datatochars[i+2] = request_data[i];
                tx_bytes++;
            }
            request_state = GMLAN_STATE_AWAITING_FC;
        } else if (tx_bytes <= request_data.size()) {
            // Additional segmented frame with data left to transmit
            datatochars[0] = (GMLAN_PCI_ADDITIONAL << 4) | (tx_frame_counter & 0xF);
            int old_tx_bytes = tx_bytes;
            for (int i = old_tx_bytes; i < old_tx_bytes + 7; i++) {
                if (i >= request_data.size()) break;
                datatochars[(i+1)-old_tx_bytes] = request_data[i];
                tx_bytes++;
            }
            tx_frame_counter++;
            if (tx_frame_counter > 0xF) tx_frame_counter = 0x0;
        }
        if (tx_bytes >= request_data.size()) {
            if (await_response == true) request_state = GMLAN_STATE_AWAITING_REPLY;
            else request_state = GMLAN_STATE_COMPLETED;
        }
    } else {
        // No flow control required, build the frames without parsing but make sure we don't overshoot 8 bytes
        for (int i = 0; i < request_data.size(); i++) {
            if (i < 8) {
                datatochars[i] = request_data[i];
                tx_bytes++;
            }
            else break;
        }
        if (await_response == true) request_state = GMLAN_STATE_AWAITING_REPLY;
        else request_state = GMLAN_STATE_COMPLETED;
    }
    
    return CANMessage(id, datatochars, 8, CANData, CANStandard);
}
CANMessage GMLAN_11Bit_Request::getFlowControl(void) {
    request_state = GMLAN_STATE_AWAITING_REPLY;
    GMLAN_Message buffer = GMLAN_Message(0x0, id, 0x0, 0x30, 0x0, 0x0);
    return buffer.generate();
}
void GMLAN_11Bit_Request::processFrame(CANMessage msg) {
    if (((msg.id & 0xFF) == (id & 0xFF)) && (request_state == GMLAN_STATE_AWAITING_REPLY)) {
        // Only handle requests we've instigated
        char datatochars [8];
        memcpy(datatochars, msg.data, 8);
        
        if (((datatochars[0] >> 4) & 0xF) == GMLAN_PCI_UNSEGMENTED) {
            // Unsegmented frame
            rx_bytes = (datatochars[0] & 0xF);
            if (datatochars[1] == GMLAN_SID_ERROR) {
                // Error frame
                if ((rx_bytes == 3) && (datatochars[3] == 0x78)) return; // "Still processing request" message, ignore this one
                request_state = GMLAN_STATE_ERROR;
            } else request_state = GMLAN_STATE_COMPLETED;
            for (int i = 1; i < (rx_bytes+1); i++) response_data.push_back(datatochars[i]);
        } else if (((datatochars[0] >> 4) & 0xF) == GMLAN_PCI_SEGMENTED) {
            // First segmented frame
            rx_bytes = (datatochars[0] & 0xF);
            rx_bytes = (rx_bytes << 8) | datatochars[1];
            for (int i = 2; i < 8; i++) {
                if ((i - 2) >= rx_bytes) {
                    // Safety net for incorrectly formatted packets
                    request_state = GMLAN_STATE_COMPLETED;
                    return;
                }
                response_data.push_back(datatochars[i]);
            }
            request_state = GMLAN_STATE_SEND_FC;
        } else if (((datatochars[0] >> 4) & 0xF) == GMLAN_PCI_ADDITIONAL) {
            // Additional segmented frame
            // TODO check for frame order
            for (int i = 1; i < 8; i++) {
                if (response_data.size() >= rx_bytes) {
                    request_state = GMLAN_STATE_COMPLETED;
                    return;
                }
                response_data.push_back(datatochars[i]);
            }
            if (response_data.size() >= rx_bytes) {
                request_state = GMLAN_STATE_COMPLETED;
                return;
            }
        } else if (((datatochars[0] >> 4) & 0xF) == GMLAN_PCI_FLOW_CONTROL) {
            // Flow control frame
            request_state = GMLAN_STATE_SEND_DATA;
        }
    }
}