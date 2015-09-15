/**
* Copyright (c) 2013 Eric Burger. All rights reserved.
*/

#include "SimpleZigBeeRadio.h"
// For Stream class (serial port object)
#include "HardwareSerial.h"

/*//////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
										SimpleZigBeeRadio Class
////////////////////////////////////////////////////////////////////////
/*//////////////////////////////////////////////////////////////////////

/*//////////////////////////////////////////////////////////////////////
									INITIALIZATION METHODS
/*//////////////////////////////////////////////////////////////////////

/**
*  Constructor: SimpleZigBeeRadio()
*  @ Since v0.1.0 by Eric Burger, September 2013
*  @ Default constructor that creates packet object for incoming and outgoing packets.
*  @ By default, library assumes XBee is in Escaped API Mode (ATAP=2).
*/
SimpleZigBeeRadio::SimpleZigBeeRadio() {
	_incoming_packet = SimpleIncomingZigBeePacket();
	_outgoing_packet = SimpleOutgoingZigBeePacket();
	_escaped_mode = true;
	reset();
}


/**
*  Constructor: SimpleZigBeeRadio(bool escaped_mode)
*  @ Since v0.1.0 by Eric Burger, September 2013
*  @ Constructor with parameter. Creates packet object for incoming and outgoing packets.
*  @ param bool escaped_mode: FALSE if API Mode (ATAP=1) and TRUE if Escaped API Mode (ATAP=2).
*/
SimpleZigBeeRadio::SimpleZigBeeRadio(bool escaped_mode) {
	_incoming_packet = SimpleIncomingZigBeePacket();
	_outgoing_packet = SimpleOutgoingZigBeePacket();
	_escaped_mode = escaped_mode;
	reset();
}

/**
*  Method: reset()
*  @ Since v0.1.0 by Eric Burger, September 2013
*  @ Resets the radio's private parameters.
*/
void SimpleZigBeeRadio::reset(){
	resetIncoming();
	resetOutgoing();
	_out_frame_id = 0;
	_out_acknowledgement = false;
}

/**
*  Method: resetIncoming()
*  @ Since v0.1.0 by Eric Burger, July 2014
*  @ Resets incoming packet and the radio's private parameters.
*/
void SimpleZigBeeRadio::resetIncoming(){
	_incoming_packet.reset();
	_in_complete = false;
	_in_escaping = false;
	_in_checksum = 0;
	_in_index = 0;
}

/**
*  Method: resetOutgoing()
*  @ Since v0.1.0 by Eric Burger, July 2014
*  @ Resets outgoing packet and the radio's private parameters.
*/
void SimpleZigBeeRadio::resetOutgoing(){
	_outgoing_packet.reset();
}

/**
*  Method: setSerial(HardwareSerial & serial)
*  @ Since v0.1.0 by Eric Burger, September 2013
*  @ Set the pointer to the HardwareSerial object communicating with the XBee radio
*  @ param HardwareSerial& serial: Pointer to HardwareSerial object.
*/
void SimpleZigBeeRadio::setSerial(HardwareSerial & serial){
	_serial = &serial;
	_is_software_serial = false;
}

/**
*  Method: setSerial(Stream & serial)
*  @ Since v0.1.0 by Eric Burger, September 2013
*  @ Set the pointer to the Stream object communicating with the XBee radio. 
*  @ Compatible with SoftwareSerial but not with flush() method.
*  @ param Stream& serial: Pointer to Stream object.
*/
void SimpleZigBeeRadio::setSerial(Stream & serial){
	_serial = &serial;
	_is_software_serial = true;
}

/*//////////////////////////////////////////////////////////////////////
									PACKET METHODS
/*//////////////////////////////////////////////////////////////////////

/**
*  Method: getIncomingPacketObject()
*  @ Since v0.1.0 by Eric Burger, September 2013
*  @ Returns the packet object for storing incoming packet
*/
SimpleIncomingZigBeePacket& SimpleZigBeeRadio::getIncomingPacketObject() {
	return _incoming_packet;
}

/**
*  Method: getOutgoingPacketObject()
*  @ Since v0.1.0 by Eric Burger, September 2013
*  @ Returns the packet object for storing outgoing packet
*/
SimpleOutgoingZigBeePacket& SimpleZigBeeRadio::getOutgoingPacketObject() {
	return _outgoing_packet;
}

/*//////////////////////////////////////////////////////////////////////
									INCOMING PACKET METHODS
/*//////////////////////////////////////////////////////////////////////

/**
*  Method: available()
*  @ Since v0.1.0 by Eric Burger, January 2014
*  @ Checks if bytes received by serial port and returns boolean
*/
bool SimpleZigBeeRadio::available(){
	if( _serial->available() > 0 ){
		return true;
	}
	return false;
}

