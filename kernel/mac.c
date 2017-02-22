/**
 * @file mac.c
 * @brief MAC functions
 * @author Adrien van den Bossche <bossche@irit.fr>
 * @date 20111011
 */

#include "mac.h"

extern uint16_t nodeShortAddress;
extern uint16_t nodePanId;


void macInit ( void ) {

  mac_sqn.data = 0;
  neighbFreeNeighborTable();
  macCbrNextTimeToSend = 0;
  macFrameToSend = false;
  macFrameReceived = false;
  lastAckReceived = 0xff;
  macCsma_CaState = MAC_CSMA_CA_WAIT_INTERFRAME_STATE;
}


void PD_data_indication ( struct rxFrame_t *rxFrame ) {

  if ( phyDebug ) {
	  Serial.printf("PHY_DEBUG %ld\t%d\t%d\t", rxFrame->timestamp, rxFrame->rssi, rxFrame->length);
	  for (int i=0; i<rxFrame->length; i++) {
	    Serial.printf("|%02X", rxFrame->data[i]);
	  }
	  Serial.printf("|\n");
  }
  macDecodeReceivedFrame(rxFrame);
}


uint8_t MCPS_data_request ( uint8_t ackRequest, uint8_t intraPan, uint16_t panId, uint16_t destinationAddress, uint8_t* payload, uint8_t payloadLength ) {


  uint8_t i,j;

  if ( macFrameToSend )
    return MCPS_DATA_REQUEST_MAC_TX_BUSY;

  // If the destinationAddress is the broadcast address, ackRequest must me disabled
  if ( destinationAddress == BROADCAST_ADDRESS )
    ackRequest = false;

  // Make MAC header
  i = macMakeMacHeader ( FRAME_TYPE_DATA, ackRequest, intraPan, panId, destinationAddress, mac_sqn.data, macCurrentTxFrame.data);

  // Copy payload
  for (j=0; j<payloadLength; j++)
    macCurrentTxFrame.data[i+j] = payload[j];
  macCurrentTxFrame.length = i+payloadLength;
  if ( macDebug ) {
	Serial.printf("MAC_DEBUG new frame in buffer\n");
  }

  // increment sequence number for the next data frame
  mac_sqn.data++;
  macFrameToSend = true;
  return MCPS_DATA_REQUEST_SUCCESS;
}


void MCPS_data_confirm ( struct txFrame_t *txFrame, uint8_t code ) {

  macFrameToSend = false;
  if ( macDebug ) {
    Serial.printf("MCPS_data_confirm ");
    switch(code) {
      case MCPS_DATA_CONFIRM_STATUS_SUCCESS: Serial.printf("SUCCESS"); break;
      case MCPS_DATA_CONFIRM_STATUS_NO_ACK: Serial.printf("NO_ACK"); break;
      case MCPS_DATA_CONFIRM_STATUS_CHANNEL_ACCESS_FAILURE: Serial.printf("CHANNEL_ACCESS_FAILURE"); break;
    }
    Serial.printf("\n");
  }
}


