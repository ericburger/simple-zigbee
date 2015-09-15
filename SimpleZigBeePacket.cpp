/**
* Copyright (c) 2013 Eric Burger. All rights reserved.
*/

#include "SimpleZigBeePacket.h"
// For memory allocation of array pointer (malloc and realloc)
//#include < ctype.h >

/*//////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
												SimpleZigBeePacket Class
////////////////////////////////////////////////////////////////////////
/*//////////////////////////////////////////////////////////////////////

/*//////////////////////////////////////////////////////////////////////
											INITIALIZATION METHODS
/*//////////////////////////////////////////////////////////////////////

/**
*  Constructor: SimpleZigBeePacket()
*  @ Since v0.1.0 by Eric Burger, August 2013
*  @ Default constructor. Allocates 20 bytes of memory to the memory array.
*      In this case, the function sizeof(uint8_t) is a little unnecessary since sizeof()
*      returns the number of bytes and since uint8_t represents one byte, sizeof(uint8_t) = 1.
*      I have left the complete expression because it is also a good way of creating an int array (sizeof(int)=4).
*      The expression (uint8_t*) casts the pointer returned by malloc as a uint8_t pointer.
*      For reference: http://www.cplusplus.com/reference/cstdlib/malloc/
*/
SimpleZigBeePacket::SimpleZigBeePacket() {
	_memoryArrayLength = 20;
	_ptrMemoryArray = (uint8_t*) malloc(sizeof(uint8_t) * _memoryArrayLength);
	_maxFrameLength = 50;
	init();
}

/**
*  Constructor: SimpleZigBeePacket()
*  @ Since v0.1.0 by Eric Burger, August 2013
*  @ Constructor with parameter. Set the maximum frame size to increase or decrease maximum memory 
*      allocation. Note the 64 byte size of the Arduino serial port buffer. If _maxFrameLength is set to larger 
*      than 64, Arduino program must insure that serial buffer does not overflow, resulting in incomplete packets.
*  @ param int maxFrameLength: Limit to the length (# of bytes) of the packet frame
*/
SimpleZigBeePacket::SimpleZigBeePacket(int maxFrameLength) {
	_memoryArrayLength = 20;
	_ptrMemoryArray = (uint8_t*) malloc(sizeof(uint8_t) * _memoryArrayLength);
	_maxFrameLength = maxFrameLength;
	init();
}

/**
*  Method: init()
*  @ Since v0.1.0 by Eric Burger, August 2013
*  @ Initiate the packet's private parameters
*/
void SimpleZigBeePacket::init() { 
	_checksum = 0;
	_frameLength = 0;
	_errorCode = NO_ERROR;
}

/**
*  Method: reset()
*  @ Since v0.1.0 by Eric Burger, August 2013
*  @ Resets the packet's private parameters
*/
void SimpleZigBeePacket::reset() { 
	init();
}

/*//////////////////////////////////////////////////////////////////////
												MEMORY METHODS
/*//////////////////////////////////////////////////////////////////////

/**
*  Method: expandMemoryArray(int size)
*  @ Since v0.1.0 by Eric Burger, August 2013
*  @ Expands the packet class's memory array based on input size or until maximum frame size limit is reached.
*      Input sizes smaller than current memory array length will be ignored. The packet class does not contain 
*      a function for reducing the memory array size. The reasoning being that if a certain size of packet has 
*      been sent or received in the past, it is likely to occur again. 
*  @ param int size: Desired size (# of bytes) of memory allocated to store the packet frame data
*/
void SimpleZigBeePacket::expandMemoryArray(int size){
	if( size > _memoryArrayLength && size <= getMaxFrameLength() ){
		uint8_t* new_array = NULL;
		// Re-allocate memory to accommodate the increased size of the memory array.
		// For reference: http://www.cplusplus.com/reference/cstdlib/realloc/
		new_array = (uint8_t*) realloc(_ptrMemoryArray,(sizeof(uint8_t) * size));
		if( new_array != NULL ){
			_ptrMemoryArray = new_array;
			_memoryArrayLength = size;
		}else{
			// Error re-allocating memory. No change change made to memory array.
			setErrorCode(ERROR_REALLOCATING_MEMORY);
		}
	}
}

/**
*  Method: SimpleZigBeePacket::getMemoryData(int index)
*  @ Since v0.1.0 by Eric Burger, August 2013
*  @ Return the byte that is stored at the specified index of the memory array.
*      Be aware that since you are accessing data from a memory address, the byte that is
*      returned may be junk from a previous packet or a previous code running on your Arduino.
*  @ param int index: Index of data in the memory array
*/
uint8_t SimpleZigBeePacket::getMemoryData(int index){
	if( index < _memoryArrayLength ){
		return _ptrMemoryArray[index];
	}
	return 0;
}