/**
*  Method: read()
*  @ Since v0.1.0 by Eric Burger, September 2013
*  @ Reads incoming ZigBee packet from serial port and stores in packet object
*/
void SimpleZigBeeRadio::read(){
	// Don't do anything if _serial not available
	if( _serial->available() ){
		// Before receiving a new packet from the serial buffer, reset incoming packet object, if necessary
		if( _incoming_packet.isError() || isComplete() ){
			// Store error code before resetting
			int err = _incoming_packet.getErrorCode();
			// If the previous packet was completely received or contained an error, reset.
			resetIncoming();
			// If error was caused by UNEXPECTED_PACKET_START, set current index to 1 since the START byte has already been read from the serial buffer.
			if( UNEXPECTED_PACKET_START == err ){
				_in_index = 1;
			}
		}
		
		// Otherwise, if the previous packet was incomplete but free of errors, try and receive the rest of the packet.
		
		// Read from serial port, while bytes are available
		while( _serial->available() ){
			_in_byte = _serial->read();
			
			// First, check if XBee is in Escaped API Mode (ATAP=2)
			if ( true == _escaped_mode ) {
				// Next, check if a (non-escaped) start frame delimiter is found anywhere other than the start of the packet.
				if ( START == _in_byte && _in_index > 0  ) {
					// AN ERROR OCCURED
					// If found, it means that a new packet has started before the previous packet was completely received.
					// This may indicate a noisy environment or that the buffer overflowed when the previous packet was being 
					// received by the XBee.
					// Set error message and return. If read() is called again, packet object will be reset but current index will be set to 1.
					_incoming_packet.setErrorCode( UNEXPECTED_PACKET_START );
					return;
				}
				
				// If byte has been flagged as escaped, "un-escape" the byte using the "Excusive bitwise OR" operator (^) 
				if ( true == isEscaping() ) {
					_in_byte = 0x20 ^ _in_byte;
					setEscaping(false);
				}
				
				// Check if current byte is escape byte. If true, this indicates that the next byte in the packet has been escaped.
				if ( _in_byte == ESCAPE && _in_index > 0 ) {
					// Try and read the next byte from the serial buffer, otherwise, note that next byte is escaped by 
					// setting _escaping to true and continuing loop.
					if ( _serial->available() ) {
						_in_byte = _serial->read();
						// "Un-escape" the byte using the "Excusive bitwise OR" operator (^) 
						_in_byte = 0x20 ^ _in_byte;
					} else {
						setEscaping(true);
						// Jump to the start of the while loop and re-check the condition (maybe buffer will be ready).
						continue; 
					}
				}

			}
			// Note that if the XBee is not in Escaped API Mode (ATAP=2) and the start delimiter is found in a position other than the beginning of a packet,
			// it is not treated as the start of a packet. In this case, it is treated as just another byte. This can lead to trouble when radios
			// are placed in a noisy environment.
			// For reference: http://www.digi.com/support/kbase/kbaseresultdetl?id=2199
			
			// All bytes starting with the Frame Type are included in the checksum
			if ( _in_index >= FRAME_TYPE_INDEX ) {
				_in_checksum += _in_byte;
			}
			
			// Start storing incoming information in _incoming_packet object
			if ( 0 == _in_index ){
				if ( START == _in_byte ) {
					// There is nothing to do with the start byte, so move unto the next position.
					_in_index++;
				}else{
					// AN ERROR OCCURED
					// If START byte was not found, set error code indicating that packet was not read correctly
					_incoming_packet.setErrorCode( PACKET_INCOMPLETE );
				}
			}else if( 1 == _in_index ){
				// Store "Most Significant Byte" of packet's length
				_incoming_packet.setFrameLengthMSB(_in_byte);
				_in_index++;
			}else if( 2 == _in_index ){
				// Store "Least Significant Byte" of packet's length
				_incoming_packet.setFrameLengthLSB(_in_byte);
				_in_index++;
			}else{
				// For the remaining bytes in the packet, check that the maximum frame length has not been exceeded...
				if ( _in_index > _incoming_packet.getMaxFrameLength() ) {
					// AN ERROR OCCURED
					_incoming_packet.setErrorCode( MAX_FRAME_LENGTH_EXCEEDED );
					return;
				}
				// ...Then check if the end of the packet has been reached (which should be the checksum).
				// Note: When setFrameLengthLSB() was last called, the frame length of the incoming packet was updated
				// based on the MSB and LSB. Therefore, the frame length should not have increased due to calls to
				// setOutgoingFrameData(). 
				// This length does not include the start byte, MSB, LSB, or checksum byte. Therefore, the length
				// plus 3 should be the position of the checksum (i.e. frame length plus 4 minus 1).
				if ( (_incoming_packet.getFrameLength() + 3) == _in_index ) {
					// Verify checksum using the bitwise AND operator (&)
					if ( 0xff == (_in_checksum & 0xff) ) {
						// Success!!! The packet was completely received and the checksum verified.
						setComplete(true);
						_incoming_packet.setChecksum(_in_checksum);
						_incoming_packet.setErrorCode( NO_ERROR );
					}else{
						// Failure!!! The packet is not usable because the checksum failed.
						// AN ERROR OCCURED 
						_incoming_packet.setErrorCode( CHECKSUM_FAILURE );
						return;
					}
					
					
					// TODO: Process packet?
					
					
					return;
				}
				
				// Otherwise, beginning with Packet index 3 (Frame index 0), store byte is FrameData array.
				// Frame index 0 should contain the Frame Type
				_incoming_packet.setFrameData( (_in_index - FRAME_TYPE_INDEX) , _in_byte);
				_in_index++;
			}    
		}
	}	
}

/**
*  Method: isEscaping()
*  @ Since v0.1.0 by Eric Burger, January 2014
*  @ Checks if next byte of incoming packet should be escaped
*/
bool SimpleZigBeeRadio::isEscaping(){
	return _in_escaping;
}

/**
*  Method: setEscaping(bool escape)
*  @ Since v0.1.0 by Eric Burger, August 2013
*  @ Sets boolean indicating if next byte of incoming packet should be escaped
*  @ param bool escape: Boolean value to store
*/
void SimpleZigBeeRadio::setEscaping(bool escape){
	_in_escaping = escape;
}

/**
*  Method: isComplete()
*  @ Since v0.1.0 by Eric Burger, January 2014
*  @ Checks if incoming packet is complete and returns boolean
*/
bool SimpleZigBeeRadio::isComplete(){
	return _in_complete;
}

/**
*  Method: setComplete(bool complete)
*  @ Since v0.1.0 by Eric Burger, August 2013
*  @ Sets boolean indicating if incoming packet is complete
*  @ param bool complete: Boolean value to store
*/
void SimpleZigBeeRadio::setComplete(bool complete){
	_in_complete = complete;
}

/**
*  Method: getIncomingFrameType()
*  @ Since v0.1.0 by Eric Burger, September 2013
*  @ Returns frame type for incoming packets
*/
uint8_t SimpleZigBeeRadio::getIncomingFrameType(){
	return _incoming_packet.getFrameType();
}