void macEngine ( void ) {

  uint8_t backoff, ui8temp;

#ifdef MAC_CBR_ACTIVE
  if ( MAC_CBR_ACTIVE ) {
    if ( micros() > macCbrNextTimeToSend ) {
      macCbrNextTimeToSend = micros() + CBR_TX_PERIOD;
      macSendCbrFrame();
    }
  }
#endif

  switch ( macCsma_CaState ) {

    case MAC_CSMA_CA_NEW_FRAME_STATE:

      if ( macFrameInCsma_CaEngine ) {

        macCsma_CaState = MAC_CSMA_CA_SET_RANDOM_BACKOFF_DELAY_STATE;
        break; // end of MAC_CSMA_CA_NEW_FRAME_STATE

      } else {

        // Check frame presence in buffer
        if ( macFrameToSend ) {
          currentTxFrame = &macCurrentTxFrame;
          currentTxFrameRetries = MAC_MAX_FRAME_RETRIES;
          macFrameInCsma_CaEngine = true;
        } else break; // end of MAC_CSMA_CA_NEW_FRAME_STATE
      }

      // No break here: go immediately to next step MAC_CSMA_CA_INIT_CSMA_CA_VALUES

    case MAC_CSMA_CA_INIT_CSMA_CA_VALUES:

      macCsma_CaNb = 0;
      macCsma_CaBe = MAC_MIN_BE;
      // No break here: go immediately to next step MAC_CSMA_CA_SET_RANDOM_BACKOFF_DELAY

    case MAC_CSMA_CA_SET_RANDOM_BACKOFF_DELAY_STATE:

      backoff = 1 << macCsma_CaBe; // 2^BE
      ui8temp = random(backoff);
      if ( macDebug ) {
        Serial.printf("MAC_DEBUG New CSMA/CA backoff=%d", ui8temp);
      }
      macCsmaCaBackoffDurationTimeout = micros() + MAC_BACKOFF_SLOT_DURATION*ui8temp;
      macCsma_CaState = MAC_CSMA_CA_WAIT_BACKOFF_DELAY;

      break; // end of MAC_CSMA_CA_SET_RANDOM_BACKOFF_DELAY_STATE

    case MAC_CSMA_CA_WAIT_BACKOFF_DELAY:

      if ( cmpUi32GreaterWithRollover(micros(), macCsmaCaBackoffDurationTimeout))
        macCsma_CaState = MAC_CSMA_CA_PERFORM_CCA;

      break; // end of MAC_CSMA_CA_WAIT_BACKOFF_DELAY

    case MAC_CSMA_CA_PERFORM_CCA:

      ui8temp = phyEdRequest();
      if ( macDebug ) {
        Serial.printf(" cca=%d\n", ui8temp);
      }
      if ( ui8temp < MAC_CCA_MEDIUM_BUSY )
        macCsma_CaState = MAC_CSMA_CA_TX_FRAME_STATE;
      else {
        macCsma_CaNb++;
        macCsma_CaBe++;
        if ( macCsma_CaNb > MAC_MAX_CSMA_CA_BACKOFF ) {
          // faillure
          MCPS_data_confirm ( currentTxFrame, MCPS_DATA_CONFIRM_STATUS_CHANNEL_ACCESS_FAILURE );
          macFrameInCsma_CaEngine = false;
          macCsma_CaState = MAC_CSMA_CA_NEW_FRAME_STATE;
        } else {
          // try again
          macCsma_CaState = MAC_CSMA_CA_SET_RANDOM_BACKOFF_DELAY_STATE;
        }
      }

      break; // end of MAC_CSMA_CA_PERFORM_CCA

    case MAC_CSMA_CA_TX_FRAME_STATE:

      if ( currentTxFrameRetries >= 0 ) {
        if ( macDebug ) {
          Serial.printf("MAC_DEBUG Sending frame\n");
        }
        PD_data_request ( currentTxFrame );

        // Is this frame require ACK?
        if ( currentTxFrame->data[1] & ACK_REQUEST ) {
          currentTxFrameAckTimeoutOnLclk = micros()+MAC_ACK_WAIT_DURATION;
          macCsma_CaState = MAC_CSMA_CA_WAIT_ACK_STATE;
        } else { 
          MCPS_data_confirm ( currentTxFrame, MCPS_DATA_CONFIRM_STATUS_SUCCESS );
          macFrameInCsma_CaEngine = false;
          macInterframeDurationTimeout = micros() + MAC_INTERFRAME_DELAY;
          macCsma_CaState = MAC_CSMA_CA_WAIT_INTERFRAME_STATE;
        }
      } else {
        if ( macDebug ) {
          Serial.printf("MAC_DEBUG MAC_MAX_FRAME_RETRIES attempt\n");
        }
        MCPS_data_confirm ( currentTxFrame, MCPS_DATA_CONFIRM_STATUS_NO_ACK );
        macFrameInCsma_CaEngine = false;
        macCsma_CaState = MAC_CSMA_CA_NEW_FRAME_STATE;
      }

      break; // end of MAC_CSMA_CA_TX_FRAME_STATE

    case MAC_CSMA_CA_WAIT_ACK_STATE:

      if ( lastAckReceived == currentTxFrame->data[2] ) {
        MCPS_data_confirm ( currentTxFrame, MCPS_DATA_CONFIRM_STATUS_SUCCESS );
        macFrameInCsma_CaEngine = false;
        macInterframeDurationTimeout = micros() + MAC_INTERFRAME_DELAY;
        macCsma_CaState = MAC_CSMA_CA_WAIT_INTERFRAME_STATE;
      } else {
        // Is this ACK in timeout ?
        if ( cmpUi32GreaterWithRollover(micros(), currentTxFrameAckTimeoutOnLclk )) {
          currentTxFrameRetries--;
          macCsma_CaState = MAC_CSMA_CA_INIT_CSMA_CA_VALUES;
        }
      }

      break; // end of MAC_CSMA_CA_WAIT_ACK_STATE

    case MAC_CSMA_CA_WAIT_INTERFRAME_STATE:

      if ( cmpUi32GreaterWithRollover(micros(), macInterframeDurationTimeout) )
        macCsma_CaState = MAC_CSMA_CA_NEW_FRAME_STATE;

      break; // end of MAC_CSMA_CA_WAIT_INTERFRAME_STATE

    default:

      break;
  }
}