/**
*  Method: setMemoryData(int index, uint8_t byte)
*  @ Since v0.1.0 by Eric Burger, August 2013
*  @ Set the byte at the specified index of the memory array.
*      First, check that index is within permitted range. Expand memory array, if necessary.
*      Note: If maximum index is 49, maximum size is 50.
*  @ param int index: Index of the memory array to store data
*  @ param uint8_t byte: Data to store
*/
void SimpleZigBeePacket::setMemoryData(int index, uint8_t byte){
	if( index < getMaxFrameLength() ){
		if( index < _memoryArrayLength ){
			_ptrMemoryArray[index] = byte;
		}else{
			expandMemoryArray( (index+10) );
			_ptrMemoryArray[index] = byte;
		}
	}else{
		setErrorCode( MAX_FRAME_LENGTH_EXCEEDED );
	}
}

/*//////////////////////////////////////////////////////////////////////
								PACKET PRIVATE VARIABLE METHODS
/*//////////////////////////////////////////////////////////////////////

/**
*  Method: getFrameLength()
*  @ Since v0.1.0 by Eric Burger, August 2013
*  @ Returns current frame length
*/  
int SimpleZigBeePacket::getFrameLength(){
	return _frameLength;
}

/**
*  Method: getLengthMSB()
*  @ Since v0.1.0 by Eric Burger, August 2013
*  @  Returns "Most Significant Byte" of packet frame length (Packet Index 1) using the operators
*       right shift (>>) and bitwise AND (&).
*/ 
uint8_t SimpleZigBeePacket::getLengthMSB(){
	return ((getFrameLength() >> 8) & 0xff);
}

/**
*  Method: getLengthLSB()
*  @ Since v0.1.0 by Eric Burger, August 2013
*  @ Returns "Least Significant Byte" of packet frame length (Packet Index 2) using the operator bitwise AND (&).
*/
uint8_t SimpleZigBeePacket::getLengthLSB(){ 
	return (getFrameLength() & 0xff);
}

/**
*  Method: setFrameLength(int frameLength)
*  @ Since v0.1.0 by Eric Burger, August 2013
*  @ Sets current frame length
*  @ param int frameLength: Frame length value to store
*/  
void SimpleZigBeePacket::setFrameLength(int frameLength){
	_frameLength = frameLength;
}

/**
*  Method: setFrameLengthMSB(uint8_t msb)
*  @ Since v0.1.0 by Eric Burger, August 2013
*  @ Sets "Most Significant Byte" of packet's length (Packet Index 1)
*  @ param uint8_t msb: Most Significant Byte of length
*/
void SimpleZigBeePacket::setFrameLengthMSB(uint8_t msb){ 
	// Update frame length using the left shift (<<) operator.
	setFrameLength( uint16_t(msb << 8) + getLengthLSB() );
}

/**
*  Method: setFrameLengthLSB(uint8_t lsb)
*  @ Since v0.1.0 by Eric Burger, August 2013
*  @ Sets "Least Significant Byte" of packet's length (Packet Index 2)
*  @ param uint8_t lsb: Least Significant Byte of length 
*/
void SimpleZigBeePacket::setFrameLengthLSB(uint8_t lsb){
	// Update frame length using the left shift (<<) operator.
	setFrameLength( uint16_t(getLengthMSB() << 8) + lsb );
} 

/**
*  Method: getFrameType()
*  @ Since v0.1.0 by Eric Burger, August 2013
*  @ Returns frame type (Packet Index 3, Frame Index 0)
*/  
uint8_t SimpleZigBeePacket::getFrameType(){
	return getFrameData(0);
}

/**
*  Method: getFrameID()
*  @ Since v0.1.0 by Eric Burger, August 2013
*  @ Returns frame ID.
*/
uint8_t SimpleZigBeePacket::getFrameID(){
	return getFrameData(1);
}

/**
*  Method: getChecksum()
*  @ Since v0.1.0 by Eric Burger, August 2013
*  @ Returns the packet checksum (End of Packet)
*/
uint8_t SimpleZigBeePacket::getChecksum(){ 
	return _checksum;
}

