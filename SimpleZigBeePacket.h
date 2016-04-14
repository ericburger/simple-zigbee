/**
* Library Name: SimpleZigBeePacket
* Library URI: https://github.com/ericburger/simple-zigbee
* Description: Classes for managing ZigBee packets that are sent or received 
* by the connected radio. 
* Version: 0.1.1
* Author(s): Eric Burger
* Author URI: WallflowerOpen.com
* License: GNU General Public License v2.0 or later
* License URI: http://www.gnu.org/licenses/gpl-2.0.html 
*
* Copyright (c) 2013 Eric Burger. All rights reserved.
*
* This file is part of SimpleZigBee, written for XBee S2 Radios.
*
* SimpleZigBee is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 2 of the License, or
* (at your option) any later version.
*
* SimpleZigBee is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with SimpleZigBee.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef SimpleZigBeePacket_h
#define SimpleZigBeePacket_h

#include "Arduino.h"
#include "SimpleZigBeeAddress.h"
// Required for uint8_t type
#include <inttypes.h>

// Start Frame Delimiter
#define START 0x7e
// Escape Byte
#define ESCAPE 0x7d
// Software Flow Control Characters to be Escaped
#define XON 0x11
#define XOFF 0x13

// Error Codes
#define NO_ERROR 0
#define ERROR_REALLOCATING_MEMORY 1
// "Unexpected start of a new packet". Only applies to radios in escaped API Mode.
#define UNEXPECTED_PACKET_START 2
#define PACKET_INCOMPLETE 3
#define MAX_FRAME_LENGTH_EXCEEDED 4
#define FRAME_LENGTH_EXCEEDED 5
#define CHECKSUM_FAILURE 6

// Frame Indexes
#define MSB_INDEX 1
#define LSB_INDEX 2
#define FRAME_TYPE_INDEX 3
#define FRAME_ID_INDEX 4
#define AT_COMMAND_INDEX_START 5
#define AT_COMMAND_INDEX_END 6
#define AT_RESPONSE_COMMAND_STATUS_INDEX 7
#define ZIGBEE_TX_REQUEST_64_ADDR_START 5
#define ZIGBEE_TX_REQUEST_64_ADDR_END 12
#define ZIGBEE_TX_REQUEST_16_ADDR_START 13
#define ZIGBEE_TX_REQUEST_16_ADDR_END 14
#define ZIGBEE_TX_REQUEST_RADIUS 15
#define ZIGBEE_TX_REQUEST_OPTIONS 16

/**
* List of API Frame Types
* (not fully implemented)
*/
#define AT_COMMAND 0x08 // #
#define AT_COMMAND_QUEUED 0x09
#define ZIGBEE_TRANSMIT_REQUEST 0x10 // #
#define ZIGBEE_EXPLICIT_ADDRESSING_COMMAND_FRAME 0x11
#define REMOTE_AT_COMMAND 0x17 // #
#define AT_COMMAND_RESPONSE 0x88 // #
#define MODEM_STATUS 0x8a // #
#define ZIGBEE_TX_STATUS 0x8b // #
#define ZIGBEE_RECIEVED_PACKET 0x90 // #
#define ZIGBEE_EXPLICIT_RX_INDICATOR 0x91
#define ZIGBEE_IO_RX_INDICATOR 0x92
#define NODE_INDENTIFICATION_INDICATOR 0x95
#define REMOTE_AT_COMMAND_RESPONSE 0x97 // #

// AT COMMANDS, 0x08
// REMOTE AT COMMANDS, 0x17 (INCOMPLETE LIST)

// AT COMMAND RESPONSE STATUS, 0x88
// REMOTE AT COMMAND RESPONSE STATUS, 0x97
#define AT_COMMAND_STATUS_OK 0x00
#define AT_COMMAND_STATUS_ERROR 0x01
#define AT_COMMAND_STATUS_INVALID_COMMAND 0x02
#define AT_COMMAND_STATUS_INVALID_PARAMETER 0x03
#define AT_COMMAND_STATUS_TX_FAILURE 0x04