/**
*  Method: getIncomingFrameID()
*  @ Since v0.1.0 by Eric Burger, September 2013
*  @ Returns frame ID for incoming packets
*/
uint8_t SimpleZigBeeRadio::getIncomingFrameID(){
	return _incoming_packet.getFrameID();
}

/**
*  Method: getIncomingFrameData(int index)
*  @ Since v0.1.0 by Eric Burger, September 2013
*  @ Returns the frame byte of the incoming packet at the specified index
*  @ param int index: Index of frame data
*/
uint8_t SimpleZigBeeRadio::getIncomingFrameData(int index){
	return _incoming_packet.getFrameData(index);
}

/**
*  Method: getIncomingFrameData(int startIndex, uint8_t* arrayPtr, int frameDataLength)
*  @ Since v0.1.0 by Eric Burger, September 2013
*  @ Places an array of bytes from the memory array of the incoming packet in the
*    provided array pointer, starting from specified start index.
*  @ param int startIndex: Index of frame data at which to start
*  @ param uint8_t* arrayPtr: Array for storing bytes
*  @ param int frameDataLength: Number of byte to return
*/
void SimpleZigBeeRadio::getIncomingFrameData(int startIndex, uint8_t* arrayPtr, int frameDataLength){
	return _incoming_packet.getFrameData(startIndex,arrayPtr,frameDataLength);
}

/*//////////////////////////////////////////////////////////////////////
									ZIGBEE RECIEVED PACKET METHODS
/*//////////////////////////////////////////////////////////////////////

/**
*  Method: isRX()
*  @ Since v0.1.0 by Eric Burger, January 2014
*  @ Checks if received packet is a RX packet and returns boolean
*/
bool SimpleZigBeeRadio::isRX(){
	if( getIncomingFrameType() == ZIGBEE_RECIEVED_PACKET ){
		return true;
	}
	return false;
}

/**
*  Method: getRXAddress()
*  @ Since v0.1.0 by Eric Burger, January 2014
*  @ Returns the source address of incoming packet
*/
SimpleZigBeeAddress SimpleZigBeeRadio::getRXAddress(){
	return _incoming_packet.getRXAddress();
}

/**
*  Method: getRXAddress64()
*  @ Since v0.1.0 by Eric Burger, June 2014
*  @ Returns the 64-bit source address of incoming packet
*/
SimpleZigBeeAddress64 SimpleZigBeeRadio::getRXAddress64(){
	return _incoming_packet.getRXAddress64();
}

/**
*  Method: getRXAddress16()
*  @ Since v0.1.0 by Eric Burger, June 2014
*  @ Returns the 16-bit source address of incoming packet
*/
SimpleZigBeeAddress16 SimpleZigBeeRadio::getRXAddress16(){
	return _incoming_packet.getRXAddress16();
}

/**
*  Method: getRXOptions()
*  @ Since v0.1.0 by Eric Burger, June 2014
*  @ Returns the options byte incoming packet
*/
uint8_t SimpleZigBeeRadio::getRXOptions(){
	return _incoming_packet.getRXOptions();
}

/**
*  Method: getRXPayloadLength()
*  @ Since v0.1.0 by Eric Burger, June 2014
*  @ Returns the payload length of the incoming packet.
*/
uint8_t SimpleZigBeeRadio::getRXPayloadLength(){
	return _incoming_packet.getRXPayloadLength();
}

/**
*  Method: getRXPayload(int index)
*  @ Since v0.1.0 by Eric Burger, January 2014
*  @ Returns the payload byte of the incoming packet at the specified index
*  @ param int index: Index of payload data
*/
uint8_t SimpleZigBeeRadio::getRXPayload(int index){
	return _incoming_packet.getRXPayload(index);
}

/*//////////////////////////////////////////////////////////////////////
							ZIGBEE TRANSMIT (TX) STATUS METHODS
/*//////////////////////////////////////////////////////////////////////

/**
*  Method: isTXStatus()
*  @ Since v0.1.0 by Eric Burger, June 2014
*  @ Checks if received packet is a TX Status and returns boolean
*/
bool SimpleZigBeeRadio::isTXStatus(){
	if( getIncomingFrameType() == ZIGBEE_TX_STATUS ){
		return true;
	}
	return false;
}

/**
*  Method: getTXStatusAddress16()
*  @ Since v0.1.0 by Eric Burger, June 2014
*  @ Returns the 16-bit source address of incoming packet (destination of TX request)
*/
SimpleZigBeeAddress16 SimpleZigBeeRadio::getTXStatusAddress16(){
	return _incoming_packet.getTXStatusAddress16();
}

/**
*  Method: getTXStatusRetryCount()
*  @ Since v0.1.0 by Eric Burger, June 2014
*  @ Returns value of retry count
*/
uint8_t SimpleZigBeeRadio::getTXStatusRetryCount(){
	return _incoming_packet.getTXStatusRetryCount();
}

/**
*  Method: getTXStatusDeliveryStatus()
*  @ Since v0.1.0 by Eric Burger, June 2014
*  @ Returns value of packet delivery status
*/
uint8_t SimpleZigBeeRadio::getTXStatusDeliveryStatus(){
	return _incoming_packet.getTXStatusDeliveryStatus();
}

/**
*  Method: getTXStatusDiscoveryStatus()
*  @ Since v0.1.0 by Eric Burger, June 2014
*  @ Returns value of packet discovery status
*/
uint8_t SimpleZigBeeRadio::getTXStatusDiscoveryStatus(){
	return _incoming_packet.getTXStatusDiscoveryStatus();
}

/*//////////////////////////////////////////////////////////////////////
									AT COMMAND RESPONSE METHODS
/*//////////////////////////////////////////////////////////////////////

/**
*  Method: isATResponse()
*  @ Since v0.1.0 by Eric Burger, January 2014
*  @ Checks if received packet is an AT Command Response and returns boolean
*/
bool SimpleZigBeeRadio::isATResponse(){
	if( getIncomingFrameType() == AT_COMMAND_RESPONSE ){
		return true;
	}
	return false;
}

