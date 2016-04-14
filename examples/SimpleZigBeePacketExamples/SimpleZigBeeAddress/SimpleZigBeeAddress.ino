/* 
   SimpleZigBeeAddress class example
  
   This sketch demonstrates how to use the 
   SimpleZigBeeAddress class to store the 64-bit
   and 16-bit addresses of a ZigBee radio. The
   SimpleZigBeeAddress class does not interact 
   with the XBee radio and thus it is not 
   necessary to connect a radio to run 
   this example code.
  
   created 5 January 2014
   by Eric Burger
   
   This example code is in the public domain.
  
 */
  
  #include <SimpleZigBeeAddress.h>
  
  // Every radio in a ZigBee mesh network has a globally
  // unique 64-bit address and a 16-bit address assigned
  // by the network. The SimpleZigBeeAddress class
  // makes it easy to store this information for use
  // in an Arduino sketch. For consistency, all 
  // addresses will be shown in their hexadecimal
  // (base 16) form.
  
  // Create a SimpleZigBeeAddress object. If no address is given,
  // the constructor will assign the broadcast addresses:
  //   - 64-bit address of 0x000000000000ffff
  //   - 16-bit address of 0xfffe
  SimpleZigBeeAddress addrOne = SimpleZigBeeAddress();
  
  // The Arduino UNO cannot create variables larger 
  // than 32-bits (4 bytes). Therefore, to express a
  // 64-bit address, we need 2 variables. msb represents
  // the most significant bytes and lsb represents the 
  // least significant bytes of the 64-bit address.
  // The SimpleZigBeeAddress class uses the inttype 
  // library (inttypes.h) to create 32-bit (uint32_t),
  // 16-bit (uint16_t), and 8-byte (uint8_t) variables.
  uint32_t msb64 = 0x0013a200;
  uint32_t lsb64 = 0x40a8bb63;
  
  // Create another SimpleZigBeeAddress object. Again,
  // we have not given a 16-bit address, therefore
  // the 16-bit broadcast address is assigned.
  SimpleZigBeeAddress addrTwo = SimpleZigBeeAddress( msb64, lsb64 );
  
  uint16_t addr16 = 0xf3a9;
  // Create one more SimpleZigBeeAddress object. 
  // This time, set a 16-bit address. 
  SimpleZigBeeAddress addrThree = SimpleZigBeeAddress( msb64, lsb64, addr16 );
  
  void setup() {
    Serial.begin( 9600 );
    while( !Serial ){;}// Wait to connect. Needed for Leonardo only
    
    Serial.println("Demo of SimpleZigBeeAddress classes and methods");
    Serial.println();
    
    // The 64-bit address can be accessed using the 
    // getAddress64() method, which returns a
    // SimpleZigBeeAddress64 object.
    SimpleZigBeeAddress64 addr64 = addrThree.getAddress64();
    // The getAddress16() method returns a 
    // SimpleZigBeeAddress16 object.
    SimpleZigBeeAddress16 addr16 = addrThree.getAddress16();
    // Store address data in variables.
    uint32_t msb = addr64.getAddressMSB(); // Most significant bytes
    uint32_t lsb = addr64.getAddressLSB(); // Least significant bytes
    uint16_t addr = addr16.getAddress();

    
    // Print addresses for addrThree
    Serial.print( "64-bit address of addrThree is: " );
    Serial.print( msb, HEX ); // Print the address in HEX form
    Serial.print( lsb, HEX );
    Serial.println();
    Serial.print( "16-bit address of addrThree is: " );
    Serial.print( addr, HEX );
    Serial.println();
    Serial.println();
    // Notice that using "Serial.print( msb, HEX );" 
    // caused the leading zeros to be dropped.
    // I have written the printFullAddress() function 
    // to check each byte and to print every zero.
    
    
    // Print addresses for addrOne
    Serial.print( "64-bit address of addrOne is: " );
    printFullAddress( addrOne.getAddress64().getAddressMSB(), 4 ); 
    printFullAddress( addrOne.getAddress64().getAddressLSB(), 4 );
    Serial.println();
    Serial.print( "16-bit address of addrOne is: " );
    printFullAddress( addrOne.getAddress16().getAddress(), 2 );
    Serial.println();
    Serial.println();
    
    Serial.println("Assign new values to addrOne");
    // The address can be changed using the setAddress64() 
    // and setAddress16() methods.
    // Pass a SimpleZigBeeAddress64 object...
    addrOne.setAddress64( addr64 );
    // ...or pass the 32-bit msb and 32-bit lsb of the address
    addrOne.setAddress64( msb, lsb );
    // Pass a SimpleZigBeeAddress16 object...
    addrOne.setAddress16( addr16 );
    // ...or pass the 16-bit address
    addrOne.setAddress16( addr );
    
    // Print new addresses for addrOne
    Serial.print( "64-bit address of addrOne is now: " );
    printFullAddress( addrOne.getAddress64().getAddressMSB(), 4 ); // Print the address in HEX form
    printFullAddress( addrOne.getAddress64().getAddressLSB(), 4 );
    Serial.println();
    Serial.print( "16-bit address of addrOne is now: " );
    printFullAddress( addrOne.getAddress16().getAddress(), 2 );
    Serial.println();
    Serial.println();
    Serial.println( "End of Demo" );
  }
  
  void loop() {
    // Do nothing
    delay(1000);
  }
  
  // Function for printing every byte of an address (including leading zeros).
  void printFullAddress( uint32_t addr, int bytes ){
    for(int i=0;i<bytes;i++){
      if( ((addr >> 8*i) & 0xf0) == 0 ){
        Serial.print( 0 );
        Serial.print( ((addr >> 8*i) & 0xf), HEX );
      }else{
        Serial.print( ((addr >> 8*i) & 0xff), HEX );
      }
    }  
  }
