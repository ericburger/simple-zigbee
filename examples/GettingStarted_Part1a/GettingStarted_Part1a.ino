/* 
   Getting Started: Part 1a
   Coordinator Example 1
  
   Welcome to the SimpleZigBee library. This library
   is intented to make it easy to setup a network of 
   Arduinos using XBee S2 (ZigBee) radios. Please note 
   that the library will only work with ZigBee radios 
   (not XBee S1) and that the radios must be in API Mode 2
   (not in transparent mode).
   
   If this is your first introduction into wireless networks,
   I highly recommend the book "Building Wireless Sensor Networks"
   by Robert Faludi. When I first began working with sensor
   networks, I found the text very helpful for understanding
   hexidecimals, networking protocols in general, and
   specifically the ZigBee protocol. 
   
   These example sketchs will help you quickly get a
   wirless network of Arduinos up and running. To complete
   the example, you will need two XBee S2 radios (one with
   Coordinator API firmware and one with Router API firmware)
   and two Arduino boards.
   
   This example will setup the network coordinator and display 
   the contents of incoming packets.
  
   ###########################################################
   created 30 June 2014
   by Eric Burger
   
   This example code is in the public domain.
   The SimpleZigBee library is released under the GNU GPL v2 License
   ###########################################################
   
   Setup: 
   1. This code will be used to interact with the coordinator. 
   Use the XCTU Software to load the Coordinator API firmware 
   onto an XBee S2 radio. Next, set an easy to remember PAN ID 
   (like 1, 100, or 2020) or leave the value as 0 and a PAN ID
   will automatically be set. Lastly, make sure the API Mode
   is set to 2 (AP=2).
   
   2. In this example, the Arduino will connect to the XBee
   using a Software Serial port. This allows the Hardware
   Serial port to remain free, making it easier to reprogram the 
   Arduino and to debug the code. Begin by connecting the XBee. 
   Connect DOUT to Pin 10 (RX) and DIN to Pin 11 (TX). Also,
   connect the XBee to 3.3V and ground (GND).
   
   3. Upload this sketch (to the Arduino attached to the coordinator)
   and open the Arduino IDE's Serial Monitor. Read through the
   commented code below to understand what is being displayed
   in the serial monitor (though very little will occur until you
   complete "Getting Started: Part 1b").
   
   4. Complete "Getting Started: Part 1b".

 */

  #include <SimpleZigBeeRadio.h>
  #include <SoftwareSerial.h>

  // Create the XBee object ...
  SimpleZigBeeRadio xbee = SimpleZigBeeRadio();
  // ... and the software serial port. Note: Only one
  // SoftwareSerial object can receive data at a time.
  SoftwareSerial xbeeSerial(10, 11); // (RX=>DOUT, TX=>DIN)


  void setup() {
    // Start the serial ports ...
    Serial.begin( 9600 );
    while( !Serial ){;// Wait for serial port (for Leonardo only). 
    }
    xbeeSerial.begin( 9600 );
    // ... and set the serial port for the XBee radio.
    xbee.setSerial( xbeeSerial );
    // Set a non-zero frame id to receive Status and Response packets.
    xbee.setAcknowledgement(true); 
  }
  
  void loop() {
    // Each SimpleZigBeeRadio object contains two 
    // SimpleZigBeePacket objects, one for storing incoming
    // messages and one for preparing outgoing messages.
    // This example will not send any messages to the network.
    // Rather, the coordinator will receive messages and display
    // the contents to the hardware serial port.
    
    // If data is waiting in the XBee serial port ...
    if( xbee.available() ){
      // ... read the data.
      xbee.read();
      // If a complete message is available, display the contents
      if( xbee.isComplete() ){
        Serial.print("\nIncoming Message: ");
        printPacket( xbee.getIncomingPacketObject() );
        // While the SimpleZigBeeRadio class includes
        // methods for getting and setting the contents
        // of the incoming and outgoing packet objects,
        // you can always access the objects directly 
        // using:
        //SimpleIncomingZigBeePacket zbpIn = xbee.getIncomingPacketObject();
        //SimpleOutgoingZigBeePacket zbpOut = xbee.getOutgoingPacketObject();
      }
    }
    // Note: Because the .isComplete() check is inside the 
    // .available() loop, an incoming message will never 
    // pass the .isComplete() check more than one. The next
    // time the .available() loop is entered (i.e. a new message
    // arrives), the previous incoming message will be erased.
      
    delay(10); // Small delay for stability
    // That's it! The coordinator is ready to go.
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