/**
*  Method: getATResponseCommand()
*  @ Since v0.1.0 by Eric Burger, January 2014
*  @ Returns AT Response Command of incoming packet
*/
uint16_t SimpleZigBeeRadio::getATResponseCommand(){
	return _incoming_packet.getATResponseCommand();
}

/**
*  Method: getATResponseStatus()
*  @ Since v0.1.0 by Eric Burger, January 2014
*  @ Returns AT Response Status
*/
uint8_t SimpleZigBeeRadio::getATResponseStatus(){
	return _incoming_packet.getATResponseStatus();
}

/**
*  Method: getATResponsePayloadLength()
*  @ Since v0.1.0 by Eric Burger, June 2014
*  @ Returns AT Response Payload Length
*/
uint8_t SimpleZigBeeRadio::getATResponsePayloadLength(){
	return _incoming_packet.getATResponsePayloadLength();
}

/**
*  Method: getATResponsePayload()
*  @ Since v0.1.0 by Eric Burger, January 2014
*  @ Returns AT Command Payload
*/
uint8_t SimpleZigBeeRadio::getATResponsePayload(){
	return _incoming_packet.getATResponsePayload();
}

/**
*  Method: getATResponsePayload(int index)
*  @ Since v0.1.0 by Eric Burger, January 2014
*  @ Returns AT Command Payload
*  @ param int index: Index of command payload
*/
uint8_t SimpleZigBeeRadio::getATResponsePayload(int index){
	return _incoming_packet.getATResponsePayload(index);
}

/*//////////////////////////////////////////////////////////////////////
						REMOTE AT COMMAND RESPONSE METHODS
/*//////////////////////////////////////////////////////////////////////

/**
*  Method: isRemoteATResponse()
*  @ Since v0.1.0 by Eric Burger, July 2014
*  @ Checks if received packet is a Remote AT Command Response and returns boolean
*/
bool SimpleZigBeeRadio::isRemoteATResponse(){
	if( getIncomingFrameType() == REMOTE_AT_COMMAND_RESPONSE ){
		return true;
	}
	return false;
}

/**
*  Method: getRemoteATResponseAddress()
*  @ Since v0.1.0 by Eric Burger, July 2014
*  @ Returns the source (remote) address of incoming packet
*/
SimpleZigBeeAddress SimpleZigBeeRadio::getRemoteATResponseAddress(){
	return _incoming_packet.getRemoteATResponseAddress();
}

/**
*  Method: getRemoteATResponseAddress64()
*  @ Since v0.1.0 by Eric Burger, July 2014
*  @ Returns the 64-bit source (remote) address of incoming packet
*/
SimpleZigBeeAddress64 SimpleZigBeeRadio::getRemoteATResponseAddress64(){
	return _incoming_packet.getRemoteATResponseAddress64();
}

/**
*  Method: getRemoteATResponseAddress16()
*  @ Since v0.1.0 by Eric Burger, July 2014
*  @ Returns the 16-bit source (remote) address of incoming packet
*/
SimpleZigBeeAddress16 SimpleZigBeeRadio::getRemoteATResponseAddress16(){
	return _incoming_packet.getRemoteATResponseAddress16();
}

/**
*  Method: getRemoteATResponseCommand()
*  @ Since v0.1.0 by Eric Burger, July 2014
*  @ Returns AT Command of incoming packet
*/
uint16_t SimpleZigBeeRadio::getRemoteATResponseCommand(){
	return _incoming_packet.getRemoteATResponseCommand();
}

/**
*  Method: getRemoteATResponseStatus()
*  @ Since v0.1.0 by Eric Burger, July 2014
*  @ Returns AT Command Status
*/
uint8_t SimpleZigBeeRadio::getRemoteATResponseStatus(){
	return _incoming_packet.getRemoteATResponseStatus();
}

/**
*  Method: getRemoteATResponsePayloadLength()
*  @ Since v0.1.0 by Eric Burger, July 2014
*  @ Returns AT Command Data Length
*/
uint8_t SimpleZigBeeRadio::getRemoteATResponsePayloadLength(){
	return _incoming_packet.getRemoteATResponsePayloadLength();
}

/**
*  Method: getRemoteATResponsePayload()
*  @ Since v0.1.0 by Eric Burger, July 2014
*  @ Returns AT Command Data
*/
uint8_t SimpleZigBeeRadio::getRemoteATResponsePayload(){
	return _incoming_packet.getRemoteATResponsePayload();
}

/**
*  Method: getRemoteATResponsePayload(int index)
*  @ Since v0.1.0 by Eric Burger, July 2014
*  @ Returns AT Command Data
*  @ param int index: Index of command data
*/
uint8_t SimpleZigBeeRadio::getRemoteATResponsePayload(int index){
	return _incoming_packet.getRemoteATResponsePayload(index);
}

/*//////////////////////////////////////////////////////////////////////
											Modem Status Methods
/*//////////////////////////////////////////////////////////////////////

/**
*  Method: isModemStatus()
*  @ Since v0.1.0 by Eric Burger, July 2014
*  @ Checks if received packet is an Modem Status and returns boolean
*/
bool SimpleZigBeeRadio::isModemStatus(){
	if( getIncomingFrameType() == MODEM_STATUS ){
		return true;
	}
	return false;
}

/**
*  Method: getModemStatus()
*  @ Since v0.1.0 by Eric Burger, July 2014
*  @ Returns value of incoming packet modem status
*/
uint8_t SimpleZigBeeRadio::getModemStatus(){
	return _incoming_packet.getModemStatus();
}

/*//////////////////////////////////////////////////////////////////////
									OUTGOING PACKET METHODS
/*//////////////////////////////////////////////////////////////////////

/**
*  Method: setOutgoingFrameType(uint8_t frameType)
*  @ Since v0.1.0 by Eric Burger, September 2013
*  @ Set frame type for outgoing packets
*  @ param uint8_t frameType: Frame type of outgoing ZigBee packet
*/
void SimpleZigBeeRadio::setOutgoingFrameType(uint8_t frameType){
	_outgoing_packet.setFrameType(frameType);
}

