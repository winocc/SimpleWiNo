/**
 * @file utils.h
 * @brief Contains SimpleWiNo kernel utils headers
 * @author Adrien van den Bossche <bossche@irit.fr>
 * @date 20110607
 */


#ifndef UTILS_H
#define UTILS_H

uint16_t decodeUint16 ( uint8_t *data );
void encodeUint16 ( uint16_t from, uint8_t *to );
uint32_t decodeUint32 ( uint8_t *data );
void encodeUint32 ( uint32_t from, uint8_t *to );
float decodeFloat ( uint8_t *data );
void encodeFloat ( float from, uint8_t *to );

void printInt(uint8_t cData);
char utilsDec2Char(uint8_t value);
void printUint8ToHex(uint8_t cData);
void printUint16ToHex(uint16_t uint16data);
void printUint32ToHex(uint32_t uint32data);
void Uint32ToHexString(uint32_t u32Num, uint8_t *pu8Str);

void strTrim ( uint8_t *str );
uint8_t strCmpUntilSpace ( uint8_t *str, uint8_t *motif, uint8_t *endPosition );
uint8_t strIsolateUntilSpace ( uint8_t *src, uint8_t *dest );
char dec2char ( uint8_t value );
void printUint8ToHex ( uint8_t cData );
void printUint16ToHex ( uint16_t uint16data );
void printUint32ToHex ( uint32_t uint32data );

uint8_t getValueAsciiHex ( uint8_t c );
uint8_t getUint8HexValue ( uint8_t* data );
uint16_t getUint16HexValue ( uint8_t* data );
uint32_t getUint32HexValue ( uint8_t* data );


/**
* @brief compares two uint32_t with rollover (for example 0x0 > 0x80000000 but 0x0 < 0x7FFFFFFF)
* @return true if a >= b, false otherwise
* @author Adrien van den Bossche <bossche@irit.fr>
* @date 20140720
*/
uint8_t cmpUi32GreaterOrEqualWithRollover ( uint32_t a, uint32_t b );


/**
* @brief compares two uint16_t with rollover (for example 0x0 > 0x8000 but 0x0 < 0x7FFF)
* @return true if a >= b, false otherwise
* @author Adrien van den Bossche <bossche@irit.fr>
* @date 20140720
*/
uint8_t cmpUi16GreaterOrEqualWithRollover ( uint16_t a, uint16_t b );


/**
* @brief compares two uint8_t with rollover (for example 0 > 128 but 0 < 127)
* @return true if a >= b, false otherwise
* @author Adrien van den Bossche <bossche@irit.fr>
* @date 20140720
*/
uint8_t cmpUi8GreaterOrEqualWithRollover ( uint8_t a, uint8_t b );


/**
* @brief compares two uint32_t with rollover (for example 0x0 > 0x80000000 but 0x0 < 0x7FFFFFFF)
* @return true if a > b, false otherwise
* @author Adrien van den Bossche <bossche@irit.fr>
* @date 20140720
*/
uint8_t cmpUi32GreaterWithRollover ( uint32_t a, uint32_t b );


/**
* @brief compares two uint16_t with rollover (for example 0x0 > 0x8000 but 0x0 < 0x7FFF)
* @return true if a > b, false otherwise
* @author Adrien van den Bossche <bossche@irit.fr>
* @date 20140720
*/
uint8_t cmpUi16GreaterWithRollover ( uint16_t a, uint16_t b );


/**
* @brief compares two uint8_t with rollover (for example 0 > 128 but 0 < 127)
* @return true if a > b, false otherwise
* @author Adrien van den Bossche <bossche@irit.fr>
* @date 20140720
*/
uint8_t cmpUi8GreaterWithRollover ( uint8_t a, uint8_t b );

void makeRandomBytes ( uint8_t* bytes, uint8_t len );

#endif //UTILS_H