/**
*  Method: calculateChecksum()
*  @ Since v0.1.0 by Eric Burger, June 2014
*  @ Calculates and returns the packet checksum (Does not set _checksum)
*/
uint8_t SimpleZigBeePacket::calculateChecksum(){ 
	uint8_t checksum = 0;
	for( int i=0; i<getFrameLength(); i++){
		checksum += getFrameData(i); 
	}
	// Calculate checksum based on summation of frame bytes
	checksum = 0xff - checksum;
	return _checksum;
}

/**
*  Method: setChecksum(uint8_t checksum)
*  @ Since v0.1.0 by Eric Burger, August 2013
*  @ Sets the packet checksum (End of Packet)
*  @ param uint8_t checksum: Checksum value to store 
*/
void SimpleZigBeePacket::setChecksum(uint8_t checksum){
	_checksum = checksum;
} 

/*//////////////////////////////////////////////////////////////////////
								PACKET FRAME METHODS
/*//////////////////////////////////////////////////////////////////////

/**
*  Method: getMaxFrameLength()
*  @ Since v0.1.0 by Eric Burger, August 2013
*  @ Returns the maximum frame length
*/
int SimpleZigBeePacket::getMaxFrameLength(){
	return _maxFrameLength;
}

/**
*  Method: setFrameData(int index, uint8_t byte)
*  @ Since v0.1.0 by Eric Burger, August 2013
*  @ Sets the frame byte of the packet at the specified index
*      and updates _frameLength, if necessary.
*  @ param int index: Index of frame data
*  @ param uint8_t byte: Byte to store
*/
void SimpleZigBeePacket::setFrameData(int index, uint8_t byte){
	if( index < getMaxFrameLength() ){
		setMemoryData(index, byte);
		if( (index+1) > getFrameLength() ){
			setFrameLength(index+1);
		}
	}else{
		setErrorCode( MAX_FRAME_LENGTH_EXCEEDED );
	}
}

/**
*  Method: setFrameData(int startIndex, uint8_t* frameData, int frameDataLength)
*  @ Since v0.1.0 by Eric Burger, August 2013
*  @ Sets an array of bytes in the memory array of the packet, starting
*      from specified start index,and updates _frameLength, if necessary.
*  @ param int startIndex: Index at which to start storing bytes
*  @ param uint8_t* frameData: Pointer to array of bytes to store
*  @ param int frameDataLength: Length of array to input
*/ 
void SimpleZigBeePacket::setFrameData(int startIndex, uint8_t* frameData, int frameDataLength){
	int lastIndex = startIndex + (frameDataLength - 1);
	if( lastIndex < getMaxFrameLength() ){
		// Expand memory array, if necessary
		expandMemoryArray( (lastIndex+10) ); 
		for(int i=0;i<frameDataLength;i++){
			setMemoryData(startIndex+i, frameData[i]);
		}
		if( (lastIndex+1) > getFrameLength() ){
			setFrameLength(lastIndex+1);
		}
	}else{
		setErrorCode( MAX_FRAME_LENGTH_EXCEEDED );
	}
}

/**
*  Method: getFrameData(int index)
*  @ Since v0.1.0 by Eric Burger, August 2013
*  @ Returns the frame byte of the incoming packet at the specified index
*  @ param int index: Index of frame data
*/
uint8_t SimpleZigBeePacket::getFrameData(int index){
	if( index < getFrameLength() ){
		return getMemoryData( index );
	}else{
		setErrorCode( FRAME_LENGTH_EXCEEDED );
		return 0;
	}
}

/**
*  Method: getFrameData(int startIndex, uint8_t* arrayPtr, int frameDataLength)
*  @ Since v0.1.0 by Eric Burger, August 2013
*  @ Places an array of bytes from the memory array of the incoming packet in the
*      provided array pointer, starting from specified start index.
*  @ param int startIndex: Index of frame data at which to start 
*  @ param uint8_t* arrayPtr: Array for storing bytes
*  @ param int frameDataLength: Number of bytes to return
*/
void SimpleZigBeePacket::getFrameData(int startIndex, uint8_t* arrayPtr, int frameDataLength){
	// Check that requested data is within length of frame
	if( (startIndex + (frameDataLength - 1)) < getFrameLength() ){
		for(int i=0;i<frameDataLength;i++){
			arrayPtr[i] = getMemoryData( startIndex+i );
		}    
	}else{
		setErrorCode( FRAME_LENGTH_EXCEEDED );
	}
}

/*//////////////////////////////////////////////////////////////////////
									ERROR CODE METHODS
/*//////////////////////////////////////////////////////////////////////

