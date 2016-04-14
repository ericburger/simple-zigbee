/**
* Library Name: SimpleZigBeeRadio
* Library URI: https://github.com/ericburger/simple-zigbee
* Description: Library for sending and receiving packets through the Arduino Serial port
* using the ZigBee protocol. Intended for use with XBee S2 radios. These radios can
* be connected directly to the Arduino's TX and RX pins or to any two digital pins 
* using the SoftwareSerial Arduino library. For more details about this library and 
* examples of its use, please visit https://github.com/ericburger/simple-zigbee.
* Version: 0.1.1
* Author(s): Eric Burger
* Author URI: WallflowerOpen.com
* License: GNU General Public License v2.0 or later
* License URI: http://www.gnu.org/licenses/gpl-2.0.html 
*
* Copyright (c) 2013 Eric Burger. All rights reserved.
*
* This file is part of SimpleZigBee.
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

/**
* Notes for modifying Arduino files:
* 1. To encourage the use of these libraries for educational purposes, please provide detailed explanations
*    throughout the code.
* 2. When adding a class or method, please provide a description of the class or method and information 
*    concerning when the addition was made, who made it, and the upcoming release in which the addition will appear. 
*    Descriptions of CLASSES should be placed in the .h file and of METHODS in the .cpp file. 
*    
*    Example for classes:
*    Class: SimpleZigBeeAddress()
*    @ Since v0.1.0 by Eric Burger, September 2013
*    @ Stores the 64-bit and 16-bit addresses of an ZigBee packet 
*
*    Example for methods:    
*    Method: setAddress64(uint32_t adr64MSB, uint32_t adr64LSB)
*    @ Since v0.1.0 by Eric Burger, September 2013
*    @ Sets 64-bit address
*    @ param uint32_t adr64MSB: 32-bit integer representing the first half of the 64-bit address
*    @ param uint32_t adr64LSB: 32-bit integer representing the second half of the 64-bit address
*
* 3. When modifying an existing class or method, please append a description of the changes
*    
*    Example for classes:
*    Class: SimpleZigBeeAddress()
*    @ Since v0.1.0 by Eric Burger, September 2013
*    @ Class storing the 64-bit and 16-bit addresses of an ZigBee packet
*    @ Last Modified v0.1.1 by Joe Smith, December 2013
*    @ Changlog for v0.1.1:
*       - Added variable "foo" for storing information
*       - Added method setFoo
*
*    Example for methods:    
*    Method: setAddress64(uint32_t adr64MSB, uint32_t adr64LSB)
*    @ Since v0.1.0 by Eric Burger, September 2013
*    @ Sets 64-bit address
*    @ param uint32_t adr64MSB: 32-bit integer representing the first half of the 64-bit address
*    @ param uint32_t adr64LSB: 32-bit integer representing the second half of the 64-bit address
*    @ Last Modified v0.1.1 by Joe Smith, December 2013
*    @ Changlog for v0.1.1:
*       - Added error check
* 
*/

#ifndef SimpleZigBeeRadio_h
#define SimpleZigBeeRadio_h

#include "Arduino.h"
// Requires SimpleZigBeePacket classes
#include "SimpleZigBeePacket.h"
// Required for uint8_t type
#include <inttypes.h>

/**
* Class: SimpleZigBeeRadio
* @ Since v0.1.0 by Eric Burger, August 2013
* @ Object for reading and sending ZigBee packets via serial port. The class
*   assumes the XBee radio is in Escaped API Mode (ATAP=2) by default. The class includes
*   an alternative constructor for radios that are not in Escaped API Mode, though this is
*   not recommended due to the inability to identify incoming packets that are incomplete. 
*   In other words, in Escaped API Mode, incoming packets can only contain the start byte,
*   0x7E, at the start of a packet since the byte is "escaped" at all other positions.
*/
class SimpleZigBeeRadio {
public:
	// INITIALIZATION METHODS //
	SimpleZigBeeRadio();
	SimpleZigBeeRadio(bool escaped_mode);
	void reset();
	void resetIncoming();
	void resetOutgoing();
	void setSerial(HardwareSerial & serial);
	void setSerial(Stream & serial);
	
	// PACKET METHODS //
	SimpleIncomingZigBeePacket & getIncomingPacketObject();
	SimpleOutgoingZigBeePacket & getOutgoingPacketObject(); 

	// INCOMING PACKET METHODS //
	bool available();
	void read();
	bool isEscaping();  
	void setEscaping(bool escape);  
	bool isComplete();  
	void setComplete(bool complete);
	
	uint8_t getIncomingFrameType();
	uint8_t getIncomingFrameID();
	uint8_t getIncomingFrameData(int index);
	void getIncomingFrameData(int startIndex, uint8_t* arrayPtr, int frameDataLength);
	
	// ZIGBEE RECIEVED (RX) PACKET METHODS //
	bool isRX();
	SimpleZigBeeAddress getRXAddress();
	SimpleZigBeeAddress64 getRXAddress64(); 
	SimpleZigBeeAddress16 getRXAddress16();  
	uint8_t getRXOptions();
	uint8_t getRXPayloadLength();
	uint8_t getRXPayload(int index);
	
	// ZIGBEE TRANSMIT (TX) STATUS METHODS //
	bool isTXStatus();
	// For Frame ID, use getIncomingFrameID()
	SimpleZigBeeAddress16 getTXStatusAddress16(); 
	uint8_t getTXStatusRetryCount();
	uint8_t getTXStatusDeliveryStatus();
	uint8_t getTXStatusDiscoveryStatus();
	
