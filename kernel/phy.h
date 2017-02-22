/**
 * @file phy.h
 * @brief PHY functions
 * @author Adrien van den Bossche <bossche@irit.fr>
 * @date 20130901
 */

rxFrame_t phyCurrentRxFrame; // No queue for reception
txFrame_t phyCurrentTxFrame; // No queue for transmission
uint32_t phyCbrNextTimeToSend;

void phyInit ( void );
void phyEngine ( void );
void PD_data_indication ( struct rxFrame_t *rxf );
void PD_data_request ( struct txFrame_t *txf );
uint8_t phyEdRequest ( void );
void phySendRandomFrame ( uint8_t length );
void phySendStringFrame ( char* str );