/**
*  Method: isError()
*  @ Since v0.1.0 by Eric Burger, August 2013
*  @ Check if packet class has encountered error
*/
bool SimpleZigBeePacket::isError() {
	return _errorCode > 0;
}

/**
*  Method: getErrorCode()
*  @ Since v0.1.0 by Eric Burger, August 2013
*  @ Return error code
*/
int SimpleZigBeePacket::getErrorCode() {
	return _errorCode;
}

/**
*  Method: setErrorCode(int errorCode)
*  @ Since v0.1.0 by Eric Burger, August 2013
*  @ Sets error code
*  @ param int errorCode: Error code value to store 
*/
void SimpleZigBeePacket::setErrorCode(int errorCode) {
	_errorCode = errorCode;
}


/*//////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
								SimpleIncomingZigBeePacket Class
////////////////////////////////////////////////////////////////////////
/*//////////////////////////////////////////////////////////////////////

/*//////////////////////////////////////////////////////////////////////
											INITIALIZATION METHODS
/*//////////////////////////////////////////////////////////////////////

/**
*  Constructor: SimpleZigBeeRadio()
*  @ Since v0.1.0 by Eric Burger, August 2013
*  @ Default constructor that creates packet object for incoming packets.
*/
SimpleIncomingZigBeePacket::SimpleIncomingZigBeePacket() : SimpleZigBeePacket(){
	SimpleZigBeePacket::init();
}

/**
*  Constructor: SimpleZigBeeRadio()
*  @ Since v0.1.0 by Eric Burger, August 2013
*  @ Constructor with inputs that creates packet object for incoming packets.
*  @ param int maxFrameLength: Limit (# of bytes) on packet frame size
*/
SimpleIncomingZigBeePacket::SimpleIncomingZigBeePacket(int maxFrameLength) : SimpleZigBeePacket(maxFrameLength) {
	SimpleZigBeePacket::init();
}

/*//////////////////////////////////////////////////////////////////////
							ZIGBEE RECIEVED (RX) PACKET METHODS
/*//////////////////////////////////////////////////////////////////////

/**
*  Method: getRXAddress()
*  @ Since v0.1.0 by Eric Burger, January 2014
*  @ Returns the source address of packet
*/
SimpleZigBeeAddress SimpleIncomingZigBeePacket::getRXAddress(){
	return SimpleZigBeeAddress( getRXAddress64(), getRXAddress16() );
}

/**
*  Method: getRXAddress64()
*  @ Since v0.1.0 by Eric Burger, January 2014
*  @ Returns the 64-bit source address of packet
*  @ There is a typo in Digi XBee S2 manual. No Frame ID for RX Packet
*  @ Cast bytes as 32-bit or 16-bit before bitshift left
*/
SimpleZigBeeAddress64 SimpleIncomingZigBeePacket::getRXAddress64(){
	uint32_t msb = (uint32_t(getFrameData(1)) << 24) + (uint32_t(getFrameData(2)) << 16) + (uint16_t(getFrameData(3)) << 8) + getFrameData(4);
	uint32_t lsb = (uint32_t(getFrameData(5)) << 24) + (uint32_t(getFrameData(6)) << 16) + (uint16_t(getFrameData(7)) << 8) + getFrameData(8);
	return SimpleZigBeeAddress64( msb, lsb );
}

/**
*  Method: getRXAddress16()
*  @ Since v0.1.0 by Eric Burger, January 2014
*  @ Returns the 16-bit source address of packet
*  @ Cast bytes as 16-bit before bitshift left 
*/
SimpleZigBeeAddress16 SimpleIncomingZigBeePacket::getRXAddress16(){
	uint16_t addr = (uint16_t(getFrameData(9)) << 8) + getFrameData(10);
	return SimpleZigBeeAddress16( addr );
}

/**
*  Method: getRXOptions()
*  @ Since v0.1.0 by Eric Burger, January 2014
*  @ Returns value of packet receive option (Packet Index 14, Frame Index 11)
*/
uint8_t SimpleIncomingZigBeePacket::getRXOptions(){
	return getFrameData(11);
}
/**
*  Method: getRXPayloadLength(int index)
*  @ Since v0.1.0 by Eric Burger, June 2014
*  @ Returns the payload length of the incoming packet. 
*/
uint8_t SimpleIncomingZigBeePacket::getRXPayloadLength(){
	return getFrameLength()-12;
}
/**
*  Method: getRXPayload(int index)
*  @ Since v0.1.0 by Eric Burger, January 2014
*  @ Returns the payload byte of the incoming packet at the specified index. 
*  @ (Starting at Packet Index 15, Frame Index 12)
*  @ param int index: Index of payload data
*/
uint8_t SimpleIncomingZigBeePacket::getRXPayload(int index){
	return getFrameData(index+12);
}

