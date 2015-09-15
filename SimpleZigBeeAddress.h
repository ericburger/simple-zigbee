/**
 * Library Name: SimpleZigBeeAddress
 * Library URI: 
 * Description: Classes for managing ZigBee addresses
 * by the connected radio. 
 * Version: 0.1.0
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

#ifndef SimpleZigBeeAddress_h
#define SimpleZigBeeAddress_h
 
#include "Arduino.h"
// Required for uint8_t type
#include <inttypes.h>

// ZigBee 64-bit Broadcast Address
#define BROADCAST_ADDRESS_64_MSB 0x00000000
#define BROADCAST_ADDRESS_64_LSB 0x0000ffff
// ZigBee 64-bit Unknown/Broadcast Address
#define COORDINATOR_ADDRESS_64_MSB 0x00000000
#define COORDINATOR_ADDRESS_64_LSB 0x00000000
// ZigBee 16-bit Unknown/Broadcast Address
#define BROADCAST_ADDRESS_16 0xfffe

/**
 * Class: SimpleZigBeeAddress64
 * @ Since v0.1.0 by Eric Burger, August 2013
 * @ Object for storing ZigBee 64-bit address
 */
class SimpleZigBeeAddress64 {
public:
  // INITIALIZATION METHODS //
  SimpleZigBeeAddress64();
  SimpleZigBeeAddress64(uint32_t adr64MSB, uint32_t adr64LSB);
  
  // PRIVATE VARIABLE METHODS //
  void setAddress(uint32_t adr64MSB, uint32_t adr64LSB);
  uint32_t getAddressMSB();
  uint32_t getAddressLSB();
  
private:
  // Most significant bytes (1st half) of 64-bit address
  uint32_t _address64MSB;
  // Least significant bytes (2nd half) of 64-bit address
  uint32_t _address64LSB;
};

/**
 * Class: SimpleZigBeeAddress64
 * @ Since v0.1.0 by Eric Burger, August 2013
 * @ Object for storing ZigBee 16-bit address
 */
class SimpleZigBeeAddress16 {
public:
  // INITIALIZATION METHODS //
  SimpleZigBeeAddress16();
  SimpleZigBeeAddress16(uint16_t adr16);
  
  // PRIVATE VARIABLE METHODS //
  void setAddress(uint16_t adr16);
  uint16_t getAddress();
  
private:
  // 16-bit address
  uint16_t _address16;
};

/**
 * Class: SimpleZigBeeAddress64
 * @ Since v0.1.0 by Eric Burger, August 2013
 * @ Object for storing ZigBee 64-bit and 16-bit addresses
 */
class SimpleZigBeeAddress {
public:
  // INITIALIZATION METHODS //
  SimpleZigBeeAddress();
  SimpleZigBeeAddress(SimpleZigBeeAddress64 adr64);
  SimpleZigBeeAddress(SimpleZigBeeAddress64 adr64, SimpleZigBeeAddress16 adr16);
  SimpleZigBeeAddress(uint32_t adr64MSB, uint32_t adr64LSB);
  SimpleZigBeeAddress(uint32_t adr64MSB, uint32_t adr64LSB, uint16_t adr16);
  
    // PRIVATE VARIABLE METHODS //  
  SimpleZigBeeAddress64 getAddress64();
  void setAddress64(SimpleZigBeeAddress64 adr64);
  void setAddress64(uint32_t adr64MSB, uint32_t adr64LSB);    
  SimpleZigBeeAddress16 getAddress16();
  void setAddress16(SimpleZigBeeAddress16 adr16);
  void setAddress16(uint16_t adr16);
  
private:
  SimpleZigBeeAddress64 _address64;
  SimpleZigBeeAddress16 _address16;
};

#endif //SimpleZigBeeAddress
