/**
 * @file SimpleWiNo.h
 * @brief Contains SimpleWiNo header elements for Arduino
 * @author Adrien van den Bossche <bossche@irit.fr>
 * @date 20130529
 */

#ifndef SIMPLEWINO_H
#define SIMPLEWINO_H

#define DEFAULT_RF22_CHANNEL 10
#define DEFAULT_RF22_TXPOWER 4
#define MAX_FRAME_LENGTH 64
#define BROADCAST_ADDRESS 0xFFFF

#include <RH_RF22.h>

struct txFrame_t {
  uint8_t length;
  uint8_t data[MAX_FRAME_LENGTH];
};

struct rxFrame_t {
  uint8_t length;
  uint8_t data[MAX_FRAME_LENGTH];
  uint8_t rssi;
  uint32_t timestamp; // 1 timestamp is 1us
#ifdef PHY_TIMESTAMPS_AT_TX
  uint32_t txTimestamp;
#endif
};

enum {
  NODE_SHORT_ADDRESS,
  NODE_PANID,
  NODE_TXPOWER,
  NODE_CHANNEL,
  RGB_PIN_RED,
  RGB_PIN_GREEN,
  RGB_PIN_BLUE,
  PHY_DEBUG,
  MAC_DEBUG
};

#include "Arduino.h"

class SimpleWiNo {

  public:
    SimpleWiNo();
    void init();
    void process();
    int set(uint8_t param, uint16_t value);
    uint16_t get(uint8_t param);
    void send(uint16_t destAddress, uint8_t* payload, uint8_t len);
    uint8_t recv(uint16_t* sourceAddress, uint8_t* payload, uint8_t* len);
    void rgb(uint8_t red, uint8_t green, uint8_t blue);
    uint16_t decodeUi16 ( uint8_t *data );
    void encodeUi16 ( uint16_t from, uint8_t *to );
    uint32_t decodeUi32 ( uint8_t *data );
    void encodeUi32 ( uint32_t from, uint8_t *to );
    float decodeFloat ( uint8_t *data );
    void encodeFloat ( float from, uint8_t *to );

  private:
    uint8_t nodeChannel;
    uint8_t nodeTxPower;
    uint8_t rgbRed;
    uint8_t rgbGreen;
    uint8_t rgbBlue;
};

#endif