/*//////////////////////////////////////////////////////////////////////
							ZIGBEE TRANSMIT (TX) STATUS METHODS
/*//////////////////////////////////////////////////////////////////////

/**
*  Method: getTXStatusAddress16()
*  @ Since v0.1.0 by Eric Burger, January 2014
*  @ Returns the 16-bit source address of packet (destination of TX request)
*  @ Cast bytes as 16-bit before bitshift left 
*/
SimpleZigBeeAddress16 SimpleIncomingZigBeePacket::getTXStatusAddress16(){
	uint16_t addr = (uint16_t(getFrameData(9)) << 2) + getFrameData(3);
	return SimpleZigBeeAddress16( addr );
}

/**
*  Method: getTXStatusRetryCount()
*  @ Since v0.1.0 by Eric Burger, April 2014
*  @ Returns value of retry count (Packet Index 7, Frame Index 4)
*/
uint8_t SimpleIncomingZigBeePacket::getTXStatusRetryCount(){
	return getFrameData(4);
}

/**
*  Method: getTXStatusDeliveryStatus()
*  @ Since v0.1.0 by Eric Burger, April 2014
*  @ Returns value of packet delivery status (Packet Index 8, Frame Index 5)
*/
uint8_t SimpleIncomingZigBeePacket::getTXStatusDeliveryStatus(){
	return getFrameData(5);
}

/**
*  Method: getTXStatusDiscoveryStatus()
*  @ Since v0.1.0 by Eric Burger, April 2014
*  @ Returns value of packet discovery status (Packet Index 9, Frame Index 6)
*/
uint8_t SimpleIncomingZigBeePacket::getTXStatusDiscoveryStatus(){
	return getFrameData(6);
}

/*//////////////////////////////////////////////////////////////////////
										AT COMMAND RESPONSE METHODS
/*//////////////////////////////////////////////////////////////////////

/**
*  Method: getATResponseCommand()
*  @ Since v0.1.0 by Eric Burger, January 2014
*  @ Returns AT Command of packet (Packet index 5 and 6, Frame Index 2 and 3)
*/
uint16_t SimpleIncomingZigBeePacket::getATResponseCommand(){
	return uint16_t( getFrameData(2) << 8 )  + getFrameData(3) ;
}

/**
*  Method: getATResponseStatus()
*  @ Since v0.1.0 by Eric Burger, January 2014
*  @ Returns AT Command Status (Packet index 7, Frame Index 4)
*/
uint8_t SimpleIncomingZigBeePacket::getATResponseStatus(){
	return getFrameData(4) ;
}

/**
*  Method: getATResponsePayloadLength()
*  @ Since v0.1.0 by Eric Burger, June 2014
*  @ Returns AT Command Data Length
*/
uint8_t SimpleIncomingZigBeePacket::getATResponsePayloadLength(){
	return getFrameLength()-5;
}

/**
*  Method: getATResponsePayload()
*  @ Since v0.1.0 by Eric Burger, January 2014
*  @ Returns AT Command Data (Packet index 8, Frame Index 5)
*/
uint8_t SimpleIncomingZigBeePacket::getATResponsePayload(){
	return getFrameData(5);
}

/**
*  Method: getATResponsePayload(int index)
*  @ Since v0.1.0 by Eric Burger, January 2014
*  @ Returns AT Command Data (Starting at Packet index 8, Frame Index 5)
*  @ param int index: Index of command data
*/
uint8_t SimpleIncomingZigBeePacket::getATResponsePayload(int index){
	return getFrameData(index+5);
}

/*//////////////////////////////////////////////////////////////////////
									REMOTE AT COMMAND RESPONSE METHODS
/*//////////////////////////////////////////////////////////////////////

/**
*  Method: getRemoteATResponseAddress()
*  @ Since v0.1.0 by Eric Burger, July 2014
*  @ Returns the source (remote) address of packet
*/
SimpleZigBeeAddress SimpleIncomingZigBeePacket::getRemoteATResponseAddress(){
	return SimpleZigBeeAddress( getRemoteATResponseAddress64(), getRemoteATResponseAddress16() );
}

