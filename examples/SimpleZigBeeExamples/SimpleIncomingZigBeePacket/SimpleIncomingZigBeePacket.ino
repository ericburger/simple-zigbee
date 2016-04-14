/* 
   SimpleIncomingZigBeePacket class example
  
   This sketch demonstrates how to use the 
   SimpleIncomingZigBeePacket class to store 
   data that has been received from an XBee
   radio. The SimpleIncomingZigBeePacket class
   does not interact with the XBee radio and 
   thus it is not necessary to connect
   a radio to run this example code.
  
   created 27 June 2014
   by Eric Burger
   
   This example code is in the public domain.
  
 */

  #include <SimpleZigBeePacket.h>
  // Create an incoming packet object. The 
  // SimpleIncomingZigBeePacket class inherits
  // from the SimpleZigBeePacket class and 
  // provides useful methods relevant to
  // receiving a message.
  SimpleIncomingZigBeePacket zbp = SimpleIncomingZigBeePacket();  
  
  void setup() {
    Serial.begin( 9600 );
    while( !Serial ){;}// Wait to connect. Needed for Leonardo only
    
    // ################################################### //
    // ################################################### //
    
    // For the sake of example, we will pretend that the 
    // following data has been successfully received and
    // stored in the packet object
    
    // *** CHANGE THIS TO TEST CODE *** //
    int ex = 0; // Change ex to 0, 1, 2, or 3 to try a different example

    if( 0 == ex ){ 
      uint8_t exFrame[] = { 0x90,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xff,0xfe,0x00,0x00,0xf1,0xf2,0xf3 };
      zbp.setFrameData(0, exFrame, sizeof(exFrame));
    }else if( 1 == ex ){ 
      uint8_t exFrame[] = { 0x88,0x01,0x4d,0x59,0x00,0xff,0xfe};
      zbp.setFrameData(0, exFrame, sizeof(exFrame));
    }else if( 2 == ex ){ 
      uint8_t exFrame[] = { 0x8b,0x01,0x7d,0x84,0x00,0x00,0x01};
      zbp.setFrameData(0, exFrame, sizeof(exFrame));
    }else if( 3 == ex ){ 
      uint8_t exFrame[] = { 0x8a,0x02};
      zbp.setFrameData(0, exFrame, sizeof(exFrame));
    }
    zbp.setChecksum( zbp.calculateChecksum() );
    
    // ################################################### //
    // ################################################### //
    
    // We can use a number of methods to figure out what 
    // is contained in the packet. 
    Serial.print("Example Packet: ");
    printPacket( zbp );
    
    // Check the packet type. Currenlty, the recognized types 
    // are ZigBee Received RX (0x90), AT Command Response (0x88), 
    // ZigBee Transmit TX Status (0x8b), and Modem Status (0x8a)
    uint8_t frameType = zbp.getFrameType();
    Serial.print("Frame Type: ");
    Serial.println( frameType, HEX );
    if( frameType == ZIGBEE_RECIEVED_PACKET ){
      
      Serial.println( "ZIGBEE_RECIEVED_PACKET" );
      
      // Methods specific to RX packets are...
      SimpleZigBeeAddress addr = zbp.getRXAddress(); // Get the 64-bit and 16-bit address
      uint8_t receiveOptions = zbp.getRXOptions(); 
      uint8_t payloadLength = zbp.getRXPayloadLength();
      int index = payloadLength - 1;
      uint8_t last = zbp.getRXPayload(index); // Search the payload by index
      
      Serial.print("Payload Length: ");
      Serial.println( payloadLength );
      Serial.print("Last Byte of Payload: ");
      Serial.println( last, HEX);
      
    }else if( frameType == AT_COMMAND_RESPONSE ){
      
      Serial.println( "AT_COMMAND_RESPONSE" );

      // Methods specific to AT Command Response packets are...
      uint16_t atCmd = zbp.getATResponseCommand();
      uint8_t atStat = zbp.getATResponseStatus();
      uint8_t atLength = zbp.getATResponsePayloadLength();
      int index = atLength - 1;
      uint8_t last = zbp.getATResponsePayload(index);
      
      Serial.print("AT Command: ");
      Serial.print( atCmd, HEX );
      // Convert command to characters...
      Serial.print( " (" );
      Serial.print( char((atCmd >> 8) & 0xff) );
      Serial.print( char((atCmd) & 0xff) ); 
      Serial.println( ")" );
      
      Serial.print("Command Data Length: ");
      Serial.println( atLength );
      Serial.print("Last Byte of Command Data: ");
      Serial.println( last, HEX);
      
    }else if( frameType == ZIGBEE_TX_STATUS ){
      
      Serial.println( "ZIGBEE_TX_STATUS" );

      // Methods specific to TX Status packets are...
      SimpleZigBeeAddress16 addr16 = zbp.getTXStatusAddress16(); 
      uint8_t txRetry = zbp.getTXStatusRetryCount();
      uint8_t txStat = zbp.getTXStatusDeliveryStatus();
      uint8_t txDscovery = zbp.getTXStatusDiscoveryStatus();

      Serial.print("Transmit Status ( >0 means failure ): ");
      Serial.println( txStat );
      Serial.print("Number of retries: ");
      Serial.println( txRetry );
      
    }else if( frameType == MODEM_STATUS ){
      
      Serial.println( "MODEM_STATUS" );

      // Methods specific to Modem Statuc packets are...
      uint8_t modemStat = zbp.getModemStatus();
      Serial.print("Status: ");
      Serial.println( modemStat );
      
    }else{
      // Unknown or unimplemented frame type
      Serial.println( "Unknown Frame Type" );
    }
  }
  
  
  void loop() {
    // Do nothing
    delay(1000);
  }
  
  /////////////////////////////////////////////////////////////
  // Function for printing the complete contents of a packet //
  /////////////////////////////////////////////////////////////
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
