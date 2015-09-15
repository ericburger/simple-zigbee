/**
 * Copyright (c) 2013 Eric Burger. All rights reserved.
 */
 
#include "SimpleZigBeeAddress.h"

/*//////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
                        SimpleZigBeeAddress64 Class
////////////////////////////////////////////////////////////////////////
/*//////////////////////////////////////////////////////////////////////

/*//////////////////////////////////////////////////////////////////////
                       INITIALIZATION METHODS
/*//////////////////////////////////////////////////////////////////////

/**
 *  Constructor: SimpleZigBeeAddress64()
 *  @ Since v0.1.0 by Eric Burger, August 2013
 *  @ Default constructor for ZigBee 64-bit address. Default to broadcast address.
 */
SimpleZigBeeAddress64::SimpleZigBeeAddress64(){
  setAddress( BROADCAST_ADDRESS_64_MSB, BROADCAST_ADDRESS_64_LSB );
}

/**
 *  Constructor: SimpleZigBeeAddress64()
 *  @ Since v0.1.0 by Eric Burger, August 2013
 *  @ Constructor with input address for ZigBee 64-bit address.
 *  @ param uint32_t adr64MSB: Most significant bytes (1st half) of 64-bit address
 *  @ param uint32_t adr64LSB: Least significant bytes (2nd half) of 64-bit address 
 */
SimpleZigBeeAddress64::SimpleZigBeeAddress64(uint32_t adr64MSB, uint32_t adr64LSB){
  setAddress( adr64MSB, adr64LSB );
}

/*//////////////////////////////////////////////////////////////////////
                    PRIVATE VARIABLE METHODS
/*//////////////////////////////////////////////////////////////////////

/**
 *  Method: setAddress(uint32_t adr64MSB, uint32_t adr64LSB)
 *  @ Since v0.1.0 by Eric Burger, August 2013
 *  @ Set 64-bit address
 *  @ param uint32_t adr64MSB: Most significant bytes (1st half) of 64-bit address
 *  @ param uint32_t adr64LSB: Least significant bytes (2nd half) of 64-bit address  
 */
void SimpleZigBeeAddress64::setAddress(uint32_t adr64MSB, uint32_t adr64LSB){
  _address64MSB = adr64MSB;
  _address64LSB = adr64LSB;
}

/**
 *  Method: getAddressMSB()
 *  @ Since v0.1.0 by Eric Burger, August 2013
 *  @ Returns MSB of 64-bit address
 */
uint32_t SimpleZigBeeAddress64::getAddressMSB(){
  return _address64MSB;
}

/**
 *  Method: getAddressLSB()
 *  @ Since v0.1.0 by Eric Burger, August 2013
 *  @ Returns LSB of 64-bit address
 */
uint32_t SimpleZigBeeAddress64::getAddressLSB(){
  return _address64LSB;
}

/*//////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
                      SimpleZigBeeAddress16 Class
////////////////////////////////////////////////////////////////////////
/*//////////////////////////////////////////////////////////////////////

/*//////////////////////////////////////////////////////////////////////
                       INITIALIZATION METHODS
/*//////////////////////////////////////////////////////////////////////
/**
 *  Constructor: SimpleZigBeeAddress16()
 *  @ Since v0.1.0 by Eric Burger, August 2013
 *  @ Default constructor for ZigBee 16-bit address. Default to broadcast address.
 */
SimpleZigBeeAddress16::SimpleZigBeeAddress16(){
  setAddress( BROADCAST_ADDRESS_16 );
}

/**
 *  Constructor: SimpleZigBeeAddress64()
 *  @ Since v0.1.0 by Eric Burger, August 2013
 *  @ Constructor with input address for ZigBee 16-bit address.
 *  @ param uint16_t adr16: 16-bit destination address
 */
SimpleZigBeeAddress16::SimpleZigBeeAddress16(uint16_t adr16){
  setAddress( adr16 );
}

/*//////////////////////////////////////////////////////////////////////
                   PRIVATE VARIABLE METHODS
/*//////////////////////////////////////////////////////////////////////

/**
 *  Method: setAddress(uint16_t adr16)
 *  @ Since v0.1.0 by Eric Burger, August 2013
 *  @ Set 16-bit address
 *  @ param uint16_t adr16: 16-bit destination address
 */
void SimpleZigBeeAddress16::setAddress(uint16_t adr16){
  _address16 = adr16;
}

/**
 *  Method: getAddress()
 *  @ Since v0.1.0 by Eric Burger, August 2013
 *  @ Returns 16-bit address
 */
uint16_t SimpleZigBeeAddress16::getAddress(){
  return _address16;
}

/*//////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
                      SimpleZigBeeAddress Class
////////////////////////////////////////////////////////////////////////
/*//////////////////////////////////////////////////////////////////////

/*//////////////////////////////////////////////////////////////////////
                       INITIALIZATION METHODS
/*//////////////////////////////////////////////////////////////////////

/**
 *  Constructor: SimpleZigBeeAddress()
 *  @ Since v0.1.0 by Eric Burger, August 2013
 *  @ Default constructor for ZigBee address object. Default to 64-bit and 16-bit broadcast address.
 */