/**
*  Method: getRemoteATResponseAddress64()
*  @ Since v0.1.0 by Eric Burger, July 2014
*  @ Returns the 64-bit source (remote) address of packet
*/
SimpleZigBeeAddress64 SimpleIncomingZigBeePacket::getRemoteATResponseAddress64(){
	uint32_t msb = (uint32_t(getFrameData(2)) << 24) + (uint32_t(getFrameData(3)) << 16) + (uint16_t(getFrameData(4)) << 8) + getFrameData(5);
	uint32_t lsb = (uint32_t(getFrameData(6)) << 24) + (uint32_t(getFrameData(7)) << 16) + (uint16_t(getFrameData(8)) << 8) + getFrameData(9);
	return SimpleZigBeeAddress64( msb, lsb );
}

/**
*  Method: getRemoteATResponseAddress16()
*  @ Since v0.1.0 by Eric Burger, July 2014
*  @ Returns the 16-bit source (remote) address of packet
*/
SimpleZigBeeAddress16 SimpleIncomingZigBeePacket::getRemoteATResponseAddress16(){
	uint16_t addr = (uint16_t(getFrameData(10)) << 8) + getFrameData(11);
	return SimpleZigBeeAddress16( addr );
}

/**
*  Method: getRemoteATResponseCommand()
*  @ Since v0.1.0 by Eric Burger, July 2014
*  @ Returns Remote AT Command of packet (Packet index 15 and 16, Frame Index 12 and 13)
*/
uint16_t SimpleIncomingZigBeePacket::getRemoteATResponseCommand(){
	return uint16_t( getFrameData(12) << 8 )  + getFrameData(13) ;
}

/**
*  Method: getRemoteATResponseStatus()
*  @ Since v0.1.0 by Eric Burger, July 2014
*  @ Returns Remote AT Command Status (Packet index 17, Frame Index 14)
*/
uint8_t SimpleIncomingZigBeePacket::getRemoteATResponseStatus(){
	return getFrameData(14) ;
}

/**
*  Method: getRemoteATResponsePayloadLength()
*  @ Since v0.1.0 by Eric Burger, July 2014
*  @ Returns Remote AT Command Data Length
*/
uint8_t SimpleIncomingZigBeePacket::getRemoteATResponsePayloadLength(){
	return getFrameLength()-15;
}

/**
*  Method: getRemoteATResponsePayload()
*  @ Since v0.1.0 by Eric Burger, July 2014
*  @ Returns Remote AT Command Data (Packet index 18, Frame Index 15)
*/
uint8_t SimpleIncomingZigBeePacket::getRemoteATResponsePayload(){
	return getFrameData(15);
}

/**
*  Method: getRemoteATResponsePayload(int index)
*  @ Since v0.1.0 by Eric Burger, July 2014
*  @ Returns Remote AT Command Data (Starting at Packet index 18, Frame Index 15)
*  @ param int index: Index of command data
*/
uint8_t SimpleIncomingZigBeePacket::getRemoteATResponsePayload(int index){
	return getFrameData(index+15);
}

/*//////////////////////////////////////////////////////////////////////
											Modem Status Methods
/*//////////////////////////////////////////////////////////////////////

/**
*  Method: getModemStatus()
*  @ Since v0.1.0 by Eric Burger, April 2014
*  @ Returns value of packet modem status (Packet Index 4, Frame Index 1)
*/
uint8_t SimpleIncomingZigBeePacket::getModemStatus(){
	return getFrameData(1);
}

/*//////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
								SimpleOutgoingZigBeePacket Class
////////////////////////////////////////////////////////////////////////
/*//////////////////////////////////////////////////////////////////////

/*//////////////////////////////////////////////////////////////////////
											INITIALIZATION METHODS
/*//////////////////////////////////////////////////////////////////////

/**
*  Constructor: SimpleOutgoingZigBeePacket()
*  @ Since v0.1.0 by Eric Burger, August 2013
*  @ Default constructor that creates packet object for outgoing packets.
*/
SimpleOutgoingZigBeePacket::SimpleOutgoingZigBeePacket() : SimpleZigBeePacket(){
	SimpleZigBeePacket::init();
}

/**
*  Constructor: SimpleZigBeeRadio()
*  @ Since v0.1.0 by Eric Burger, August 2013
*  @ Constructor with inputs that creates packet object for outgoing packets.
*  @ param int maxFrameLength: Limit (# of bytes) on packet frame size 
*/
SimpleOutgoingZigBeePacket::SimpleOutgoingZigBeePacket(int maxFrameLength) : SimpleZigBeePacket(maxFrameLength) {
	SimpleZigBeePacket::init();
}

