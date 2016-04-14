/* 
  Quick Demo: AT Command
  
  This example will show how to send an AT Command and how to
  interpret the response. You will need one XBee S2 radio 
  (with Coordinator API firmware) and one Arduino board.
  
  ###########################################################
  created 3 July 2014
  updated 13 April 2016
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
    while( !Serial ){;// Wait for serial port (for Leonardo only). 
    }
    xbeeSerial.begin( 9600 );
    // ... and set the serial port for the XBee radio.
    xbee.setSerial( xbeeSerial );
    // Set a non-zero frame id to receive Status and Response packets.
    xbee.setAcknowledgement(true);
  }
  
  void loop() {
    // If data is waiting in the XBee serial port ...
    while( xbee.available() ){
      // ... read the data.
      xbee.read();
      // If a complete message is available, check the contents
      if( xbee.isComplete() ){

        Serial.print("\nIncoming Message: ");
        printPacket( xbee.getIncomingPacketObject() );
         
        if( xbee.isATResponse() ){
          Serial.println( "AT Command Response Packet" );
          // Methods specific to AT Command Response packets are...
          uint8_t frameID = xbee.getIncomingFrameID();
          uint16_t atCmd = xbee.getATResponseCommand();
          uint8_t atStat = xbee.getATResponseStatus();
          uint8_t atLength = xbee.getATResponsePayloadLength();
          
          // Below are the possible AT Response Status Values
          // AT_COMMAND_STATUS_OK = 0x00
          // AT_COMMAND_STATUS_ERROR = 0x01
          // AT_COMMAND_STATUS_INVALID_COMMAND = 0x02
          // AT_COMMAND_STATUS_INVALID_PARAMETER = 0x03
          // AT_COMMAND_STATUS_TX_FAILURE = 0x04
          
          Serial.print("AT Command: ");
          Serial.println( atCmd, HEX );

          if( atCmd == 'ID' ){
            if( atLength > 0 ){
              Serial.print("PAN ID: ");
              for(int i=0;i<atLength;i++){
                Serial.print( xbee.getATResponsePayload(i), HEX);
                Serial.print(' ');
              }
              Serial.println();
            }else if( atStat == AT_COMMAND_STATUS_OK ){
              Serial.println("PAN ID Set");
            }else if( atStat == AT_COMMAND_STATUS_ERROR ){
              Serial.println("Error While Setting PAN ID");
            }else{
              Serial.print("AT Command Error: ");
              Serial.println(atStat);
            }
          }else if( atCmd == 'AP' ){
            if( atLength > 0 ){
              Serial.print("API Mode: ");
              Serial.println( xbee.getATResponsePayload(0), HEX);
            }else if( atStat == AT_COMMAND_STATUS_OK ){
              Serial.println("API Mode Set");
            }else if( atStat == AT_COMMAND_STATUS_ERROR ){
              Serial.println("Error While Setting API Mode");
            }else{
              Serial.print("AT Command Error: ");
              Serial.println(atStat);
            }
          }
          
        }else{
          Serial.println( "Other Frame Type" );
        }
      }
    }
    
    delay(10); // Small delay for stability
    
    // Below are examples of using AT Commands to set and
    // check the API Mode and the PAN ID.
    if( check < 5){
      if( check == 0 ){
        // To ensure that the radio is in API Mode 2 and is
        // operating on the correct PAN ID, you can use the 
        // AT Commands AP and ID. Note: These changes will
        // be stored in volitile memory and will not persist
        // if power is lost.
        Serial.println();
        Serial.println( "Set API Mode (AP2)" );
        xbee.prepareATCommand('AP',2);
        //printPacket( xbee.getOutgoingPacketObject() );
        xbee.send();
      }else if( check == 1){
        Serial.println();
        Serial.println( "Set the PAN ID" );
        uint8_t panID[] = {0x12,0x34}; // Max: 64-bit
        xbee.prepareATCommand('ID',panID,sizeof(panID));
        //printPacket( xbee.getOutgoingPacketObject() );
        xbee.send();
      }else if( check == 2){
        // Since changes were made to the radio's settings,
        // below are a fews checks to verify that the changes
        // were applied.
        Serial.println();
        Serial.println( "Send command to check API Mode (AP)" );
        xbee.prepareATCommand('AP');
        //printPacket( xbee.getOutgoingPacketObject() );
        xbee.send();
      }else if( check == 3){
        Serial.println();
        Serial.println( "Send command to check PAN ID (ID)" );
        xbee.prepareATCommand('ID');
        //printPacket( xbee.getOutgoingPacketObject() );
        xbee.send();
      }
      check++;
      delay(2000);
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