	// AT COMMAND RESPONSE METHODS //
	bool isATResponse();
	// For Frame ID, use getIncomingFrameID()
	uint16_t getATResponseCommand();
	uint8_t getATResponseStatus();
	uint8_t getATResponsePayloadLength();
	uint8_t getATResponsePayload();
	uint8_t getATResponsePayload(int index);
	
	// REMOTE AT COMMAND RESPONSE METHODS //
	bool isRemoteATResponse();
	// For Frame ID, use getIncomingFrameID()
	SimpleZigBeeAddress getRemoteATResponseAddress();
	SimpleZigBeeAddress64 getRemoteATResponseAddress64();
	SimpleZigBeeAddress16 getRemoteATResponseAddress16();  
	uint16_t getRemoteATResponseCommand();
	uint8_t getRemoteATResponseStatus();
	uint8_t getRemoteATResponsePayloadLength();
	uint8_t getRemoteATResponsePayload();
	uint8_t getRemoteATResponsePayload(int index);
	
	// MODEM STATUS METHODS //
	bool isModemStatus();
	uint8_t getModemStatus();
	
	// OUTGOING PACKET METHODS //
	void setOutgoingFrameType(uint8_t frameType);
	void setOutgoingFrameID(uint8_t id);
	uint8_t getLastFrameID();
	void saveLastFrameID(uint8_t frameID);
	void setNextFrameID();
	void setAcknowledgement(bool ack);
	void setOutgoingFrameData(int index, uint8_t byte);
	void setOutgoingFrameData(int startIndex, uint8_t* frameData, int frameDataLength);
	
	void setOutgoingAddress(SimpleZigBeeAddress address);
	void setOutgoingAddress(uint32_t adr64MSB, uint32_t adr64LSB, uint16_t adr16);  
	void setOutgoingAddress64(uint32_t adr64MSB, uint32_t adr64LSB);
	void setOutgoingAddress16(uint16_t adr16);
	
	void send();
	void send(SimpleZigBeePacket & packet);
	void sendPacket(SimpleZigBeePacket & packet);
	void writeByte(uint8_t byte);
	void write(uint8_t byte);
	void flush();
	
	// ZIGBEE TRANSMIT (TX) REQUEST METHODS //
	// Use General Packet Methods for Frame Type, Frame ID, and Address
	void setTXRequestBroadcastRadius(uint8_t rad);
	void setTXRequestOption(uint8_t opt);
	void setTXRequestPayload(uint8_t* payload, int payloadSize);
	
	void prepareTXRequest(uint32_t adr64MSB, uint32_t adr64LSB, uint16_t adr16, uint8_t* payload, int payloadSize);
	void prepareTXRequest(SimpleZigBeeAddress address, uint8_t* payload, int payloadSize);
	void prepareTXRequestBroadcast(uint8_t* payload, int payloadSize);
	void prepareTXRequestToCoordinator(uint8_t* payload, int payloadSize);
	
	// AT COMMAND METHODS //
	// Use General Packet Methods for Frame Type, Frame ID, and Address
	void setATCommand(uint16_t command); 
	void setATCommandPayload(uint8_t payload); // aka, parameter
	void setATCommandPayload(uint8_t* payload, int payloadSize);
	
	void prepareATCommand(uint16_t command);
	void prepareATCommand(uint16_t command, uint8_t payload);
	void prepareATCommand(uint16_t command, uint8_t* payload, int payloadSize);
	
	// REMOTE AT COMMAND METHODS //
	// Use General Packet Methods for Frame Type, Frame ID, and Address
	void setRemoteATCommandOption(uint8_t opt);
	void setRemoteATCommand(uint16_t command);
	void setRemoteATCommandPayload(uint8_t payload);
	void setRemoteATCommandPayload(uint8_t* payload, int payloadSize);

	void prepareRemoteATCommand(uint32_t adr64MSB, uint32_t adr64LSB, uint16_t adr16, uint16_t command);
	void prepareRemoteATCommand(uint32_t adr64MSB, uint32_t adr64LSB, uint16_t adr16, uint16_t command, uint8_t payload);
	void prepareRemoteATCommand(uint32_t adr64MSB, uint32_t adr64LSB, uint16_t adr16, uint16_t command, uint8_t* payload, int payloadSize);
	
	void prepareRemoteATCommand(SimpleZigBeeAddress address, uint16_t command);
	void prepareRemoteATCommand(SimpleZigBeeAddress address, uint16_t command, uint8_t payload);
	void prepareRemoteATCommand(SimpleZigBeeAddress address, uint16_t command, uint8_t* payload, int payloadSize);

	// ERRORS //
	
	
private:
	Stream * _serial;
	// Boolean indicating whether or not XBee radio is in escaped API Mode (ATAP=2) 
	bool _escaped_mode;
	// Boolean indicating whether or not serial port is SoftwareSerial 
	bool _is_software_serial;

	// Object for storing incoming packet
	SimpleIncomingZigBeePacket _incoming_packet;
	// Most recent byte received from incoming packet
	uint8_t _in_byte;
	// Current index of incoming packet
	int _in_index;
	// Current checksum of incoming packet
	uint8_t _in_checksum;
	// Escape the next byte of incoming packet
	bool _in_escaping;
	// Boolean for tracking if incoming packet is completely received
	bool _in_complete;
	
	// Object for preparing outgoing packet
	SimpleOutgoingZigBeePacket _outgoing_packet;
	// Boolean specifying if outgoing packets should require acknowledgement.
	bool _out_acknowledgement;
	// Frame ID of last outgoing packet
	uint8_t _out_frame_id;

};

#endif //SimpleZigBeeRadio_h