/**
*  Method: setOutgoingFrameID(uint8_t id)
*  @ Since v0.1.0 by Eric Burger, September 2013
*  @ Set frame ID for outgoing packets
*  @ param uint8_t id: Frame id number
*/
void SimpleZigBeeRadio::setOutgoingFrameID(uint8_t id){
	_outgoing_packet.setFrameID(id);
}

/**
*  Method: setAcknowledgement(bool ack)
*  @ Since v0.1.0 by Eric Burger, September 2013
*  @ Require acknowledgement for outgoing packets
*  @ param bool ack: True to require acknowledgement and set frame id
*/
void SimpleZigBeeRadio::setAcknowledgement(bool ack){
	_out_acknowledgement = ack;
	if( _out_acknowledgement == true){
		setOutgoingFrameID( 1 );
	}else{
		setOutgoingFrameID( 0 );
	}
}

/**
*  Method: getLastFrameID()
*  @ Since v0.1.0 by Eric Burger, July 2014
*  @ Return frame id of the last outgoing packet
*/
uint8_t SimpleZigBeeRadio::getLastFrameID(){
	return _out_frame_id;
}

/**
*  Method: saveLastFrameID(uint8_t frameID)
*  @ Since v0.1.0 by Eric Burger, July 2014
*  @ Save the frame id of the last outgoing packet
*  @ uint8_t frameID: ID to store
*/
void SimpleZigBeeRadio::saveLastFrameID(uint8_t frameID){
	_out_frame_id = frameID;
}

/**
*  Method: setNextFrameID()
*  @ Since v0.1.0 by Eric Burger, August 2013
*  @ Sets frame ID (Packet index 4, Frame Index 1)
*      If acknowledgement requested, increment frame id using mod operator and set value. Otherwise, set 0.
*      Note: Maximum value stored in a byte is 255.
*/
void SimpleZigBeeRadio::setNextFrameID(){
	uint8_t id = 0;
	if( _out_acknowledgement == true ){
		uint8_t prev = getLastFrameID();
		id = ((prev)%255 + 1) ;
	}
	setOutgoingFrameID( id );
}

/**
*  Method: setOutgoingFrameData(int index, uint8_t byte)
*  @ Since v0.1.0 by Eric Burger, September 2013
*  @ Sets the frame byte of the outgoing packet at the specified index.
*  @ int index: Index of frame data
*  @ uint8_t byte: Byte to store
*/
void SimpleZigBeeRadio::setOutgoingFrameData(int index, uint8_t byte){
	_outgoing_packet.setFrameData(index,byte);
}

/**
*  Method: setOutgoingFrameData(int startIndex, uint8_t* frameData, int frameDataLength)
*  @ Since v0.1.0 by Eric Burger, September 2013
*  @ Sets an array of bytes in the memory array of the outgoing packet, 
*    starting from specified start index.
*  @ param int startIndex: Index at which to start storing bytes
*  @ param uint8_t* frameData: Pointer to array of bytes to store
*  @ param int frameDataLength: Length of array to input
*/
void SimpleZigBeeRadio::setOutgoingFrameData(int startIndex, uint8_t* frameData, int frameDataLength){
	_outgoing_packet.setFrameData(startIndex,frameData,frameDataLength);
}


/**
*  Method: setOutgoingAddress(SimpleZigBeeAddress address)
*  @ Since v0.1.0 by Eric Burger, September 2013
*  @ Set the 64-bit and 16-bit destination addresses of the outgoing packet
*  @ param SimpleZigBeeAddress address: Object containing 64-bit and 16-bit destination addresses
*/
void SimpleZigBeeRadio::setOutgoingAddress(SimpleZigBeeAddress address){ 
	_outgoing_packet.setAddress(address);
}

/**
*  Method: setOutgoingAddress(uint32_t adr64MSB, uint32_t adr64LSB, uint16_t adr16)
*  @ Since v0.1.0 by Eric Burger, September 2013
*  @ Set the 64-bit and 16-bit destination addresses of the outgoing packet
*  @ param uint32_t adr64MSB: Most significant bytes (1st half) of 64-bit address
*  @ param uint32_t adr64LSB: Least significant bytes (2nd half) of 64-bit address 
*  @ param uint16_t adr16: 16-bit destination address 
*/
void SimpleZigBeeRadio::setOutgoingAddress(uint32_t adr64MSB, uint32_t adr64LSB, uint16_t adr16){
	_outgoing_packet.setAddress(adr64MSB,adr64LSB,adr16);
}

/**
*  Method: setOutgoingAddress64(uint32_t adr64MSB, uint32_t adr64LSB)
*  @ Since v0.1.0 by Eric Burger, September 2013
*  @ Set the 64-bit destination addresses of the outgoing packet
*  @ param uint32_t adr64MSB: Most significant bytes (1st half) of 64-bit address
*  @ param uint32_t adr64LSB: Least significant bytes (2nd half) of 64-bit address 
*/
void SimpleZigBeeRadio::setOutgoingAddress64(uint32_t adr64MSB, uint32_t adr64LSB){
	_outgoing_packet.setAddress64(adr64MSB,adr64LSB);
}

/**
*  Method: setOutgoingAddress16(uint16_t adr16)
*  @ Since v0.1.0 by Eric Burger, September 2013
*  @ Set the 16-bit destination address of the outgoing packet
*  @ param uint16_t adr16: 16-bit destination address  
*/
void SimpleZigBeeRadio::setOutgoingAddress16(uint16_t adr16){
	_outgoing_packet.setAddress16(adr16);
}

/**
*  Method: send()
*  @ Since v0.1.0 by Eric Burger, September 2013
*  @ Send packet to serial port based on _outgoing_packet object
*/ 
void SimpleZigBeeRadio::send(){
	saveLastFrameID( _outgoing_packet.getFrameID() ); // Record frame ID
	sendPacket(_outgoing_packet);
}

