/* 
   SimpleOutgoingZigBeePacket class example for a TX packet
  
   This sketch demonstrates how to use the 
   SimpleOutgoingZigBeePacket class to prepare a 
   transmit request (TX) packet that will be sent
   to an XBee radio. The SimpleOutgoingZigBeePacket 
   class does not interact with the XBee radio and 
   thus it is not necessary to connect a radio to
   run this example code.
  
   created 30 June 2014
   by Eric Burger
   
   This example code is in the public domain.
  
 */

  #include <SimpleZigBeePacket.h>
  // Create an outgoing packet object. The
  // SimpleOutgoingZigBeePacket class inherits
  // from the SimpleZigBeePacket class and 
  // provides useful methods relevant to 
  // preparing a message.
  SimpleOutgoingZigBeePacket zbp = SimpleOutgoingZigBeePacket();  
  
  void setup() {
    Serial.begin( 9600 );
    while( !Serial ){;}// Wait to connect. Needed for Leonardo only
      
    // To assemble an outgoing packet, 
    // we will first set the frame type...
    zbp.setFrameType(ZIGBEE_TRANSMIT_REQUEST); // Same as zbp.setFrameType(0x10)
    // ...and the frame id
    zbp.setFrameID(0x01);
    
    // Next, set the destination address 
    // (in this example, the coordinator address).
    // You can pass a SimpleZigBeeAddress object ...
    SimpleZigBeeAddress coorAddr = SimpleZigBeeAddress(0x00000000,0x00000000,0xfffe);
    zbp.setAddress(coorAddr);
    // ... or directly set the address. 
    zbp.setAddress(0x00000000,0x00000000,0xfffe);
    // Note: 0x00000000 is the same as 0. I have 
    // written 0x00000000 to show that the functions 
    // accept a 32-bit, 32-bit, and 16-bit value.
    
    // Next, set the Broadcast Radius and Option
    // (A radius of 0 means unlimited jumps).
    zbp.setTXRequestBroadcastRadius(0x00);
    zbp.setTXRequestOption(0x00);

    // Finally, set the payload.
    uint8_t exPayload[] = { 0xa1,0xa2,0xa3 };
    zbp.setTXRequestPayload( exPayload, sizeof(exPayload) ); // Same as zbp.setPayload( exPayload, 3 );
    
    // Now that the TX packet is complete, 
    // we will display the contents to the serial port.
    printPacket(zbp);
  }
  
  void loop() {
    // Do nothing
    delay(1000);
  }
  
  // Function for printing the complete contents of a packet
  void printPacket(SimpleZigBeePacket & p){
    Serial.print( START, HEX );
    Serial.print(' ');
    Serial.print( p.getLengthMSB(), HEX );
    Serial.print(' ');
    Serial.print( p.getLengthLSB(), HEX );
    Serial.print(' ');
    // Frame Type and Frame ID are stored in Frame Data
    uint8_t checksum = 0;
    for( int i=0; i<p.getFrameLength(); i++){
  	Serial.print( p.getFrameData(i), HEX );
        Serial.print(' ');
        checksum += p.getFrameData(i); 
    }
    // Calculate checksum based on summation of frame bytes
    checksum = 0xff - checksum;
    Serial.print(checksum, HEX );
    Serial.println();
  }
