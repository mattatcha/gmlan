/*
GMLAN_11bit.h - 11-Bit specific header file for GMLAN Library

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

#ifndef GMLAN_11BIT_H
#define GMLAN_11BIT_H

// CAN IDs
#define GMLAN_INITIAL_WAKE_UP_REQUEST   0x100
#define GMLAN_REQUEST_TO_ALL_NODES  0x101
#define GMLAN_DIAGNOSTIC_REQUEST    0x102
#define GMLAN_TO_RESERVED_REQUEST   0x240
#define GMLAN_TO_BCM    0x241
#define GMLAN_TO_TDM    0x242
#define GMLAN_TO_EBCM   0x243
#define GMLAN_TO_EHU    0x244
#define GMLAN_TO_SIC    0x246
#define GMLAN_TO_SDC    0x247
#define GMLAN_TO_IPC    0x24C
#define GMLAN_TO_HVAC   0x251
#define GMLAN_TO_RFA    0x258
#define GMLAN_SF_FROM_RESERVED_RESPONSE 0x540
#define GMLAN_MF_FROM_RESERVED_RESPONSE 0x640
#define GMLAN_MF_FROM_BCM   0x641
#define GMLAN_MF_FROM_TDM   0x642
#define GMLAN_MF_FROM_EBCM  0x643
#define GMLAN_MF_FROM_EHU   0x644
#define GMLAN_MF_FROM_SIC   0x646
#define GMLAN_MF_FROM_SDC   0x647
#define GMLAN_MF_FROM_IPC   0x64C
#define GMLAN_MF_FROM_HVAC  0x651
#define GMLAN_MF_FROM_RFA   0x658
#define GMLAN_EXTERNAL_OBD_TEST_EQUIPMENT_TO_NON_SPECIFIC_OBD_COMPLIANT_ECUS    0x7DF
#define GMLAN_EXTERNAL_OBD_TEST_EQUIPMENT_TO_ECM    0x7E0
#define GMLAN_EXTERNAL_OBD_TEST_EQUIPMENT_TO_SPECIFIC_OBD_COMPLIANT_ECU 0x7E1
#define GMLAN_ECM_TO_EXTERNAL_OBD_TEST_EQUIPMENT    0x7E8
#define GMLAN_SPECIFIC_OBD_COMPLIANT_ECU_TO_EXTERNAL_OBD_TEST_EQUIPMENT 0x7E9

// PCI byte
#define GMLAN_PCI_UNSEGMENTED       0x0
#define GMLAN_PCI_SEGMENTED         0x1
#define GMLAN_PCI_ADDITIONAL        0x2
#define GMLAN_PCI_FLOW_CONTROL      0x3

// Service ID byte
#define GMLAN_SID_CLEAR_DTC         0x4
#define GMLAN_SID_START_DIAG        0x10
#define GMLAN_SID_REQ_FAIL_RECS     0x12
#define GMLAN_SID_REQ_DID           0x1A
#define GMLAN_SID_RES_NORM_OP       0x20
#define GMLAN_SID_REQ_PID           0x22
#define GMLAN_SID_READ_ADDR         0x23
#define GMLAN_SID_REQ_SEC_ACCESS    0x27
#define GMLAN_SID_DSBL_NORM_OP      0x28
#define GMLAN_SID_DEF_DPID_MSG      0x2C
#define GMLAN_SID_DEF_PID_BY_ADDR   0x2D
#define GMLAN_SID_DL_REQ            0x34
#define GMLAN_SID_DATA_TRANS        0x36
#define GMLAN_SID_WRITE_DID         0x3B
#define GMLAN_SID_TESTER_PRESENT    0x3E
#define GMLAN_SID_ERROR             0x7F
#define GMLAN_SID_REQ_PROG_STATE    0xA2
#define GMLAN_SID_PROG_MODE         0xA5
#define GMLAN_SID_READ_DTC          0xA9
#define GMLAN_SID_REQ_DPID          0xAA
#define GMLAN_SID_REQ_CONTROL       0xAE

// States of request
#define GMLAN_STATE_READY_TO_SEND   0x0
#define GMLAN_STATE_SEND_DATA       0x1
#define GMLAN_STATE_AWAITING_FC     0x2
#define GMLAN_STATE_AWAITING_REPLY  0x3
#define GMLAN_STATE_SEND_FC         0x4
#define GMLAN_STATE_COMPLETED       0x5
#define GMLAN_STATE_ERROR           0x6

#endif