uint16_t macMakeFrameControlField ( uint8_t frameType, uint8_t ackRequest, uint8_t intraPan ) {

  uint16_t frameControl = 0;

  if ( intraPan ) frameControl |= INTRA_PAN;
  if ( ackRequest ) frameControl |= ACK_REQUEST;

  return frameControl | frameType | DEST_ADDR_MODE_16BITS | SRC_ADDR_MODE_16BITS;
}


uint8_t macMakeMacHeader ( uint8_t frameType, uint8_t ackRequest, uint8_t intraPan, uint16_t panId, uint16_t destinationAddress, uint8_t sequenceNumber, uint8_t buffer[] ) {

  // make frame control field
  encodeUint16 ( macMakeFrameControlField ( frameType, ackRequest, intraPan ), &buffer[0] );

  // place sequence number
  buffer[2] = sequenceNumber;

  // place addressing fields: panId, destination and source addresses
  encodeUint16 ( panId, &buffer[3] );
  encodeUint16 ( destinationAddress, &buffer[5] );
  encodeUint16 ( nodeShortAddress, &buffer[7] );

  return (9);
}


uint8_t macDecodeMacHeader ( uint8_t *frameType, uint8_t *ackRequest, uint8_t *intraPan, uint16_t *panId, uint16_t *destinationAddress, uint16_t *sourceAddress, uint8_t *sequenceNumber, uint8_t buffer[] ) {

  *frameType = buffer[1] & FRAME_TYPE_MASK;
  if ( buffer[1] & INTRA_PAN ) *intraPan = true; else *intraPan = false;
  if ( buffer[1] & ACK_REQUEST ) *ackRequest = true; else *ackRequest = false;

  *sequenceNumber = buffer[2];

  if ( *frameType == FRAME_TYPE_ACK ) return 3;

  *panId = decodeUint16 ( &buffer[3] );
  *destinationAddress = decodeUint16 ( &buffer[5] );
  *sourceAddress = decodeUint16 ( &buffer[7] );

  return (9);
}


void macSendAck ( uint8_t sqn ) {

  struct txFrame_t txFrame;

  txFrame.length = MAC_ACK_FRAME_LENGTH;
  encodeUint16 ( macMakeFrameControlField ( FRAME_TYPE_ACK, NO_ACK_REQUESTED, true ), &(txFrame.data[0]) );
  txFrame.data[2] = sqn;
  PD_data_request ( &txFrame );
}


