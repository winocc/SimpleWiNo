/**
 * @file phy.c
 * @brief PHY functions
 * @author Adrien van den Bossche <bossche@irit.fr>
 * @date 20130901
 */


#include "phy.h"

extern RH_RF22 rf22;


void phyInit ( void ) {

  SPI.setSCK(14);
  if (!rf22.init())
    Serial.println(" RF22 init failed");
  else 
    Serial.println(" RF22 init OK");

  //rf22.setFrequency(433.1 + DEFAULT_RF22_CHANNEL*0.1, 0.05);
  //rf22.setTxPower(DEFAULT_RF22_TXPOWER);
  rf22.setModemConfig(RH_RF22::GFSK_Rb125Fd125);
  phyCbrNextTimeToSend = 0;
}


void phyEngine ( void ) {

#ifdef PHY_CBR_ACTIVE
  if ( PHY_CBR_ACTIVE ) {
    if ( micros() > phyCbrNextTimeToSend ) {
      phyCbrNextTimeToSend = micros() + CBR_TX_PERIOD;
      phySendRandomFrame(CBR_TX_LENGTH);
    }
  }
#endif

  if (rf22.available()) {
    // Should be a message for us now
    uint8_t buf[RH_RF22_MAX_MESSAGE_LEN];
    uint8_t len = sizeof(buf);
    if (rf22.recv(buf, &len)) {
      phyCurrentRxFrame.timestamp=micros();
      phyCurrentRxFrame.rssi=rf22.lastRssi();
      phyCurrentRxFrame.length=len;
#ifdef PHY_TIMESTAMPS_AT_TX
      phyCurrentRxFrame.txTimestamp = decodeUint32(&buf[len-sizeof(phyCurrentRxFrame.txTimestamp)]);
      phyCurrentRxFrame.length-=sizeof(phyCurrentRxFrame.txTimestamp);
#endif
      for (int i=0; i<len; i++) {
        phyCurrentRxFrame.data[i]=buf[i];
      }
      PD_data_indication(&phyCurrentRxFrame);
    }
  }
}


/* Now in mac.c (upper layer)
void PD_data_indication ( struct rxFrame_t *rxf ) {

  Serial.printf("%ld\t%d\t%d\t", rxf->timestamp, rxf->rssi, rxf->length);
  for (int i=0; i<rxf->length; i++) {
    Serial.printf("|%02X", rxf->data[i]);
  }
  Serial.println("|");
}
*/

void PD_data_request ( struct txFrame_t *txf ) {

/*
  Serial.print("PHY_DEBUG Sending ");
  for (int i=0; i<txf->length; i++)
    Serial.printf("|%02X", txf->data[i]);
  Serial.print("| ...");
*/

#ifdef PHY_TIMESTAMPS_AT_TX
  encodeUint32(micros(), &txf->data[txf->length]);
  txf->length+=4;
#endif

  rf22.send(txf->data, txf->length);
  rf22.waitPacketSent();
  //Serial.print(" Sent.\n");

#ifdef PHY_TIMESTAMPS_AT_TX
  // Restore original size in case of retry
  txf->length-=4;
#endif

}


void phySendRandomFrame ( uint8_t length ) {

  txFrame_t txf;

  makeRandomBytes(txf.data, length); 
  txf.length = length;
  PD_data_request(&txf);
}


void phySendStringFrame ( char* str ) {

  txFrame_t txf;
  
  for (unsigned int i=0; i<strlen(str); i++)
    txf.data[i]=str[i];
  txf.length = strlen(str);
  PD_data_request(&txf);
}


uint8_t phyEdRequest ( void ) {

  uint8_t rssi[3];

  rf22.setModeRx();
  rssi[0] = rf22.rssiRead();
  rssi[1] = rf22.rssiRead();
  rssi[2] = rf22.rssiRead();
  rf22.setModeIdle();

  if ( rssi[0] == rssi[1] ) return rssi[1];
  if ( rssi[1] == rssi[2] ) return rssi[2];
  if ( rssi[2] == rssi[0] ) return rssi[0];
  return rssi[2];
}