// ZigBee Modem Status, 0x8a (INCOMPLETE LIST)
#define MODEM_STATUS_HARDWARE_RESET 0x00
#define MODEM_STATUS_WATCHDOG_TIMER_RESET 0x01
#define MODEM_STATUS_JOINED_NETWORK 0x02
#define MODEM_STATUS_DISASSOCIATED 0x03
#define MODEM_STATUS_COORDINATOR_STARTED 0x06

// ZigBee Transmit Status, 0x8b (INCOMPLETE LIST)
#define TRANSMIT_STATUS_SUCCESS 0x00
#define TRANSMIT_STATUS_MAC_ACK_FAILURE 0x01
#define TRANSMIT_STATUS_CCA_FAILURE 0x02
#define TRANSMIT_STATUS_INVALID_DEST_ENDPOINT 0x15
#define TRANSMIT_STATUS_NETWORK_ACK_FAILURE 0x21
#define TRANSMIT_STATUS_NOT_JOINED_TO_NETWORK 0x22
#define TRANSMIT_STATUS_SELF_ADDRESSED 0x23
#define TRANSMIT_STATUS_ADDRESS_NOT_FOUND 0x24
#define TRANSMIT_STATUS_ROUTE_NOT_FOUND 0x25
#define TRANSMIT_STATUS_PAYLOAD_TOO_LARGE 0x74



/**
* Class: SimpleZigBeePacket
* @ Since v0.1.0 by Eric Burger, August 2013
* @ Object for incoming and outgoing packets.
*/
class SimpleZigBeePacket {
public:
	// INITIALIZATION METHODS //
	SimpleZigBeePacket();
	SimpleZigBeePacket(int maxFrameLength);
	void init();  
	void reset();
	
	// MEMORY METHODS //
	void expandMemoryArray(int size);
	uint8_t getMemoryData(int index);
	void setMemoryData(int index, uint8_t byte);
	
	// PACKET PRIVATE VARIABLE METHODS //
	int getFrameLength();
	uint8_t getLengthMSB();
	uint8_t getLengthLSB();
	void setFrameLength(int frameLength);
	void setFrameLengthMSB(uint8_t msb);
	void setFrameLengthLSB(uint8_t lsb);
	
	uint8_t getFrameType();
	uint8_t getFrameID();
	uint8_t getChecksum();
	uint8_t calculateChecksum();
	void setChecksum(uint8_t checksum);
	
	// PACKET FRAME METHODS //
	int getMaxFrameLength();
	void setFrameData(int index, uint8_t byte);
	void setFrameData(int startIndex, uint8_t* frameData, int frameDataLength);
	uint8_t getFrameData(int index);
	void getFrameData(int startIndex, uint8_t* arrayPtr, int frameDataLength);

	// ERROR CODE METHODS //
	bool isError();
	int getErrorCode();
	void setErrorCode(int errorCode); 

private:
	// Current length of memory array
	int _memoryArrayLength;
	// Maximum length of packet frame (also defines maximum length of memory array).
	// Note that this does not define the maximum length of the packet, only the max number of bytes 
	// between the LSB and checksum. For receiving packets, be aware that the Arduino serial buffer 
	// is 64 bytes (http://arduino.cc/en/Serial/Available). For this reason, a frame limit around 50 bytes
	// is recommended. Incoming packets that exceed the _maxFrameLength will not be parsed. While it is 
	// possible to receive packets larger than 64 bytes by reading from the serial port as bytes are being
	// received, failure to do so will result in incomplete packets. To support Arduino to Arduino 
	// communication, the length of outgoing packets is also restricted by _maxFrameLength. 
	int _maxFrameLength;
	// Memory array used to store frame data
	uint8_t *_ptrMemoryArray;
	
	// Packet checksum (End of packet)
	uint8_t _checksum;
	// Tracks frame length based on input data
	int _frameLength;
	// Error code
	int _errorCode;  
};