/**
*  Method: send(SimpleZigBeePacket & packet)
*  @ Since v0.1.0 by Eric Burger, September 2013
*  @ Send packet to serial port based on input Packet object
*  @ param SimpleZigBeePacket & packet: Pointer to packet object
*/  
void SimpleZigBeeRadio::send(SimpleZigBeePacket & packet){
	sendPacket(packet);
}

/**
*  Method: sendPacket(SimpleZigBeePacket & p)
*  @ Since v0.1.0 by Eric Burger, September 2013
*  @ Send packet to serial port
*  @ param SimpleZigBeePacket & p: Pointer to packet object 
*/
void SimpleZigBeeRadio::sendPacket(SimpleZigBeePacket & p){
	// Everything should be ready to go, so write it to the serial port...
	write( START );
	writeByte( p.getLengthMSB() );
	writeByte( p.getLengthLSB() );
	// Frame Type and Frame ID are stored in Frame Data
	uint8_t checksum = 0;
	for( int i=0; i<p.getFrameLength(); i++){
		writeByte(p.getFrameData(i));
		checksum += p.getFrameData(i); 
	}
	// Calculate checksum based on summation of frame bytes
	checksum = 0xff - checksum;
	writeByte(checksum);
	flush();
}

/**
*  Method: writeByte(uint8_t byte)
*  @ Since v0.1.0 by Eric Burger, September 2013
*  @ Check for escape characters before writing to serial port. Escape by writing escape 
*    character and applying bitwise XOR operator to byte. 
*  @ param uint8_t byte: Byte to check
*/
void SimpleZigBeeRadio::writeByte(uint8_t byte){
	if( _escaped_mode && (START == byte || XON == byte || XOFF == byte || ESCAPE == byte) ){
		write(ESCAPE);
		write(byte ^ 0x20);
	}else{
		write(byte);
	}
}

/**
*  Method: write(uint8_t byte)
*  @ Since v0.1.0 by Eric Burger, September 2013
*  @ Write byte to serial port
*  @ param uint8_t byte: Byte to write to serial port
*/
void SimpleZigBeeRadio::write(uint8_t byte){
	_serial->write(byte);
}

/**
*  Method: flush()
*  @ Since v0.1.0 by Eric Burger, September 2013
*  @ Flush serial port (wait until all outgoing bytes have been sent)
*/
void SimpleZigBeeRadio::flush(){
	if( _is_software_serial ){
		// July 2014: With SoftwareSerial, flush() clears incoming buffer  
		// (same as pre Arduino 1.0 HardwareSerial flush()). Disable this method
		// until this is fixed/changed. Apply a short delay instead.
		// Reference: http://forum.arduino.cc/index.php?topic=87651.0;wap2
		delay(50);
	}else{
		// For HardwareSerial, use flush()
		_serial->flush();
	}

}

/*//////////////////////////////////////////////////////////////////////
								ZIGBEE TRANSMIT (TX) REQUEST METHODS
/*//////////////////////////////////////////////////////////////////////

/**
*  Method: setTXRequestBroadcastRadius(uint8_t rad)
*  @ Since v0.1.0 by Eric Burger, September 2013
*  @ Set broadcast radius of the outgoing packet
*  @ param uint8_t rad: Maximum radius (# of hops) of packets, 0 for no limit
*/
void SimpleZigBeeRadio::setTXRequestBroadcastRadius(uint8_t rad){
	_outgoing_packet.setTXRequestBroadcastRadius(rad);
}

/**
*  Method: setTXRequestOption(uint8_t opt)
*  @ Since v0.1.0 by Eric Burger, September 2013
*  @ Set broadcast radius of the outgoing packet
*  @ param uint8_t opt: Option byte
*/
void SimpleZigBeeRadio::setTXRequestOption(uint8_t opt){
	_outgoing_packet.setTXRequestOption(opt);
}

/**
*  Method: setTXRequestPayload(uint8_t* payload, int payloadSize)
*  @ Since v0.1.0 by Eric Burger, September 2013
*  @ Set payload of the outgoing packet
*  @ param uint8_t* payload: Pointer to array of bytes containing payload data
*  @ param int payloadSize: Length of payload array
*/
void SimpleZigBeeRadio::setTXRequestPayload(uint8_t* payload, int payloadSize){
	_outgoing_packet.setTXRequestPayload(payload,payloadSize);
}

/**
*  Method: prepareTXRequest(uint32_t adr64MSB, uint32_t adr64LSB, uint16_t adr16, uint8_t* payload, int payloadSize)
*  @ Since v0.1.0 by Eric Burger, April 2014
*  @ Easy to use method for sending transmit request
*  @ param uint32_t adr64MSB: Most significant bytes (1st half) of 64-bit address
*  @ param uint32_t adr64LSB: Least significant bytes (2nd half) of 64-bit address 
*  @ param uint16_t adr16: 16-bit destination address 
*  @ param uint8_t* payload: Pointer to array of bytes to store
*  @ param int payloadSize: Length of payload array 
*/
void SimpleZigBeeRadio::prepareTXRequest(uint32_t adr64MSB, uint32_t adr64LSB, uint16_t adr16, uint8_t* payload, int payloadSize){
	// Clear checksum, frame length, and any error. Set broadcast radius and frame options to 0.
	resetOutgoing();
	setTXRequestPayload(payload, payloadSize); // Set payload first so that memory array expansion occurs only once, if applicable
	setOutgoingFrameType(ZIGBEE_TRANSMIT_REQUEST);
	setOutgoingAddress(adr64MSB,adr64LSB,adr16);
	setTXRequestBroadcastRadius(0);
	setTXRequestOption(0);
	setNextFrameID();
}

