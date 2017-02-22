/**
 * @file mac.h
 * @brief MAC functions
 * @author Adrien van den Bossche <bossche@irit.fr>
 * @date 20111118
 */

#ifndef MAC_H
#define MAC_H

#define MAC_CCA_MEDIUM_BUSY 135 // if energy on medium > 135: medium busy

#define MAC_BACKOFF_SLOT_DURATION 640 // us
#define MAC_MIN_BE 3
#define MAC_MAX_CSMA_CA_BACKOFF 4
#define MAC_MAX_FRAME_RETRIES 3
#define MAC_ACK_WAIT_DURATION 10000 // us
#define MAC_WAIT_BEFORE_SEND_ACK 640 // us
#define MAC_INTERFRAME_DELAY 2000 // us
#define MAC_ACK_FRAME_LENGTH 3 // bytes
#define NO_ACK_REQUESTED false
#define ACK_REQUESTED true
#define MAX_MAC_HEADER_SIZE 16

#define NEIGHB_TABLE_MAX_NEIGHBORS_COUNT 16
#define NEIGHB_TABLE_ADDRESS_NEIGHBOR_EMPTY 0xFFFF
#define NEIGHB_NEIGHBOR_NOT_FOUND 0xFF

struct sqn_t {
 /**
  * @brief Contains the sequence numbers received for each neighbor.
  */
  uint8_t beacon;/**< @brief The last received beacon's sequence number.*/
  uint8_t data;/**< @brief The last received data's sequence number.*/
  uint8_t mac_command;/**< @brief The last received mac_command's sequence number.*/

}; // sqn_t

struct neighbor_t {

  uint16_t address;
  uint32_t lastUpdate;
  uint8_t lastRssi;
  struct sqn_t sqn;

}; // neighbor_struct

// Global vars
struct neighbor_t neighbors [NEIGHB_TABLE_MAX_NEIGHBORS_COUNT];
uint8_t neighborsCount;

#define FRAME_TYPE_MASK           0x03
#define FRAME_TYPE_BEACON         0x00
#define FRAME_TYPE_DATA           0x01
#define FRAME_TYPE_ACK            0x02
#define FRAME_TYPE_MAC_COMMAND    0x03
#define SECURITY_ENABLED          0x08
#define FRAME_PENDING             0x10
#define ACK_REQUEST               0x20
#define INTRA_PAN                 0x40
#define DEST_ADDR_MODE_16BITS     0x800
#define SRC_ADDR_MODE_16BITS      0x8000

#define MAC_CSMA_CA_BEGIN_OF_PERIOD_STATE		1
#define MAC_CSMA_CA_NEW_FRAME_STATE			2
#define MAC_CSMA_CA_INIT_CSMA_CA_VALUES			3
#define MAC_CSMA_CA_SET_RANDOM_BACKOFF_DELAY_STATE	4
#define MAC_CSMA_CA_WAIT_BACKOFF_DELAY			5
#define MAC_CSMA_CA_PERFORM_CCA				6
#define MAC_CSMA_CA_TX_FRAME_STATE			7
#define MAC_CSMA_CA_WAIT_ACK_STATE			8
#define MAC_CSMA_CA_WAIT_INTERFRAME_STATE       	9

#define MCPS_DATA_REQUEST_SUCCESS			0
#define MCPS_DATA_REQUEST_MAC_TX_BUSY			1
#define MCPS_DATA_CONFIRM_STATUS_SUCCESS		0
#define MCPS_DATA_CONFIRM_STATUS_NO_ACK			1
#define MCPS_DATA_CONFIRM_STATUS_CHANNEL_ACCESS_FAILURE	2

//#define CONSOLE_CMD_HANDLE_STATE_MAC_DEBUG 10

// Global variables
rxFrame_t macCurrentRxFrame; // No queue for reception
txFrame_t macCurrentTxFrame; // No queue for transmission
uint32_t macCbrNextTimeToSend, macCsmaCaBackoffDurationTimeout, macInterframeDurationTimeout;
uint8_t macFrameToSend;
uint8_t macFrameReceived;
uint16_t macLastSourceAddressReceived;
uint8_t* macLastPayload;
uint8_t macLastPayloadLen;

struct sqn_t mac_sqn;
uint8_t lastAckReceived;
struct txFrame_t* currentTxFrame;
uint8_t macCsma_CaState;
uint32_t currentTxFrameAckTimeoutOnLclk;
uint32_t currentTxFrameBackoff;
int8_t currentTxFrameRetries;
uint32_t macEndOfTxPeriod;
uint8_t macFrameInCsma_CaEngine;
uint8_t macFrameInGtsEngine;
uint8_t macCsma_CaNb;
uint8_t macCsma_CaBe;


// Prototypes

/**
* @brief Initilize MAC layer process
* @return No return
* @author Adrien van den Bossche <bossche@irit.fr>
* @date 20111011
*/
void macInit ( void );

/**
* @brief Process engine of MAC layer. State of MAC state machine is a global var. 
* @return No return
* @author Adrien van den Bossche <bossche@irit.fr>
* @date 20111118
*/
void macEngine ( void );

/**
* @brief Called by upper layer, prepare and send a MAC-level data frame with given parameters and payload.
* @return Return the length of the frame
* @author Adrien van den Bossche <bossche@irit.fr>
* @date 20111118
*/
uint8_t MCPS_data_request ( uint8_t ackRequest, uint8_t intraPan, uint16_t panId, uint8_t mode, uint16_t destinationAddress, struct txFrame_t *txFrame );

