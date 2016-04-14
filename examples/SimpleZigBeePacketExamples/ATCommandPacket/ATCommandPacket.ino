/* 
   SimpleOutgoingZigBeePacket class example 
   for an AT Command packet
  
   This sketch demonstrates how to use the 
   SimpleOutgoingZigBeePacket class to prepare
   an AT Command packet that will be sent to 
   an XBee radio. The SimpleOutgoingZigBeePacket 
   class does not interact with the XBee radio 
   and thus it is not necessary to connect 
   a radio to run this example code.
  
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
    zbp.setFrameType(AT_COMMAND); // Same as zbp.setFrameType(0x08)
    // ... and the frame id
    zbp.setFrameID(0x01);

    // *** CHANGE THIS TO TEST CODE *** //
    // Next, set the command. Below are 3 examples
    // of AT Commands. Change ex to 0, 1, or 2
    // to try a different example.
    int ex = 0;

    if( 0 == ex ){ 
      // The command MY (0x4d,0x59) requests 
      // the radio's 16-bit network address.
      zbp.setATCommand(0x4d59);
    }else if( 1 == ex ){ 
      // The command AP (0x41,0x50) sets the 
      // API Mode of the radio (1 or 2).
      zbp.setATCommand(0x4150);
      zbp.setATCommandPayload(0x02);
    }else if( 2 == ex ){ 
      // The command NI (0x4e,0x49) sets 
      // the Node Identifier string
      uint8_t nodeID[] = { 0x4e,0x6f,0x64,0x65,0x49,0x44 }; // "NodeID"
      zbp.setATCommand(0x4e49);
      zbp.setATCommandPayload(nodeID,sizeof(nodeID));
    }
    
    // Now that the AT Command packet is complete, 
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
