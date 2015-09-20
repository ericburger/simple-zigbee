/* 
   Getting Started, Part 1: Router 
  
   This example will show you how to setup a router and
   send a packet to the network coordinator using the
   SimpleZigBee library. You will need to complete
   "Getting Started, Part 1: Coordinator" before 
   completing this example.
  
   ###########################################################
   created 30 June 2014
   updated 16 Sept 2015
   by Eric Burger
   
   This example code is in the public domain.
   The SimpleZigBee library is released under the GNU GPL v2 License
   ###########################################################
   
   Setup:
   1. Complete "Getting Started, Part 1: Coordinator".
   
   2. This code will be used to interact with a router. 
   Use the XCTU Software to load the Router API firmware 
   onto a second XBee S2 radio. Set the PAN ID to whatever you
   used in the previous example (Getting Started: Part 1a) or
   leave the value as 0 and the router will connect to any
   network. Lastly, make sure the API Mode is set to 2 (AP=2).
   
   3. In this example, the Arduino will connect to the XBee
   using a Software Serial port. This allows the Hardware
   Serial port to remain free, making it easier to reprogram the 
   Arduino and to debug the code. Begin by connecting the XBee. 
   Connect DOUT to Pin 10 (RX) and DIN to Pin 11 (TX). Also,
   connect the XBee to 3.3V and ground (GND).
   
   4. Upload this sketch (to a second Arduino attached to the
   Router) and open the Arduino IDE's Serial Monitor. Read 
   through the commented code below to understand what is
   being displayed in the serial monitor.
   
   5. If you are having trouble getting the router to connect to
   the coordinator, try reseting both XBees to the default
   settings using XCTU. Also, you can try sending the AT Command
   CB4 to leave the network (see setup() below).
   
   6. The next "Getting Started" examples will provide more
   detail about how to setup a message and to interpret what
   is being received.
   
 */

  #include <SimpleZigBeeRadio.h>
  #include <SoftwareSerial.h>

  // Create the XBee object ...
  SimpleZigBeeRadio xbee = SimpleZigBeeRadio();
  // ... and the software serial port. Note: Only one
  // SoftwareSerial object can receive data at a time.
  SoftwareSerial xbeeSerial(10, 11); // (RX=>DOUT, TX=>DIN)
  
  // Packet to send: In this example, we will update
  // the contents of a packet before sending it.
  SimpleZigBeePacket zbp = SimpleZigBeePacket();
  
  // Value and payload to be sent
  int val = 0;
  // Variables to store time
  unsigned long time = 0;
  unsigned long last_sent = 0;
      
  void setup() {
    // Start the serial ports ...
    Serial.begin( 9600 );
    while( !Serial ){;}// Wait for serial port (for Leonardo only). 
    xbeeSerial.begin( 9600 );
    // ... and set the serial port for the XBee radio.
    xbee.setSerial( xbeeSerial );
    // Receive TX Status packets
    xbee.setAcknowledgement(true);
    
    // The frame data in a ZigBee packet refers to the data between 
    // the length LSB and the checksum. Below is an example of
    // a frame where:
    // Frame type = 0x10 (ZigBee Transmit Request)
    // Frame id = 0x01
    // 64-bit destination address = 0x0000000000000000 (Coordinator address)
    // 16-bit destination address = 0xfffe (unknown or broadcast)
    // Broadcast radius = 0x00
    // Options = 0x00
    // Payload data = 0xffff
    uint8_t exFrame[] = { 0x10,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xff,0xfe,0x00,0x00,0xff,0xff };
    
    // Now store the example frame in the packet object. 
    // The setFrameData() method accepts a starting index
    // (where in the frame to begin), an array, and the 
    // array's length (number of bytes to store).
    zbp.setFrameData(0, exFrame, sizeof(exFrame));
   
    // If you are having trouble connecting the Router
    // to the Coordinator, you can try sending the
    // AT command to leave the current network and then 
    // wait for the Router to reconnect.
    /*
    Serial.println( "Send command to leave network (CB4)" );
    xbee.prepareATCommand('CB',4);
    xbee.send();
    delay(10000);
    */
  }
  
  
  void loop() {
    // While data is waiting in the XBee serial port ...
    while( xbee.available() ){
      // ... read the data.
      xbee.read();
      // If a complete message is available and it
      // has not been read yet, read it.
      if( xbee.isComplete() ){
        // Print the contents of the incoming packet
        Serial.print("\nIncoming Message: ");
        printPacket( xbee.getIncomingPacketObject() );
      }
    }
    
    // The Arduino will send a packet to the XBee once every 5 
    // seconds. To avoid overflowing the serial buffer, you 
    // should avoid putting long delays, like delay(5000),
    // in a sketch.
    time = millis();
    if( time > (last_sent+5000) ){
      last_sent = time; // Update the last_sent variable
      // Update the payload (in this case, the last 2 bytes of the frame)
      // The rest of the frame (address, etc) does not need to be changed.
      // Note: The max value of a byte is 255. Therefore, larger values,
      // like ints, must be broken into bytes.
      zbp.setFrameData( zbp.getFrameLength()-2, val >> 8 & 0xff );
      zbp.setFrameData( zbp.getFrameLength()-1, val & 0xff );
      Serial.print("\nSend Message: ");
      printPacket( zbp );
      // Send the packet. This example does not use the outgoing packet
      // object contained in the SimpleZigBeeRadio class.
      xbee.send( zbp );
     
      val = (val + 10)%500; // Increase val by 10 (start over at 500)
    }
    
    delay(10); // Small delay for stability
    // That's it! The router is ready to go.
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