/**
* @brief Give received data from physical layer
* @return No return
* @author Adrien van den Bossche <bossche@irit.fr>
* @date 20111011
*/
void PD_data_indication ( struct rxFrame_t *rxFrame );

/**
* @brief Make a frame control field with given parameters
* @return Return the frame control field
* @author Adrien van den Bossche <bossche@irit.fr>
* @date 20111123
*/
uint16_t macMakeFrameControlField ( uint8_t frameType, uint8_t ackRequest, uint8_t intraPan );

/**
* @brief Make a MAC-level header @buffer with given parameters
* @return Return the length of the header
* @author Adrien van den Bossche <bossche@irit.fr>
* @date 20111114
*/
uint8_t macMakeMacHeader ( uint8_t frameType, uint8_t ackRequest, uint8_t intraPan, uint16_t panId, uint16_t destinationAddress, uint8_t sequenceNumber, uint8_t buffer[] );

/**
* @brief Decode a MAC-level header @buffer with given parameters
* @return Return the length of the header
* @author Adrien van den Bossche <bossche@irit.fr>
* @date 20111122
* @todo vérifier DEST_ADDR_MODE_16BITS & SRC_ADDR_MODE_16BITS;
*/
uint8_t macDecodeMacHeader ( uint8_t *frameType, uint8_t *ackRequest, uint8_t *intraPan, uint16_t *panId, uint16_t *destinationAddress, uint16_t *sourceAddress, uint8_t *sequenceNumber, uint8_t buffer[] );

/**
* @brief Send an ACK frame with given sequence number
* @return No return
* @author Adrien van den Bossche <vandenbo@iut-blagnac.fr>
* @date 20111123
*/
void macSendAck ( uint8_t sqn );

/**
* @brief Decode a received frame on radio
* @return No return
* @author Adrien van den Bossche <vandenbo@iut-blagnac.fr>
* @date 20100118
*/
void macDecodeReceivedFrame ( struct rxFrame_t *rxFrame );

/**
* @brief Initialize the neighbour table and tools
* @date 20131102
* @author Adrien van den Bossche <bossche@irit.fr>
* @return No return
*/
void neighbInit ( void );

/**
* @brief Process neighbour engine. Must be called regularly.
* @return No return
* @author Adrien van den Bossche <bossche@irit.fr>
* @date 20140720
*/
void neighbEngine ( void );

/**
* @brief cleans the neighbor table (adress <- NEIGHBOR_ELEMENT_EMPTY)
* @return no return
* @author Adrien van den Bossche <bossche@irit.fr>
* @date 20110607
*/
void neighbFreeNeighborTable ( void );

/**
* @brief adds a node in the neighbor table if it is not present yet
* @return returns the number of times of nodeAdress is seen in the neighbor table before adding or -1 if neighbor table is full
* @author Adrien van den Bossche <bossche@irit.fr>
* @date 20110607
*/
uint8_t neighbAddNeighbor ( uint16_t nodeAddress, uint32_t lastUpdate, uint8_t RSSI );

/**
* @brief get neighbor index
* @return returns the index of the first nodeAddress found in the neighbor table or NEIGHB_TABLE_NEIGHBOR_NOT_FOUND if not present
* @author Adrien van den Bossche <bossche@irit.fr>
* @date 20110607
*/
uint8_t neighbGetNeighborIndex ( uint16_t nodeAddress );

/**
* @brief update a node in the neighbor table 
* @return returns TRUE if update succefull or FALSE otherwise
* @author Adrien van den Bossche <bossche@irit.fr>
* @date 20110607
*/
uint8_t neighbUpdateNeighbor ( uint16_t nodeAddress, uint32_t lastUpdate, uint8_t RSSI );

/**
* @brief set data of the #elementIndex of the neighbor table (internal usage)
* @return no return
* @author Adrien van den Bossche <bossche@irit.fr>
* @date 20110607
*/
void neighbSetElementOfNeighborTable ( uint8_t elementIndex, uint16_t nodeAddress, uint32_t lastUpdate, uint8_t RSSI );

/**
* @brief gets the number of neighbors
* @return returns the number of neighbors
* @author Adrien van den Bossche <bossche@irit.fr>
* @date 20130822
*/
uint8_t neighbGetNeighborsCount ( void );

/**
* @brief gets the list of address of neighbors
* @return returns the length of the list
* @author Adrien van den Bossche <bossche@irit.fr>
* @date 20110607
*/
uint8_t neighbGetNeighbors ( uint16_t* list );

/**
* @brief prints the neighbor table on console
* @return no return
* @author Adrien van den Bossche <bossche@irit.fr>
* @date 20110607
*/
void neighbPrintNeighbors ( void );

/**
* @brief send a CBR frame 
* @return no return
* @author Adrien van den Bossche <bossche@irit.fr>
* @date 01032015
*/
void macSendCbrFrame();

/**
* @brief send a MAC payload (typically from Arduino sketch)
* @return no return
* @author Adrien van den Bossche <bossche@irit.fr>
* @date 01032015
*/
void send ( uint16_t destAddress, uint8_t* payload, uint8_t len );

/**
* @brief non-blocked MAC payload reception (typically from Arduino sketch)
* @return true if a payload has been transfered in uint8_t* payload
* @author Adrien van den Bossche <bossche@irit.fr>
* @date 01032015
*/
uint8_t recv ( uint16_t* sourceAddress, uint8_t* payload, uint8_t* len );

#endif

