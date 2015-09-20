/* 
   Getting Started, Part 2: Coordinator
  
   This example will expand upon the previous Coordinator sketch
   (Getting Started, Part 1: Coordinator). Added features include
   setting the API Mode and PAN ID from the Arduino and identifying
   incoming packet types.
   
   Again, you will need two XBee S2 radios (one with
   Coordinator API firmware and one with Router API firmware)
   and two Arduino boards.
   
   ###########################################################
   created 30 June 2014
   updated 16 Sept 2015
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
   
   4. Complete "Getting Started, Part 2: Router".

 */

  #include <SimpleZigBeeRadio.h>
  #include <SoftwareSerial.h>

  // Create the XBee object ...
  SimpleZigBeeRadio xbee = SimpleZigBeeRadio();
  // ... and the software serial port. Note: Only one
  // SoftwareSerial object can receive data at a time.
  SoftwareSerial xbeeSerial(10, 11); // (RX=>DOUT, TX=>DIN)

  // Setup check
  int check = 0;
      
  void setup() {
    // Start the serial ports ...
    Serial.begin( 9600 );
    while( !Serial ){;} // Wait for serial port (for Leonardo only).
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
    xbee.prepareATCommand('AP',0x02);
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
      // If a complete message is available, check the contents
      if( xbee.isComplete() ){
        // Remember, you can use 
        //SimpleIncomingZigBeePacket zbpIn = xbee.getIncomingPacketObject();
        // to access the incoming packet. However, in this example,
        // we will use SimpleZigBeeRadio methods to interpret the
        // message.
        
        Serial.print("\nIncoming Message: ");
        printPacket( xbee.getIncomingPacketObject() );
        
        // Check the frame type. Currently, the only types
        // recognized by the SimpleZigBeeRadio class are 
        // ZigBee RX Packet (0x90), ZigBee TX Status (0x8b),
        // AT Command Response (0x88), Remote AT Command
        // Response (0x97), and Modem Status (0x8a).
        if( xbee.isRX() ){
          Serial.println( "RX Packet Received" );
          // Methods for RX packets are...
          SimpleZigBeeAddress addr = xbee.getRXAddress();
          SimpleZigBeeAddress64 addr64 = xbee.getRXAddress64(); 
          SimpleZigBeeAddress16 addr16 = xbee.getRXAddress16();  
          uint8_t rxOpt = xbee.getRXOptions();
          uint8_t rxLength = xbee.getRXPayloadLength();
          Serial.print( "Payload: " );
          for(int i=0;i<rxLength;i++){
            uint8_t rxData = xbee.getRXPayload( i );
            Serial.print(rxData,HEX);
            Serial.print(' ');
          }
          Serial.println();

        }else if( xbee.isTXStatus() ){
          Serial.print( "TX Status Received: " );
          // Methods for TX Status packets are...
          uint8_t frameID = xbee.getIncomingFrameID();
          SimpleZigBeeAddress16 addr16 = xbee.getTXStatusAddress16(); 
          uint8_t txRetry = xbee.getTXStatusRetryCount();
          uint8_t txStat = xbee.getTXStatusDeliveryStatus();
          uint8_t txDscovery = xbee.getTXStatusDiscoveryStatus();
          Serial.print( "Status: " );
          Serial.println(txStat,HEX);
          
        }else if( xbee.isATResponse() ){
          Serial.println( "AT Command Response Received" );
          // Methods for AT Command Response packets are...
          uint8_t frameID = xbee.getIncomingFrameID();
          uint16_t atCmd = xbee.getATResponseCommand();
          uint8_t atStat = xbee.getATResponseStatus();
          uint8_t atLength = xbee.getATResponsePayloadLength();
          Serial.print( "Status: " );
          Serial.println(atStat,HEX);
          if( atLength == 1 ){
            uint8_t atData = xbee.getATResponsePayload();
            Serial.print( "Payload: " );
            Serial.println(atData,HEX);
          }else if( atLength > 1 ){
            Serial.print( "Payload: " );
            for(int i=0;i<atLength;i++){
              uint8_t atData = xbee.getATResponsePayload(i);
              Serial.print(atData,HEX);
              Serial.print(' ');
            }
            Serial.println();
          }
          
        }else if( xbee.isRemoteATResponse() ){
          Serial.println( "Remote AT Command Response Received" );
          // Methods for Remote AT Command Response packets are...
          SimpleZigBeeAddress addr = xbee.getRemoteATResponseAddress();
          SimpleZigBeeAddress64 addr64 = xbee.getRemoteATResponseAddress64(); 
          SimpleZigBeeAddress16 addr16 = xbee.getRemoteATResponseAddress16();  
          uint8_t frameID = xbee.getIncomingFrameID();
          uint16_t reATCmd = xbee.getRemoteATResponseCommand();
          uint8_t reATStat = xbee.getRemoteATResponseStatus();
          uint8_t reATLength = xbee.getRemoteATResponsePayloadLength();
          Serial.print( "Status: " );
          Serial.println(reATStat,HEX);
          if( reATLength == 1 ){
            uint8_t reATData = xbee.getRemoteATResponsePayload();
            Serial.print( "Payload: " );
            Serial.println(reATData,HEX);
          }else if( reATLength > 1 ){
            Serial.print( "Payload: " );
            for(int i=0;i<reATLength;i++){
              uint8_t reATData = xbee.getRemoteATResponsePayload(i);
              Serial.print(reATData,HEX);
              Serial.print(' ');
            }
            Serial.println();
          }
          
        }else if( xbee.isModemStatus() ){
          Serial.println( "Modem Status Received" );
          // Methods for Modem Status packets are...
          uint8_t modemStat = xbee.getModemStatus();
          Serial.print( "Status: " );
          Serial.println(modemStat,HEX);
          
        }else{
          // Other or unimplemented frame type
          SimpleZigBeePacket p = xbee.getIncomingPacketObject();
          uint8_t frameType = p.getFrameData(0);
          Serial.print( "Other Frame Type: " );
          Serial.println(frameType,HEX);
          for( int i=1; i<p.getFrameLength(); i++ ){
            uint8_t frameData = p.getFrameData(i);
          }
        }
      }
    }
    
    delay(10); // Small delay for stability
    
    
    // Since changes were made to the radio's settings,
    // below are a few checks to verify that the changes
    // were applied.
    if( check < 2 ){
      if( check == 0 ){
        xbee.prepareATCommand('AP');
        xbee.send();
        Serial.println();
        Serial.print("Send: ");
        printPacket( xbee.getOutgoingPacketObject() ); 
      }else if( check == 1){
        xbee.prepareATCommand('ID');
        xbee.send();
        Serial.println();      
        Serial.print("Send: ");
        printPacket( xbee.getOutgoingPacketObject() ); 
      }
      check ++;
      delay(200);
    }

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