SimpleZigBeeAddress::SimpleZigBeeAddress(){
  _address64 = SimpleZigBeeAddress64();
  _address16 = SimpleZigBeeAddress16();
}

/**
 *  Constructor: SimpleZigBeeAddress()
 *  @ Since v0.1.0 by Eric Burger, July 2014
 *  @ Constructor with 64-bit input address for ZigBee address object. Default to 16-bit broadcast address.
 *  @ param SimpleZigBeeAddress64 adr64: Object containing 64-bit address 
 */
SimpleZigBeeAddress::SimpleZigBeeAddress(SimpleZigBeeAddress64 adr64){
  _address64 = adr64;
  _address16 = SimpleZigBeeAddress16();
}

/**
 *  Constructor: SimpleZigBeeAddress()
 *  @ Since v0.1.0 by Eric Burger, July 2014
 *  @ Constructor with 64-bit and 16-bit input addresses for ZigBee address object.
 *  @ param SimpleZigBeeAddress64 adr64: Object containing 64-bit address 
 *  @ param SimpleZigBeeAddress16 adr16: Object containing 16-bit address 
 */
SimpleZigBeeAddress::SimpleZigBeeAddress(SimpleZigBeeAddress64 adr64, SimpleZigBeeAddress16 adr16){
  _address64 = adr64;
  _address16 = adr16;
}

/**
 *  Constructor: SimpleZigBeeAddress()
 *  @ Since v0.1.0 by Eric Burger, August 2013
 *  @ Constructor with 64-bit input address for ZigBee address object. Default to 16-bit broadcast address.
 *  @ param uint32_t adr64MSB: Most significant bytes (1st half) of 64-bit address
 *  @ param uint32_t adr64LSB: Least significant bytes (2nd half) of 64-bit address 
 */
SimpleZigBeeAddress::SimpleZigBeeAddress(uint32_t adr64MSB, uint32_t adr64LSB){
  _address64 = SimpleZigBeeAddress64(adr64MSB,adr64LSB);
  _address16 = SimpleZigBeeAddress16();
}

/**
 *  Constructor: SimpleZigBeeAddress()
 *  @ Since v0.1.0 by Eric Burger, August 2013
 *  @ Constructor with 64-bit and 16-bit input addresses for ZigBee address object.
 *  @ param uint32_t adr64MSB: Most significant bytes (1st half) of 64-bit address
 *  @ param uint32_t adr64LSB: Least significant bytes (2nd half) of 64-bit address 
 *  @ param uint16_t adr16: 16-bit address 
 */
SimpleZigBeeAddress::SimpleZigBeeAddress(uint32_t adr64MSB, uint32_t adr64LSB, uint16_t adr16){
  _address64 = SimpleZigBeeAddress64(adr64MSB,adr64LSB);
  _address16 = SimpleZigBeeAddress16(adr16);
}

/*//////////////////////////////////////////////////////////////////////
                    PRIVATE VARIABLE METHODS
/*//////////////////////////////////////////////////////////////////////

/**
 *  Method: getAddress64()
 *  @ Since v0.1.0 by Eric Burger, August 2013
 *  @ Returns 64-bit address object
 */
SimpleZigBeeAddress64 SimpleZigBeeAddress::getAddress64(){
  return _address64;
}

/**
 *  Method: setAddress64(SimpleZigBeeAddress64 adr64)
 *  @ Since v0.1.0 by Eric Burger, August 2013
 *  @ Sets 64-bit address
 *  @ param SimpleZigBeeAddress64 adr64: Object containing 64-bit address 
 */
void SimpleZigBeeAddress::setAddress64(SimpleZigBeeAddress64 adr64){
  _address64 = adr64;
}

/**
 *  Method: setAddress64(uint32_t adr64MSB, uint32_t adr64LSB)
 *  @ Since v0.1.0 by Eric Burger, August 2013
 *  @ Sets 64-bit address
 *  @ param uint32_t adr64MSB: Most significant bytes (1st half) of 64-bit address
 *  @ param uint32_t adr64LSB: Least significant bytes (2nd half) of 64-bit address 
 */
void SimpleZigBeeAddress::setAddress64(uint32_t adr64MSB, uint32_t adr64LSB){
  _address64.setAddress(adr64MSB,adr64LSB);
}

/**
 *  Method: getAddress16()
 *  @ Since v0.1.0 by Eric Burger, August 2013
 *  @ Returns 16-bit address object
 */
SimpleZigBeeAddress16 SimpleZigBeeAddress::getAddress16(){
  return _address16;
}

/**
 *  Method: setAddress16(SimpleZigBeeAddress16 adr16)
 *  @ Since v0.1.0 by Eric Burger, August 2013
 *  @ Sets 16-bit address
 *  @ param SimpleZigBeeAddress16 adr16: Object containing 16-bit address 
 */
void SimpleZigBeeAddress::setAddress16(SimpleZigBeeAddress16 adr16){
  _address16 = adr16;
}

/**
 *  Method: setAddress16(uint16_t adr16)
 *  @ Since v0.1.0 by Eric Burger, August 2013
 *  @ Sets 16-bit address
 *  @ param uint16_t adr16: 16-bit address 
 */
void SimpleZigBeeAddress::setAddress16(uint16_t adr16){
  _address16.setAddress( adr16 );
}
