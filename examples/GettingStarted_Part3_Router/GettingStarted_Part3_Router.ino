/* 
   Getting Started, Part 3: Router
  
   This example will expand upon the previous Router sketch.
   (Getting Started, Part 2: Router). In this example, the
   Router will send 3 1-byte integers usig a TX Request to
   the Coordinator. The Coordinator will add the numbers and
   return the sum.
  
   ###########################################################
   created 30 June 2014
   updated 17 Sept 2015
   by Eric Burger
   
   This example code is in the public domain.
   The SimpleZigBee library is released under the GNU GPL v2 License
   ###########################################################
   
   
   Setup (same as Getting Started, Part 1: Router):
   1. Complete "Getting Started, Part 3: Coordinator".
   
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
  
  unsigned long time = 0;
  unsigned long last_sent = 0;
  int sum = 0;
      int check = 0;
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
    
  }
  
  
  void loop() {
    // If data is waiting in the XBee serial port ...
    while( xbee.available() ){
      // ... read the data.
      xbee.read();
      // If a complete message is available, check the contents
      if( xbee.isComplete() ){
        // Print the contents of the incoming packet
        Serial.print("\nIncoming Message: ");
        printPacket( xbee.getIncomingPacketObject() );
        
        // Check the packet type. 
        if( xbee.isRX() ){
          Serial.println( "RX Packet Received" );
          // In this example, we expect the Coordinator to send
          // a TX Request containing an integer (2 bytes) in
          // the payload. This int should match the sum of
          // the numbers last sent by the router.
          if( xbee.getRXPayloadLength() == 2 ){
            int payloadVal = (xbee.getRXPayload(0) << 8) + xbee.getRXPayload(1);
  
            Serial.print( "Received Sum: " );
            Serial.println( payloadVal );
            Serial.print( "Stored Sum: " );
            Serial.println( sum );
          }
        }else if( xbee.isTXStatus() ){
          Serial.println( "TX Status Received" );
          Serial.print( "Status: " );
          Serial.println(xbee.getTXStatusDeliveryStatus(),HEX);
          
        }else if( xbee.isATResponse() ){
          Serial.println( "AT Command Response Received" );
          Serial.print( "Status: " );
          Serial.println(xbee.getATResponseStatus(),HEX);
          
        }else if( xbee.isRemoteATResponse() ){
          Serial.println( "Remote AT Command Response Received" );
          Serial.print( "Status: " );
          Serial.println(xbee.getRemoteATResponseStatus(),HEX);
          
        }else if( xbee.isModemStatus() ){
          Serial.println( "Modem Status Received" );
          Serial.print( "Status: " );
          Serial.println(xbee.getModemStatus(),HEX);
          
        }else{
          // Other or unimplemented frame type
          Serial.println( "Other Frame Type" );
        }
      }
    }
    
    // The Arduino will send a packet to the XBee once every 5 
    // seconds. To avoid overflowing the serial buffer, you 
    // should avoid putting long delays in a sketch and instead
    // use other methods of tracking time.
    time = millis();
    if( time > (last_sent+5000) ){
      last_sent = time; // Update the last_sent variable
      
      // Send 3 random numbers (between 0 and 254) to the
      // coordinator.
      uint8_t payload[3];
      payload[0] = random(255) & 0xff;
      payload[1] = random(255) & 0xff;
      payload[2] = random(255) & 0xff;
      sum = payload[0] + payload[1] + payload[2];
      xbee.prepareTXRequestToCoordinator( payload, sizeof(payload) );
      xbee.send();
      Serial.println();
      Serial.print( "Send: " );
      printPacket( xbee.getOutgoingPacketObject() );
    }
    
    delay(10); // Small delay for stability
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