void macDecodeReceivedFrame ( struct rxFrame_t *rxFrame ) {

  uint8_t i;
  uint8_t frameType;
  uint8_t ackRequest;
  uint8_t intraPan;
  uint16_t panId;
  uint16_t destinationAddress;
  uint16_t sourceAddress;
  uint8_t sequenceNumber;
  uint8_t headerLength;

  headerLength = macDecodeMacHeader (&frameType,&ackRequest,&intraPan,&panId,
                                     &destinationAddress,&sourceAddress,
                                     &sequenceNumber,rxFrame->data);

  if ( frameType == FRAME_TYPE_ACK ) {

    if ( rxFrame->length == MAC_ACK_FRAME_LENGTH ) {
      lastAckReceived = rxFrame->data[2];
      if ( macDebug ) {
        Serial.printf("MAC_DEBUG An ack with sqn=%02x has been received\n", rxFrame->data[2]);
      }
    } else {
      if ( macDebug ) {
        Serial.printf("MAC_DEBUG An ack with anormal size has been received (%dbytes)\n", rxFrame->length);
      }
    }
  } else {

    if ( panId != nodePanId ) {

      // This frame is not for my PanID. Drop it
      if ( macDebug ) {
        Serial.printf("MAC_DEBUG Another PanId received\n");
      }
      return;
    }

    // This frame contains addressing fields. Look for the source presence in the neighbor table
    //printf(" panId=%x destinationAddress=%x sourceAddress=%x\n",panId,destinationAddress,sourceAddress);

    i = neighbGetNeighborIndex ( sourceAddress );

    if ( i == NEIGHB_NEIGHBOR_NOT_FOUND ) {

      i = neighbAddNeighbor ( sourceAddress, rxFrame->timestamp, rxFrame->rssi );
      if ( i > 1 ) {
        if ( macDebug ) {
          Serial.printf("MAC_DEBUG neighbAddNeighbor() duplicated node in neighb table\n");
        }
      } else if ( i == -1 ) {
        if ( macDebug ) {
          Serial.printf("Neighbor table is full\n");
        }
      } else {
        i = neighbGetNeighborIndex ( sourceAddress );
        if ( macDebug ) {
          Serial.printf("MAC_DEBUG new neighbour (#%d)\n", i);
          neighbPrintNeighbors();
        }
      }
    } else {
      // Update required fields
      neighbors[i].lastRssi = rxFrame->rssi;
      neighbors[i].lastUpdate = rxFrame->timestamp;
    }

    if ( ( destinationAddress == nodeShortAddress ) || ( destinationAddress == BROADCAST_ADDRESS )) {

      // The frame is for this node or broadcast
      // the hardware does not manage ACK. Send it if required
      if ( ackRequest ) {
        delayMicroseconds(MAC_WAIT_BEFORE_SEND_ACK);
        if ( macDebug ) {
          Serial.printf("MAC_DEBUG Sending ACK to %04X sqn=%d\n", sourceAddress, sequenceNumber);
        }
        macSendAck ( sequenceNumber );
        if ( macDebug ) {
          Serial.printf("MAC_DEBUG ACK sent\n");
        }
      }

      switch ( frameType ) {

        case FRAME_TYPE_BEACON: 

          break;
   
        case FRAME_TYPE_DATA:

  	  // getting last sqn.data for this source. If duplicate frame detected, free the frame
          i = neighbGetNeighborIndex ( sourceAddress );

          if ( neighbors[i].sqn.data == sequenceNumber ) {
            if ( macDebug ) {
	      Serial.printf("Duplicated frame %d from %04X\n", sequenceNumber, sourceAddress);
	    }
  	  } else {
	    if ( macDebug ) {
	      Serial.printf("RX_DATA from %04X: calling MCPS_data_indication\n", sourceAddress);
	    }
 	    neighbors[i].sqn.data = sequenceNumber;
            //MCPS_data_indication ( rxFrame, sourceAddress ); no NWK layer. Simply prepare gloabal vars to recv payload

            macLastPayload = rxFrame->data+headerLength;
            macLastPayloadLen = rxFrame->length - headerLength;
            macLastSourceAddressReceived = sourceAddress;
            macFrameReceived = true;
          }

          break;

        case FRAME_TYPE_MAC_COMMAND:
   
          break;

        default:
   
          break;
      }
    } else {

      // This frame is not for me. Ignore it
    }
  }
}


void macSendCbrFrame() {

#ifdef MAC_CBR_ACTIVE
  uint8_t data[CBR_TX_LENGTH];

  makeRandomBytes(data, CBR_TX_LENGTH);

  if ( MCPS_data_request ( MAC_CBR_ACK_REQUESTED, true, nodePanId, MAC_CBR_DESTINATION_SHORT_ADDRESS, data, CBR_TX_LENGTH ) != MCPS_DATA_REQUEST_SUCCESS ) {

    Serial.printf("MAC_CBR_DEBUG congestion at MAC layer\n");
  }
#endif
}

