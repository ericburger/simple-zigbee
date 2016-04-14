/* 
   Getting Started, Part 2: Router
  
   This example will expand upon the previous Router sketch.
   (Getting Started, Part 1: Router). Added features include 
   setting the API Mode and PAN ID from the Arduino and using
   methods to change the contents of the outgoing packet. You
   will need to complete "Getting Started, Part 2: Coordinator"
   before completing this example.
  
   ###########################################################
   created 30 June 2014
   updated 16 Sept 2015
   by Eric Burger
   
   This example code is in the public domain.
   The SimpleZigBee library is released under the GNU GPL v2 License
   ###########################################################
   
   
   Setup (same as Getting Started, Part 1: Router):
   1. Complete "Getting Started, Part 2: Coordinator".
   
   2. Use the XCTU Software to load the Router API firmware 
   onto an XBee S2 radio.
   
   3. Connect DOUT to Pin 10 (RX) and DIN to Pin 11 (TX). Also,
   connect the XBee to 3.3V and ground (GND).
   
   4. Upload this sketch (to a second Arduino attached to the
   Router) and open the Arduino IDE's Serial Monitor. Read 
   through the commented code below to understand what is
   being displayed in the serial monitor.

 */

  #include <SimpleZigBeeRadio.h>
  #include <SoftwareSerial.h>

  // Create the XBee object ...
  SimpleZigBeeRadio xbee = SimpleZigBeeRadio();
  // ... and the software serial port. Note: Only one
  // SoftwareSerial object can receive data at a time.
  SoftwareSerial xbeeSerial(10, 11); // (RX=>DOUT, TX=>DIN)
  
  int val = 0;
  uint8_t payload[] = {0x00,0x00};
  unsigned long time = 0;
  unsigned long last_sent = 0;
      
  void setup() {
    // Start the serial ports ...
    Serial.begin( 9600 );
    while( !Serial ){;// Wait for serial port (for Leonardo only). 
    }
    xbeeSerial.begin( 9600 );
    // ... and set the serial port for the XBee radio.
    xbee.setSerial( xbeeSerial );
    // Receive TX Status packets
    xbee.setAcknowledgement(true);
    
    // To ensure that the radio is in API Mode 2 and is
    // operating on the correct PAN ID, you can use the 
    // AT Commands AP and ID. Note: These changes will
    // be stored in volatile memory and will not persist
    // if power is lost.
    xbee.prepareATCommand('AP',2);
    xbee.send();
    delay(200);
    uint8_t panID[] = {0x12,0x34}; // Max: 64-bit
    xbee.prepareATCommand('ID',panID,sizeof(panID));
    xbee.send();    
    
    // The changes above can be saved to non-volatile memory 
    // (and will survive power on/off) using the WR command.
    // However, the WR command should be used sparingly! 
    // The EM250 chip inside the XBee only supports a
    // limited number of write cycles.
    //xbee.prepareATCommand('WR'); 
    //xbee.send();
    //delay(500);
  }
  
  
  void loop() {
    // If data is waiting in the XBee serial port ...
    while( xbee.available() ){
      // ... read the data.
      xbee.read();
      // If a complete message is available, display the contents
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
      payload[0] = val >> 8 & 0xff;
      payload[1] = val & 0xff;
      // Each SimpleZigBeeRadio objects contains two 
      // SimpleZigBeePacket objects, one for storing  
      // incoming messages and one for outgoing messages.
      // The SimpleZigBeeRadio class includes several 
      // methods to make it easy to prepare outgoing packets.
      // For example, prepareTXRequestToCoordinator()
      // will accept a payload and payload length and will 
      // set the frame type, frame ID, option, and address.
      xbee.prepareTXRequestToCoordinator( payload, 2 );
      
      // Other TX methods include:
      // setFrameType(uint8_t frameType);
      // setFrameID(uint8_t id);
      // setAddress(SimpleZigBeeAddress address);
      // setAddress(uint32_t adr64MSB, uint32_t adr64LSB, uint16_t adr16);  
      // setTXRequestBroadcastRadius(uint8_t rad);
      // setTXRequestOption(uint8_t opt);
      // setTXRequestPayload(uint8_t* payload, int payloadSize);
      // prepareTXRequest(uint32_t adr64MSB, uint32_t adr64LSB, uint16_t adr16, uint8_t* payload, int payloadSize);
      // prepareTXRequest(SimpleZigBeeAddress address, uint8_t* payload, int payloadSize);
      // prepareTXRequestBroadcast(uint8_t* payload, int payloadSize);
            
      // If no packet is passed to send(), the SimpleZigBeeRadio's
      // outgoing packet will be sent.
      xbee.send(); 
      Serial.print("\nSend: ");
      printPacket( xbee.getOutgoingPacketObject() );
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
