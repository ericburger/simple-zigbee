/* 
   Getting Started, Part 3: Coordinator
  
   This example will expand upon the previous Coordinator sketch
   (Getting Started, Part 2: Coordinator). In this example, the
   Arduino will process and respond to incoming RX packets. 
   Specifically, the Router will send 3 1-byte integers in
   a TX packet payload. The Coordinator will add the integers 
   and return the sum with a TX packet to the Router.
   
   Again, you will need two XBee S2 radios (one with
   Coordinator API firmware and one with Router API firmware)
   and two Arduino boards.
   
   ###########################################################
   created 30 June 2014
   updated 17 Sept 2015
   by Eric Burger
   
   This example code is in the public domain.
   The SimpleZigBee library is released under the GNU GPL v2 License
   ###########################################################
   
   Setup (same as Getting Started, Part 1: Coordinator):
   1. Use the XCTU Software to load the Coordinator API firmware 
   onto an XBee S2 radio.
   
   2. Connect DOUT to Pin 10 (RX) and DIN to Pin 11 (TX). Also,
   connect the XBee to 3.3V and ground (GND).
   
   3. Upload this sketch (to the Arduino attached to the 
   Coordinator) and open the Arduino IDE's Serial Monitor. 
   Read through the commented code below to understand what
   is being displayed in the serial monitor.

   4. Complete "Getting Started, Part 3: Router".
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
        
        // Check the packet type. Currently, the only types
        // recognized by the SimpleZigBeeRadio class are 
        // ZigBee RX Packet (0x90), ZigBee TX Status (0x8b),
        // AT Command Response (0x88), Remote AT Command
        // Response (0x97), and Modem Status (0x8a).
        if( xbee.isRX() ){
          Serial.println( "RX Packet Received" );
          // In this example, we will treat the payload as a list
          // of numbers that need to be summed. The Arduino will
          // add up the values in the list and return the solution
          // to the sender.
          Serial.print( "Sum: " );
          int sum = 0;
          for(int i=0;i<xbee.getRXPayloadLength();i++){
            sum = sum + xbee.getRXPayload(i);
            if(i!=0){
              Serial.print( " + " );
            }
            Serial.print(xbee.getRXPayload(i));
          }
          Serial.print( " = " );
          Serial.println( sum );
          
          // The sum is an int and must therefore be broken
          // into 2 bytes and set as the payload.
          uint8_t payload[2]; // ints are 2 bytes
          payload[0] = sum >> 8 & 0xff;
          payload[1] = sum & 0xff;
          // Prepare a TX Packet with the address of the sender.
          xbee.prepareTXRequest(xbee.getRXAddress(),payload,2);
          // Same as
          //xbee.prepareTXRequest(xbee.getRXAddress64().getAddressMSB(),xbee.getRXAddress64().getAddressLSB(),xbee.getRXAddress16().getAddress(),payload,2);
          
          xbee.send();
          Serial.print( "Send: " );
          printPacket( xbee.getOutgoingPacketObject() );

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