/*//////////////////////////////////////////////////////////////////////
										GENERAL PACKET METHODS
/*//////////////////////////////////////////////////////////////////////

/**
*  Method: setFrameType(uint8_t frameType)
*  @ Since v0.1.0 by Eric Burger, August 2013
*  @ Sets Frame Type (Packet Index 3, Frame Index 0). Method does not check if
*  @ input is a valid ZigBee frame type.
*  @ param uint8_t frameType: Frame type value to store
*/
void SimpleOutgoingZigBeePacket::setFrameType(uint8_t frameType){
	setFrameData(0,frameType);
}

/**
*  Method: setFrameID(uint8_t id)
*  @ Since v0.1.0 by Eric Burger, August 2013
*  @ Sets frame ID (Packet index 4, Frame Index 1). If zero, no response or status will be sent.
*  @ param uint8_t id: Frame ID value to store 
*/
void SimpleOutgoingZigBeePacket::setFrameID(uint8_t id){
	setFrameData(1, id);
}

/**
*  Method: setAddress(SimpleZigBeeAddress address)
*  @ Since v0.1.0 by Eric Burger, August 2013
*  @ Set the 64-bit and 16-bit destination addresses
*  @ param SimpleZigBeeAddress address: Object containing 64-bit and 16-bit destination addresses
*/
void SimpleOutgoingZigBeePacket::setAddress(SimpleZigBeeAddress address){
	SimpleZigBeeAddress64 adr64 = address.getAddress64();
	SimpleZigBeeAddress16 adr16 = address.getAddress16();
	setAddress64( adr64.getAddressMSB(), adr64.getAddressLSB() );
	setAddress16( adr16.getAddress() );
}

/**
*  Method: setAddress(uint32_t adr64MSB, uint32_t adr64LSB, uint16_t adr16)
*  @ Since v0.1.0 by Eric Burger, August 2013
*  @ Set the 64-bit and 16-bit destination addresses
*  @ param uint32_t adr64MSB: Most significant bytes (1st half) of 64-bit address
*  @ param uint32_t adr64LSB: Least significant bytes (2nd half) of 64-bit address 
*  @ param uint16_t adr16: 16-bit destination address
*/
void SimpleOutgoingZigBeePacket::setAddress(uint32_t adr64MSB, uint32_t adr64LSB, uint16_t adr16){
	setAddress64( adr64MSB, adr64LSB );
	setAddress16( adr16 );
}

/**
*  Method: setAddress64(uint32_t adr64MSB, uint32_t adr64LSB)
*  @ Since v0.1.0 by Eric Burger, August 2013
*  @ Set the 64-bit destination address (Packet Index 5 to 12, Frame Index 2 to 9)
*  @ param uint32_t adr64MSB: Most significant bytes (1st half) of 64-bit address
*  @ param uint32_t adr64LSB: Least significant bytes (2nd half) of 64-bit address 
*/
void SimpleOutgoingZigBeePacket::setAddress64(uint32_t adr64MSB, uint32_t adr64LSB){
	uint8_t startIndex = 2; // Frame data index marking start of 64-bit address.
	for(uint8_t i=0;i<4;i++){
		uint8_t by = (8*(3-i));
		uint8_t byte1 = (adr64MSB >> by) & 0xff;
		uint8_t byte2 = (adr64LSB >> by) & 0xff;
		setFrameData( startIndex+i, byte1);
		setFrameData( startIndex+i+4, byte2);
	}
}

/**
*  Method: setAddress16(uint16_t adr16)
*  @ Since v0.1.0 by Eric Burger, August 2013
*  @ Set the 16-bit destination address. (Packet Index 13 and 14, Frame Index 10 and 11)
*  @ param uint16_t adr16:  16-bit destination address  
*/
void SimpleOutgoingZigBeePacket::setAddress16(uint16_t adr16){
	setFrameData( 10, ((adr16 >> 8) & 0xff) );
	setFrameData( 11, (adr16 & 0xff) );
}

/*//////////////////////////////////////////////////////////////////////
								ZIGBEE TRANSMIT (TX) REQUEST METHODS
/*//////////////////////////////////////////////////////////////////////

/**
*  Method: setTXRequestBroadcastRadius(uint8_t rad)
*  @ Since v0.1.0 by Eric Burger, August 2013
*  @ Set broadcast radius. (Packet Index 15, Frame Index 12)
*  @ param uint8_t rad: Maximum radius (# of hops) of packets, 0 for no limit
*/
void SimpleOutgoingZigBeePacket::setTXRequestBroadcastRadius(uint8_t rad){
	setFrameData( 12, rad );
}