/**
*  Method: prepareTXRequest(SimpleZigBeeAddress address, uint8_t* payload, int payloadSize)
*  @ Since v0.1.0 by Eric Burger, April 2014
*  @ Easy to use method for sending transmit request
*  @ param SimpleZigBeeAddress address: Object containing 64-bit and 16-bit destination addresses
*  @ param uint8_t* payload: Pointer to array of bytes to store
*  @ param int payloadSize: Length of payload array 
*/
void SimpleZigBeeRadio::prepareTXRequest(SimpleZigBeeAddress address, uint8_t* payload, int payloadSize){
	prepareTXRequest(COORDINATOR_ADDRESS_64_MSB,COORDINATOR_ADDRESS_64_LSB,BROADCAST_ADDRESS_16,payload,payloadSize);
	setOutgoingAddress(address);
}

/**
*  Method: prepareTXRequestBroadcast(uint8_t* payload, int payloadSize)
*  @ Since v0.1.0 by Eric Burger, April 2014
*  @ Easy to use method for sending broadcast payload
*  @ param uint8_t* payload: Pointer to array of bytes to store
*  @ param int payloadSize: Length of payload array 
*/
void SimpleZigBeeRadio::prepareTXRequestBroadcast(uint8_t* payload, int payloadSize){
	prepareTXRequest(BROADCAST_ADDRESS_64_MSB,BROADCAST_ADDRESS_64_LSB,BROADCAST_ADDRESS_16,payload,payloadSize);
}

/**
*  Method: prepareTXRequestToCoordinator(uint8_t* payload, int payloadSize)
*  @ Since v0.1.0 by Eric Burger, April 2014
*  @ Easy to use method for sending payload to coordinator
*  @ param uint8_t* payload: Pointer to array of bytes to store
*  @ param int payloadSize: Length of payload array 
*/
void SimpleZigBeeRadio::prepareTXRequestToCoordinator(uint8_t* payload, int payloadSize){
	prepareTXRequest(COORDINATOR_ADDRESS_64_MSB,COORDINATOR_ADDRESS_64_LSB,BROADCAST_ADDRESS_16,payload,payloadSize);
}

/*//////////////////////////////////////////////////////////////////////
												AT COMMAND METHODS
/*//////////////////////////////////////////////////////////////////////

/**
*  Method: setATCommand(uint16_t command)
*  @ Since v0.1.0 by Eric Burger, July 2014
*  @ Set AT command of outgoing packet.
*  @ param uint16_t command: 16-bit AT Command
*/
void SimpleZigBeeRadio::setATCommand(uint16_t command){
	_outgoing_packet.setATCommand(command); 
}

/**
*  Method: setATCommandPayload(uint8_t payload)
*  @ Since v0.1.0 by Eric Burger, July 2014
*  @ Set AT command parameter of outgoing packet.
*  @ param uint8_t payload: Byte containing payload
*/
void SimpleZigBeeRadio::setATCommandPayload(uint8_t payload){
	_outgoing_packet.setATCommandPayload(payload);
}

/**
*  Method: setATCommandPayload(uint8_t* payload, int payloadSize)
*  @ Since v0.1.0 by Eric Burger, July 2014
*  @ Set AT command parameter of outgoing packet.
*  @ param uint8_t* payload: Pointer to array of bytes containing payload
*  @ param int payloadSize: Length of payload array
*/
void SimpleZigBeeRadio::setATCommandPayload(uint8_t* payload, int payloadSize){
	_outgoing_packet.setATCommandPayload(payload, payloadSize);
}

/**
*  Method: prepareATCommand(uint16_t command)
*  @ Since v0.1.0 by Eric Burger, April 2014
*  @ Easy to use method for sending AT command
*  @ param uint16_t command: 16-bit AT Command
*/
void SimpleZigBeeRadio::prepareATCommand(uint16_t command){
	// Clear checksum, frame length, and any error.
	resetOutgoing();
	setATCommand(command); 
	setOutgoingFrameType(AT_COMMAND);
	setNextFrameID();
}

/**
*  Method: prepareATCommand(uint16_t command, uint8_t payload)
*  @ Since v0.1.0 by Eric Burger, April 2014
*  @ Easy to use method for sending AT command
*  @ param uint16_t command: 16-bit AT Command
*  @ param uint8_t payload: Byte containing payload
*/
void SimpleZigBeeRadio::prepareATCommand(uint16_t command, uint8_t payload){
	prepareATCommand(command);
	setATCommandPayload(payload);
}

/**
*  Method: prepareATCommand(uint16_t command, uint8_t* payload, int payloadSize)
*  @ Since v0.1.0 by Eric Burger, April 2014
*  @ Easy to use method for sending AT command
*  @ param uint16_t command: 16-bit AT Command
*  @ param uint8_t* payload: Pointer to array of bytes containing payload
*  @ param int payloadSize: Length of payload array
*/
void SimpleZigBeeRadio::prepareATCommand(uint16_t command, uint8_t* payload, int payloadSize){
	prepareATCommand(command);
	setATCommandPayload(payload, payloadSize);
}

/*//////////////////////////////////////////////////////////////////////
										REMOTE AT COMMAND METHODS
/*//////////////////////////////////////////////////////////////////////

/**
*  Method: setRemoteATCommandOption(uint8_t opt)
*  @ Since v0.1.0 by Eric Burger, July 2014
*  @ Set remote command option of outgoing packet.
*  @ param uint8_t opt: Option value to set 
*/
void SimpleZigBeeRadio::setRemoteATCommandOption(uint8_t opt){
	_outgoing_packet.setRemoteATCommandOption(opt);
}

/**
*  Method: setRemoteATCommand(uint16_t command)
*  @ Since v0.1.0 by Eric Burger, April 2014
*  @ Store Remote AT command of outgoing packet.
*  @ param uint16_t command: 16-bit AT Command
*/
void SimpleZigBeeRadio::setRemoteATCommand(uint16_t command){
	_outgoing_packet.setRemoteATCommand(command);
}

/**
*  Method: setRemoteATCommandPayload(uint8_t payload)
*  @ Since v0.1.0 by Eric Burger, July 2014
*  @ Store AT command parameter of outgoing packet.
*  @ param uint8_t payload: Byte containing payload
*/
void SimpleZigBeeRadio::setRemoteATCommandPayload(uint8_t payload){
	_outgoing_packet.setRemoteATCommandPayload(payload);
}