/*
void send ( uint16_t destAddress, uint8_t* payload, uint8_t len ) {

  if ( MCPS_data_request ( true, true, nodePanId, destAddress, payload, len ) != MCPS_DATA_REQUEST_SUCCESS ) {

    Serial.printf("MAC_DEBUG cannot send data\n");
  }
}


uint8_t recv ( uint16_t* sourceAddress, uint8_t* payload, uint8_t* len ) {

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
*/


void neighbInit ( void ) {

  neighbFreeNeighborTable();
}


void neighbFreeNeighborTable ( void ) {
 
  uint8_t i;
  
  for ( i=0; i<NEIGHB_TABLE_MAX_NEIGHBORS_COUNT; i++ ) {
    neighbors[i].address = NEIGHB_TABLE_ADDRESS_NEIGHBOR_EMPTY;
    neighbors[i].sqn.beacon = 255;
    neighbors[i].sqn.data = 255;
    neighbors[i].sqn.mac_command = 255;
  }
    
  neighborsCount = 0;
}


uint8_t neighbAddNeighbor ( uint16_t nodeAddress, uint32_t lastUpdate, uint8_t RSSI ) {

  uint8_t i,j;
  i = 0; j = 0;

  // Is this node in the table yet? j is the number of time
  for ( i=0; i<NEIGHB_TABLE_MAX_NEIGHBORS_COUNT; i++ )
    if ( neighbors[i].address == nodeAddress ) j++;
    
  if ( j != 0 ) return j;
  else {
    // No neighbor with address=nodeAddress found on the table.
    // Now if table is full, return -1
    if ( neighborsCount == NEIGHB_TABLE_MAX_NEIGHBORS_COUNT ) return -1;
    // Searching for a empty place in the neighbor table
    for ( i=0; neighbors[i].address != NEIGHB_TABLE_ADDRESS_NEIGHBOR_EMPTY; i++ ); // table must be initialized with NEIGHB_TABLE_ADDRESS_NEIGHBOR_EMPTY

    neighbSetElementOfNeighborTable ( i, nodeAddress, lastUpdate, RSSI );
    if ( macDebug ) {
      Serial.printf("NEIGHB_DEBUG 0x%04X added in NT\n", nodeAddress);
    }
    neighborsCount++;
    return 0;
  }
}


uint8_t neighbGetNeighborIndex ( uint16_t nodeAddress ) {

  uint8_t i;

  for ( i=0; i<NEIGHB_TABLE_MAX_NEIGHBORS_COUNT; i++ )
    if ( neighbors[i].address == nodeAddress )
      return i;

  return NEIGHB_NEIGHBOR_NOT_FOUND;
}


uint8_t neighbUpdateNeighbor ( uint16_t nodeAddress, uint32_t lastUpdate, uint8_t RSSI ) {

  uint8_t neighborIndex;
  
  neighborIndex = neighbGetNeighborIndex ( nodeAddress );
  if ( neighborIndex == NEIGHB_NEIGHBOR_NOT_FOUND ) return false;
  neighbSetElementOfNeighborTable ( neighborIndex, nodeAddress, lastUpdate, RSSI );

  return true;
}


void neighbSetElementOfNeighborTable ( uint8_t elementIndex, uint16_t nodeAddress, uint32_t lastUpdate, uint8_t RSSI ) {

  neighbors[elementIndex].address = nodeAddress;
  neighbors[elementIndex].lastUpdate = lastUpdate;
  neighbors[elementIndex].lastRssi = RSSI;
}


uint8_t neighbGetNeighborsCount ( void ) {

  return neighborsCount;
}


void neighbPrintNeighbors ( void ) {

  uint8_t i;

  Serial.printf(">>> Neighbor table\n#neighbs: %d\n", neighborsCount);
  if ( neighborsCount != 0 ) {
    Serial.print(">@\tdate\t\tlstRssi\n");
    for ( i=0; i<NEIGHB_TABLE_MAX_NEIGHBORS_COUNT; i++ )
      if ( neighbors[i].address != NEIGHB_TABLE_ADDRESS_NEIGHBOR_EMPTY )
        Serial.printf("%04x\t%010ld\t%d\n", neighbors[i].address, neighbors[i].lastUpdate, neighbors[i].lastRssi);
  }
}