/**
*  Method: setTXRequestOption(uint8_t opt)
*  @ Since v0.1.0 by Eric Burger, August 2013
*  @ Set frame option. (Packet Index 16, Frame Index 13)
*  @ param uint8_t opt: Option value to set 
*/
void SimpleOutgoingZigBeePacket::setTXRequestOption(uint8_t opt){
	setFrameData( 13, opt );
}

/**
*  Method: setTXRequestPayload(uint8_t* payload, int payloadSize)
*  @ Since v0.1.0 by Eric Burger, August 2013
*  @ Store payload array. (Start Packet Index 17, Frame Index 14)
*  @ param uint8_t* payload: Pointer to array of bytes containing payload
*  @ param int payloadSize: Length of payload array
*/
void SimpleOutgoingZigBeePacket::setTXRequestPayload(uint8_t* payload, int payloadSize){
	setFrameData( 14, payload, payloadSize );
}

/*//////////////////////////////////////////////////////////////////////
									AT COMMAND METHODS
/*//////////////////////////////////////////////////////////////////////

/**
*  Method: setATCommand(uint16_t command)
*  @ Since v0.1.0 by Eric Burger, April 2014
*  @ Store AT command. (Start Packet Index 5, Frame Index 2)
*  @ param uint16_t command: 16-bit AT Command
*/
void SimpleOutgoingZigBeePacket::setATCommand(uint16_t command){
	setFrameData( 2, ((command >> 8) & 0xff) );
	setFrameData( 3, (command & 0xff) );
}

/**
*  Method: setATCommandPayload(uint8_t payload)
*  @ Since v0.1.0 by Eric Burger, July 2014
*  @ Store AT command parameter. (Start Packet Index 7, Frame Index 4)
*  @ param uint8_t payload: Byte containing payload
*/
void SimpleOutgoingZigBeePacket::setATCommandPayload(uint8_t payload){
	setFrameData( 4, payload );
}

/**
*  Method: setATCommandPayload(uint8_t* payload, int payloadSize)
*  @ Since v0.1.0 by Eric Burger, July 2014
*  @ Store AT command parameter. (Start Packet Index 7, Frame Index 4)
*  @ param uint8_t* payload: Pointer to array of bytes containing payload
*  @ param int payloadSize: Length of payload array
*/
void SimpleOutgoingZigBeePacket::setATCommandPayload(uint8_t* payload, int payloadSize){
	setFrameData( 4, payload, payloadSize );
}

/*//////////////////////////////////////////////////////////////////////
										REMOTE AT COMMAND METHODS
/*//////////////////////////////////////////////////////////////////////
/**
*  Method: setRemoteATCommandOption(uint8_t opt)
*  @ Since v0.1.0 by Eric Burger, July 2014
*  @ Set remote command option. (Packet Index 15, Frame Index 12)
*  @ param uint8_t opt: Option value to set 
*/
void SimpleOutgoingZigBeePacket::setRemoteATCommandOption(uint8_t opt){
	setFrameData( 12, opt );
}
/**
*  Method: setRemoteATCommand(uint16_t command)
*  @ Since v0.1.0 by Eric Burger, April 2014
*  @ Store Remote AT command. (Start Packet Index 16, Frame Index 13)
*  @ param uint16_t command: 16-bit AT Command
*/
void SimpleOutgoingZigBeePacket::setRemoteATCommand(uint16_t command){
	setFrameData( 13, ((command >> 8) & 0xff) );
	setFrameData( 14, (command & 0xff) );
}

/**
*  Method: setRemoteATCommandPayload(uint8_t payload)
*  @ Since v0.1.0 by Eric Burger, July 2014
*  @ Store AT command parameter. (Start Packet Index 18, Frame Index 15)
*  @ param uint8_t payload: Byte containing payload
*/
void SimpleOutgoingZigBeePacket::setRemoteATCommandPayload(uint8_t payload){
	setFrameData( 15, payload );
}

/**
*  Method: setRemoteATCommandPayload(uint8_t* payload, int payloadSize)
*  @ Since v0.1.0 by Eric Burger, July 2014
*  @ Store AT command parameter. (Start Packet Index 18, Frame Index 15)
*  @ param uint8_t* payload: Pointer to array of bytes containing payload
*  @ param int payloadSize: Length of payload array
*/
void SimpleOutgoingZigBeePacket::setRemoteATCommandPayload(uint8_t* payload, int payloadSize){
	setFrameData( 15, payload, payloadSize );
}

