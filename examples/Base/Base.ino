// This code is an example for using the SimpleWiNo library.
// See comments in the code for help on usage

#include <SPI.h>
#include <RH_RF22.h>
#include <SimpleWiNo.h>

#define MAX_DATA_LEN 64 // Maximum payload 
#define DESTINATION_ADDRESS 0xFFFF // 0xFFFF is the broadcast address

SimpleWiNo wino;

uint16_t srcAddr;
uint8_t len;
unsigned char data[MAX_DATA_LEN];

void setup() {
  
  // Init SimpleWiNo
  wino.init();
  
  // Set SimpleWiNo properties
  wino.set(NODE_SHORT_ADDRESS, 2); // WiNo address
  wino.set(NODE_PANID, 0xCAFE); // Logical isolation in the PAN
  wino.set(NODE_TXPOWER, 4); // Transmit power (0-7)
  wino.set(NODE_CHANNEL, 10); // Radio channel (0-17)

  // Init Serial and print some informations
  Serial.begin(115200);
  Serial.print("My PANid is 0x");
  Serial.println(wino.get(NODE_PANID), HEX);
  Serial.print("My address is 0x");
  Serial.println(wino.get(NODE_SHORT_ADDRESS), HEX);
}


void loop() {

  // Always call process() to enable SimpleWiNo's PHY and MAC engines
  wino.process();
  
  // If something is available on Serial, make a dummy message containing "test" and send it
  if ( Serial.available() > 0 ) {
    while ( Serial.available() > 0 ) Serial.read(); // Empties Serial buffer
    // Make a message "test" and send it
    data[0] = 't';
    data[1] = 'e';
    data[2] = 's';
    data[3] = 't';
    len = 4;
    wino.send(DESTINATION_ADDRESS, data, len); // Send data buffer (len bytes) to DESTINATION_ADDRESS 
  }

  // Check if something has been received by MAC layer. Print it on Serial
  if ( wino.recv(&srcAddr, &data[0], &len ) ) {
    Serial.print("Something received from 0x");
    Serial.println(srcAddr);
  }
}