/**
*  Method: setRemoteATCommandPayload(uint8_t* payload, int payloadSize)
*  @ Since v0.1.0 by Eric Burger, July 2014
*  @ Store AT command parameter of outgoing packet.
*  @ param uint8_t* payload: Pointer to array of bytes containing payload
*  @ param int payloadSize: Length of payload array
*/
void SimpleZigBeeRadio::setRemoteATCommandPayload(uint8_t* payload, int payloadSize){
	_outgoing_packet.setRemoteATCommandPayload(payload,payloadSize);
}






/**
*  Method: prepareRemoteATCommand(uint32_t adr64MSB, uint32_t adr64LSB, uint16_t adr16, uint16_t command)
*  @ Since v0.1.0 by Eric Burger, July 2014
*  @ Easy to use method for sending AT command
*  @ param uint32_t adr64MSB: Most significant bytes of 64-bit address
*  @ param uint32_t adr64LSB: Least significant bytes of 64-bit address 
*  @ param uint16_t adr16: 16-bit destination address 
*  @ param uint16_t command: 16-bit AT Command
*/
void SimpleZigBeeRadio::prepareRemoteATCommand(uint32_t adr64MSB, uint32_t adr64LSB, uint16_t adr16, uint16_t command){
	// Clear checksum, frame length, and any error. Set option to apply changes (0x02).
	resetOutgoing();
	setRemoteATCommand(command);
	setOutgoingFrameType(REMOTE_AT_COMMAND);
	setOutgoingAddress(adr64MSB,adr64LSB,adr16);
	setRemoteATCommandOption(0x02);
	setNextFrameID();
}

/**
*  Method: prepareRemoteATCommand(uint32_t adr64MSB, uint32_t adr64LSB, uint16_t adr16, uint16_t command, uint8_t payload)
*  @ Since v0.1.0 by Eric Burger, July 2014
*  @ Easy to use method for sending AT command
*  @ param uint32_t adr64MSB: Most significant bytes of 64-bit address
*  @ param uint32_t adr64LSB: Least significant bytes of 64-bit address 
*  @ param uint16_t adr16: 16-bit destination address 
*  @ param uint16_t command: 16-bit AT Command
*  @ param uint8_t payload: Byte containing payload
*/
void SimpleZigBeeRadio::prepareRemoteATCommand(uint32_t adr64MSB, uint32_t adr64LSB, uint16_t adr16, uint16_t command, uint8_t payload){
	prepareRemoteATCommand(adr64MSB,adr64LSB,adr16,command);
	setRemoteATCommandPayload(payload);
}

/**
*  Method: prepareRemoteATCommand(uint32_t adr64MSB, uint32_t adr64LSB, uint16_t adr16, uint16_t command, uint8_t* payload, int payloadSize)
*  @ Since v0.1.0 by Eric Burger, July 2014
*  @ Easy to use method for sending AT command
*  @ param uint32_t adr64MSB: Most significant bytes of 64-bit address
*  @ param uint32_t adr64LSB: Least significant bytes of 64-bit address 
*  @ param uint16_t adr16: 16-bit destination address 
*  @ param uint16_t command: 16-bit AT Command
*  @ param uint8_t* payload: Pointer to array of bytes containing payload
*  @ param int payloadSize: Length of payload array
*/
void SimpleZigBeeRadio::prepareRemoteATCommand(uint32_t adr64MSB, uint32_t adr64LSB, uint16_t adr16, uint16_t command, uint8_t* payload, int payloadSize){
	prepareRemoteATCommand(adr64MSB,adr64LSB,adr16,command);
	setRemoteATCommandPayload(payload, payloadSize);
}









/**
*  Method: prepareRemoteATCommand(SimpleZigBeeAddress address, uint16_t command)
*  @ Since v0.1.0 by Eric Burger, July 2014
*  @ Easy to use method for sending AT command
*  @ param SimpleZigBeeAddress address: Object containing 64-bit and 16-bit destination addresses
*  @ param uint16_t command: 16-bit AT Command
*/
void SimpleZigBeeRadio::prepareRemoteATCommand(SimpleZigBeeAddress address, uint16_t command){
	// Clear checksum, frame length, and any error. Set option to apply changes (0x02).
	resetOutgoing();
	setRemoteATCommand(command);
	setOutgoingFrameType(REMOTE_AT_COMMAND);
	setOutgoingAddress(address);
	setRemoteATCommandOption(0x02);
	setNextFrameID();
}

/**
*  Method: prepareRemoteATCommand(SimpleZigBeeAddress address, uint16_t command, uint8_t payload)
*  @ Since v0.1.0 by Eric Burger, July 2014
*  @ Easy to use method for sending AT command
*  @ param SimpleZigBeeAddress address: Object containing 64-bit and 16-bit destination addresses
*  @ param uint16_t command: 16-bit AT Command
*  @ param uint8_t payload: Byte containing payload
*/
void SimpleZigBeeRadio::prepareRemoteATCommand(SimpleZigBeeAddress address, uint16_t command, uint8_t payload){
	prepareRemoteATCommand(address,command);
	setRemoteATCommandPayload(payload);
}

/**
*  Method: prepareRemoteATCommand(SimpleZigBeeAddress address, uint16_t command, uint8_t* payload, int payloadSize)
*  @ Since v0.1.0 by Eric Burger, July 2014
*  @ Easy to use method for sending AT command
*  @ param SimpleZigBeeAddress address: Object containing 64-bit and 16-bit destination addresses
*  @ param uint16_t command: 16-bit AT Command
*  @ param uint8_t* payload: Pointer to array of bytes containing payload
*  @ param int payloadSize: Length of payload array
*/
void SimpleZigBeeRadio::prepareRemoteATCommand(SimpleZigBeeAddress address, uint16_t command, uint8_t* payload, int payloadSize){
	prepareRemoteATCommand(address,command);
	setRemoteATCommandPayload(payload, payloadSize);
}
