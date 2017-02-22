/**
 * @file SimpleWiNo.cpp
 * @brief Contains SimpleWiNo cpp class for Arduino
 * @author Adrien van den Bossche <bossche@irit.fr>
 * @date 20130529
 */

#include "Arduino.h"
#include "SimpleWiNo.h"

// Global vars (c)
RH_RF22 rf22(SS, 9);
uint16_t nodeShortAddress;
uint16_t nodePanId;
int phyDebug;
int macDebug;

#include "kernel/utils.c"
#include "kernel/phy.c"
#include "kernel/mac.c"


SimpleWiNo::SimpleWiNo(void) {

  /**
  * @brief SimpleWiNo constructor
  * @author Adrien van den Bossche <bossche@irit.fr>
  * @date 20130529
  * @return no return
  */

  set(RGB_PIN_RED, 23);
  set(RGB_PIN_GREEN, 5);
  set(RGB_PIN_BLUE, 6);
  set(PHY_DEBUG, 0);
  set(MAC_DEBUG, 0);
}


void SimpleWiNo::init() {

  /**
  * @brief SimpleWiNo init function
  * @author Adrien van den Bossche <bossche@irit.fr>
  * @date 20130529
  * @return no return
  */

  pinMode(rgbRed, OUTPUT);
  pinMode(rgbGreen, OUTPUT);
  pinMode(rgbBlue, OUTPUT);
  digitalWrite(rgbRed, LOW);
  digitalWrite(rgbGreen, LOW);
  digitalWrite(rgbBlue, LOW);

  randomSeed(analogRead(A13)); // Initialization of pseudo random seed

  phyInit();
  macInit();
}


void SimpleWiNo::process() {

  /**
  * @brief SimpleWiNo process function
  * @author Adrien van den Bossche <bossche@irit.fr>
  * @date 20130529
  * @return no return
  */

  phyEngine();
  macEngine();
}


int SimpleWiNo::set(uint8_t param, uint16_t value) {

  switch ( param ) {

    case RGB_PIN_RED:
      rgbRed = value;
      return true;
      break;
    
    case RGB_PIN_GREEN:
      rgbGreen = value;
      return true;
      break;
    
    case RGB_PIN_BLUE:
      rgbBlue = value;
      return true;
      break;

    case NODE_SHORT_ADDRESS:
      nodeShortAddress = value;
      return true;
      break;
    
    case NODE_PANID:
      nodePanId = value;
      return true;
      break;
    
    case NODE_TXPOWER:
      if ( (value>7) || (value<0) ) return false;
      nodeTxPower = value;
      rf22.setTxPower(value);
      return true;
      break;
    
    case NODE_CHANNEL:
      if ( (value>17) || (value<0) ) return false;
      nodeChannel = value;
      rf22.setFrequency(433.0 + value*0.1, 0.05);
      return true;
      break;

    case PHY_DEBUG:
      phyDebug = value;
      return true;
      break;

    case MAC_DEBUG:
      macDebug = value;
      return true;
      break;

    default:
      break;
  }
  
  return false;
}


uint16_t SimpleWiNo::get(uint8_t param) {

  switch ( param ) {

    case RGB_PIN_RED:
      return rgbRed;
      break;
    
    case RGB_PIN_GREEN:
      return rgbGreen;
      break;
    
    case RGB_PIN_BLUE:
      return rgbBlue;
      break;

    case NODE_SHORT_ADDRESS:
      return nodeShortAddress;
      break;
    
    case NODE_PANID:
      return nodePanId;
      break;
    
    case NODE_TXPOWER:
      return nodeTxPower;
      break;
    
    case NODE_CHANNEL:
      return nodeChannel;
      break;

    case PHY_DEBUG:
      return phyDebug;
      break;

    case MAC_DEBUG:
      return macDebug;
      break;

    default:
      break;
  }
  
  return -1;
}


void SimpleWiNo::send ( uint16_t destAddress, uint8_t* payload, uint8_t len ) {

  if ( MCPS_data_request ( true, true, nodePanId, destAddress, payload, len ) != MCPS_DATA_REQUEST_SUCCESS ) {

    Serial.printf("MAC_DEBUG cannot send data\n");
  }
}


uint8_t SimpleWiNo::recv ( uint16_t* sourceAddress, uint8_t* payload, uint8_t* len ) {

  if ( macFrameReceived ) {

    for ( uint8_t i=0; i<macLastPayloadLen; i++) {
      // copy payload
      payload[i] = macLastPayload[i];
    } 
    *sourceAddress = macLastSourceAddressReceived;
    *len = macLastPayloadLen;
    macFrameReceived = false;
    return true;

  } else return false;
}


void SimpleWiNo::rgb(uint8_t red, uint8_t green, uint8_t blue) {

  analogWrite(rgbRed, red);
  analogWrite(rgbGreen, green);
  analogWrite(rgbBlue, blue);
}


uint16_t SimpleWiNo::decodeUi16 ( uint8_t *data ) {

  return decodeUint16(data);
}


void SimpleWiNo::encodeUi16 ( uint16_t from, uint8_t *to ) {

  encodeUint16(from,to);
}


uint32_t SimpleWiNo::decodeUi32 ( uint8_t *data ) {

  return decodeUint32(data);
}


void SimpleWiNo::encodeUi32 ( uint32_t from, uint8_t *to ) {

  encodeUint32(from,to);
}


float SimpleWiNo::decodeFloat ( uint8_t *data ) {

  return decodeFloat(data);
}


void SimpleWiNo::encodeFloat ( float from, uint8_t *to ) {

  encodeFloat(from,to);
}