/**
* Class: SimpleIncomingZigBeePacket
* @ Since v0.1.0 by Eric Burger, August 2013
* @ Object for incoming packets.
*/
class SimpleIncomingZigBeePacket : public SimpleZigBeePacket {
public:
	// INITIALIZATION METHODS //
	SimpleIncomingZigBeePacket();
	SimpleIncomingZigBeePacket(int maxFrameLength);
	
	// ZIGBEE RECIEVED (RX) PACKET METHODS //
	// No Frame ID
	SimpleZigBeeAddress getRXAddress();
	SimpleZigBeeAddress64 getRXAddress64(); 
	SimpleZigBeeAddress16 getRXAddress16();  
	uint8_t getRXOptions();
	uint8_t getRXPayloadLength();
	uint8_t getRXPayload(int index);
	
	// ZIGBEE TRANSMIT (TX) STATUS METHODS //
	// For Frame ID, use getFrameID()
	SimpleZigBeeAddress16 getTXStatusAddress16(); 
	uint8_t getTXStatusRetryCount();
	uint8_t getTXStatusDeliveryStatus();
	uint8_t getTXStatusDiscoveryStatus();
	
	// AT COMMAND RESPONSE METHODS //
	// For Frame ID, use getFrameID()
	uint16_t getATResponseCommand();
	uint8_t getATResponseStatus();
	uint8_t getATResponsePayloadLength();
	uint8_t getATResponsePayload();
	uint8_t getATResponsePayload(int index);
	
	// REMOTE AT COMMAND RESPONSE METHODS //
	// For Frame ID, use getFrameID()
	SimpleZigBeeAddress getRemoteATResponseAddress();
	SimpleZigBeeAddress64 getRemoteATResponseAddress64(); 
	SimpleZigBeeAddress16 getRemoteATResponseAddress16();  
	uint16_t getRemoteATResponseCommand();
	uint8_t getRemoteATResponseStatus();
	uint8_t getRemoteATResponsePayloadLength();
	uint8_t getRemoteATResponsePayload();
	uint8_t getRemoteATResponsePayload(int index);
	
	// MODEM STATUS METHODS //
	uint8_t getModemStatus();
};


/**
* Class: SimpleZigBeePacket
* @ Since v0.1.0 by Eric Burger, August 2013
* @ Object for outgoing packets.
*/
class SimpleOutgoingZigBeePacket : public SimpleZigBeePacket {
public:
	// INITIALIZATION METHODS //
	SimpleOutgoingZigBeePacket();
	SimpleOutgoingZigBeePacket(int maxFrameLength); 

	// GENERAL PACKET METHODS //
	void setFrameType(uint8_t frameType);
	void setFrameID(uint8_t id); 
	void setAddress(SimpleZigBeeAddress address);
	void setAddress(uint32_t adr64MSB, uint32_t adr64LSB, uint16_t adr16);  
	void setAddress64(uint32_t adr64MSB, uint32_t adr64LSB);
	void setAddress16(uint16_t adr16);

	// ZIGBEE TRANSMIT (TX) REQUEST METHODS //
	// Use General Packet Methods for Frame Type, Frame ID, and Address
	void setTXRequestBroadcastRadius(uint8_t rad);
	void setTXRequestOption(uint8_t opt);
	void setTXRequestPayload(uint8_t* payload, int payloadSize);

	// AT COMMAND METHODS //
	// Use General Packet Methods for Frame Type and Frame ID
	void setATCommand(uint16_t command); 
	void setATCommandPayload(uint8_t payload); // aka, parameter
	void setATCommandPayload(uint8_t* payload, int payloadSize);

	// REMOTE AT COMMAND METHODS //
	// Use General Packet Methods for Frame Type, Frame ID, and Address
	void setRemoteATCommandOption(uint8_t opt);
	void setRemoteATCommand(uint16_t command); 
	void setRemoteATCommandPayload(uint8_t payload);
	void setRemoteATCommandPayload(uint8_t* payload, int payloadSize);

};


#endif //SimpleZigBeePacket
