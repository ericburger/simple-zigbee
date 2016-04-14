/* 
   SimpleZigBeePacket class example
  
   This sketch demonstrates how to use the 
   SimpleZigBeePacket class to store data that has 
   been received from or will be sent to an XBee
   radio. The SimpleZigBeeAddress class does not
   interact with the XBee radio and thus it is 
   not necessary to connect a radio to run 
   this example code.
  
   created 27 June 2014
   by Eric Burger
   
   This example code is in the public domain.
  
 */

  #include <SimpleZigBeePacket.h>
  // Create a packet object. When you create a packet,
  // the Arduino allocates 4 bytes of memory to store
  // the frame data. As you add to the content in the 
  // packet, the memory allocation is expanded, as 
  // necessary, up to a maximum of 50 bytes (default max
  // packet frame length is 50). Some of the frame data, 
  // such as the start delimiter, packet length, and checksum,
  // are not stored in the memory. Therefore, the actual 
  // packet length will be larger than the frame length.
  SimpleZigBeePacket zbpOne = SimpleZigBeePacket();

  // The max memory allocation can be changed by passing
  // an int to the constructor. However, note that the 
  // Arduino serial port buffer is 64 bytes. Therefore, 
  // you may be able to send larger packets but you will
  // not be able to receive them.
  SimpleZigBeePacket zbpTwo = SimpleZigBeePacket(40);

  // Below is an example of a frame where:
  // Frame type = 0x10 (ZigBee Transmit Request)
  // Frame id = 0x01
  // 64-bit destination address = 0x0000000000000000 (Coordinator address)
  // 16-bit destination address = 0xfffe (unknown or broadcast)
  // Broadcast radius = 0x00
  // Options = 0x00
  // Payload data = 0xff
  uint8_t exFrame[] = { 0x10,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xff,0xfe,0x00,0x00,0xff };
  
  void setup() {
    Serial.begin( 9600 );
    while( !Serial ){;}// Wait to connect. Needed for Leonardo only
    
    // Now store the example frame in a packet object. 
    // The setFrameData() method accepts a starting index
    // (where to start in the packet memory), an array,  
    // and the array's length (number of bytes to store).
    zbpOne.setFrameData( 0, exFrame, sizeof(exFrame));
    
    // Below are several examples of methods for
    // recalling information stored in the packet...
    Serial.print("Frame Type: ");
    Serial.println( zbpOne.getFrameType(), HEX );
    Serial.print("Frame ID: ");
    Serial.println( zbpOne.getFrameID(), HEX );
    
    // ...and for getting non-frame information.
    Serial.print("Frame Length: ");
    Serial.println( zbpOne.getFrameLength() );
    Serial.print("Checksum: ");
    Serial.println( zbpOne.calculateChecksum(), HEX );
    Serial.println();
    
    // Here is a functions for printing the entire 
    // packet (frame + non-frame data).
    Serial.print("Packet One: ");
    printPacket(zbpOne);
    
    // You can also get and set a specific index of
    // the frame data.
    Serial.print("Frame Payload: ");
    Serial.println( zbpOne.getFrameData( zbpOne.getFrameLength()-1 ), HEX );
    Serial.println("Change Payload... ");
    uint8_t newPayload = 0xaa; 
    zbpOne.setFrameData( zbpOne.getFrameLength()-1 , newPayload );
    Serial.println("Change Frame ID... ");
    zbpOne.setFrameData( 1, 0x05 );
    Serial.print("Packet One: ");
    printPacket(zbpOne);
    Serial.println();
    
    // Last, the SimpleZigBeePacket class can report 
    // if an error has occur.
    // The Error Codes are:
    // NO_ERROR = 0
    // ERROR_REALLOCATING_MEMORY = 1
    // UNEXPECTED_PACKET_START = 2
    // PACKET_INCOMPLETE = 3
    // MAX_FRAME_LENGTH_EXCEEDED = 4
    // FRAME_LENGTH_EXCEEDED = 5
    // CHECKSUM_FAILURE = 6
    
    // For example, if we try to access data that 
    // is outside the frame,
    zbpOne.getFrameData( zbpOne.getFrameLength()+5  );
    // we can see that an error will be reported.
    Serial.print("Error Check: ");
    Serial.println( zbpOne.isError());
    Serial.print("Error Code: ");
    Serial.println( zbpOne.getErrorCode() );
    
    Serial.println();
    Serial.println();
    Serial.println( "End of Demo" );
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
