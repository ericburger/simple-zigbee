/* 
  Quick Demo: Modem Status
  
  This example will show how to interpret the modem
  status packets. You will need one XBee S2 radio 
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
  
  4. Power the XBee off and on (disconnect and reconnect to 3.3v)
  to see the Hardware Reset notice.
  
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
    // Random seed (for random PAN ID)
    randomSeed(analogRead(0));
  }
  
  
  void loop() {
    // If data is waiting in the XBee serial port ...
    while( xbee.available() ){
      // ... read the data.
      xbee.read();
      // If a complete message is available, check the contents
      if( xbee.isComplete() ){
        Serial.println();
        Serial.print("Incoming Message: ");
        printPacket( xbee.getIncomingPacketObject() );

        if( xbee.isModemStatus() ){
          Serial.print("Modem Status: ");
          uint8_t mStat = xbee.getModemStatus();
          
          // Below is an incomplete list of modem status values:
          // MODEM_STATUS_HARDWARE_RESET = 0x00
          // MODEM_STATUS_WATCHDOG_TIMER_RESET = 0x01
          // MODEM_STATUS_JOINED_NETWORK = 0x02
          // MODEM_STATUS_DISASSOCIATED = 0x03
          // MODEM_STATUS_COORDINATOR_STARTED = 0x06
          
          if( mStat == MODEM_STATUS_HARDWARE_RESET ){
            Serial.println("Hardware Reset");
          }else if( mStat == MODEM_STATUS_JOINED_NETWORK ){
            Serial.println("Network Joined");
          }else if( mStat == MODEM_STATUS_DISASSOCIATED ){
            Serial.println("Disassociated");
          }else if( mStat == MODEM_STATUS_COORDINATOR_STARTED ){
            Serial.println("Coordinator Started");
          }else{
            Serial.print("Code ");
            Serial.println(mStat);
          }

        }else if( xbee.isATResponse() ){
          Serial.println( "AT Command Response Packet" );
          uint16_t atCmd = xbee.getATResponseCommand();
          uint8_t atStat = xbee.getATResponseStatus();
          uint8_t atLength = xbee.getATResponsePayloadLength();
          
          //Serial.print("AT Command: ");
          //Serial.println( atCmd, HEX );

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

    // Send a series of AT Commands which will
    // change the Modem Status.
    if( check < 7 ){
      if( check == 0 ){
        Serial.println();
        Serial.println( "Send command to leave network (CB4)" );
        xbee.prepareATCommand('CB',4);
        xbee.send();
        delay(5000);
      }else if( check == 1){
        Serial.println();
        Serial.println( "Set API Mode (AP2)" );
        xbee.prepareATCommand('AP',2);
        xbee.send();
      }else if( check == 2 ){
        Serial.println();
        Serial.println( "Send command to check PAN ID (ID)" );
        xbee.prepareATCommand('ID');
        xbee.send();
      }else if( check == 3){
        Serial.println();
        Serial.print( "Set random PAN ID: " );
        uint8_t panID[] = {uint8_t(random(255)),uint8_t(random(255))};
        Serial.print( panID[0], HEX );
        Serial.println( panID[1], HEX );
        xbee.prepareATCommand('ID',panID,sizeof(panID));
        xbee.send();
        delay(5000);
      }else if( check == 4){
        Serial.println();
        Serial.println( "Send command to check PAN ID (ID)" );
        xbee.prepareATCommand('ID');
        xbee.send();
      }else if( check == 5){
        Serial.println();
        Serial.println( "Send command to check API Mode (AP)" );
        xbee.prepareATCommand('AP');
        xbee.send();
      }else if( check == 6){
        Serial.println();
        Serial.println( "Power the XBee off and on to see Hardware Reset notice." );
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
