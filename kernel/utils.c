/**
 * @file utils.c
 * @brief Contains SimpleWiNo kernel utils
 * @author Adrien van den Bossche <bossche@irit.fr>
 * @date 20110607
 */

#include "utils.h"


char utilsDec2Char(uint8_t value) {

  if (value <=9) // ((value >= 0) && (value <=9))
    return (char) (value + 0x30);
  else
    return (char) (value + 0x37);
}


uint16_t decodeUint16 ( uint8_t *data ) {

 /**
  * Return a uint16_t based on two uint8_t
  * @author Adrien van den Bossche <bossche@irit.fr>
  * @date 20111123
  */

  return 0 | (data[0] << 8) | data[1];
}


void encodeUint16 ( uint16_t from, uint8_t *to ) {

 /**
  * Place data from at to address
  * No return
  * @author Adrien van den Bossche <bossche@irit.fr>
  * @date 20111011
  */

  to[0] = (from & 0xFF00) >> 8;
  to[1] = from & 0xFF;
}


uint32_t decodeUint32 ( uint8_t *data ) {

 /**
  * Return a uint32_t based on four uint8_t
  * @author Adrien van den Bossche <bossche@irit.fr>
  * @date 20121104
  */

  uint32_t data0, data1, data2, data3;
  data0 = data[0];
  data1 = data[1];
  data2 = data[2];
  data3 = data[3];

  //return 0 | (data[0] << 24) | (data[1] << 16) | (data[2] << 8) | data[3];
  return 0 | (data0 << 24) | (data1 << 16) | (data2 << 8) | data3;
}


void encodeUint32 ( uint32_t from, uint8_t *to ) {

 /**
  * Place data from at to address
  * No return
  * @author Adrien van den Bossche <bossche@irit.fr>
  * @date 20111011
  */

  to[0] = (from & 0xFF000000) >> 24;
  to[1] = (from & 0xFF0000) >> 16;
  to[2] = (from & 0xFF00) >> 8;
  to[3] = from & 0xFF;
}


float decodeFloat ( uint8_t *data ) {

 /**
  * Return a float based on four uint8_t
  * @author Adrien van den Bossche <bossche@irit.fr>
  * @date 20121104
  */

  typedef union _data {
    float f;
    char  s[4];
  } myData;

  myData q;

  q.s[0] = data[0];
  q.s[1] = data[1];
  q.s[2] = data[2];
  q.s[3] = data[3];

  return q.f;
}


void encodeFloat ( float from, uint8_t *to ) {

 /**
  * Place data from at to address
  * No return
  * @author Adrien van den Bossche <bossche@irit.fr>
  * @date 20111011
  */

  typedef union _data {
    float f;
    char  s[4];
  } myData;

  myData q;
  q.f = from;

  to[0] = q.s[0];
  to[1] = q.s[1];
  to[2] = q.s[2];
  to[3] = q.s[3];
}


uint8_t getValueAsciiHex ( uint8_t c ) {

  /**
  * @brief Get the numeric value of an hex ascii char (unsensitive case) 
  * @date 20121117
  * @author Adrien van den Bossche <bossche@irit.fr>
  * @return The uint8_t value
  */

  uint8_t value = 0;

  if ( c < '0' ) {
    return 0;
  } else {
    if (c <= '9') {
      value = c-'0';
    } else {
      if ( c >= 'A' && c <= 'F' ) {
        value = c-'A'+10;
      } else {
        if ( c >= 'a' && c <= 'f' ) {
          value = c-'a'+10;
        } else return 0;
      }
    }
  }

  return value;
}

uint8_t getUint8HexValue ( uint8_t* data ) {

  /**
  * @brief Get the numeric value of an uint8_t in unsensitive case ASCII hex (sscanf(buf,"%x",return) equivalent)
  * @date 20121117
  * @author Adrien van den Bossche <bossche@irit.fr>
  * @return The uint8_t value
  */

/*
  // @todo Il y a un bug quelque part. A tester
  uint8_t value = 0;
  if ( data[1] == 0 ) value = getValueAsciiHex(data[0]);
  else value = 0xFF ; //getValueAsciiHex(data[0])*16 + getValueAsciiHex(data[1]);
  return value;
*/

  uint8_t value, i, j, len;

  value = 0;
  j=1;

  for ( i=0; i<2; i++ )
    if ( data[i] == 0 ) break;
  len = i;/*
  for ( i=0; i<len; i++ ) {

    if ( data[len-1-i] != 0 ) {

      value += getValueAsciiHex(data[len-1-i]) * j;
      j *= 16;
    }
  }*/
  for ( i=0; i<=len; i++ ) {

    if ( data[len-i] != 0 ) {

      value += getValueAsciiHex(data[len-i]) * j;
      j *= 16;
    }
  }

  return value;
}


uint16_t getUint16HexValue ( uint8_t* data ) {

  /**
  * @brief Get the numeric value of an uint16_t in unsensitive case ASCII hex
  * @date 20121117
  * @author Adrien van den Bossche <bossche@irit.fr>
  * @return The uint16_t value
  */

  uint16_t value,j;
  uint8_t i, len;

  value = 0;
  j=1;

  for ( i=0; i<4; i++ )
    if ( data[i] == 0 ) break;
  len = i;
  for ( i=0; i<=len; i++ ) {

    if ( data[len-i] != 0 ) {

      value += getValueAsciiHex(data[len-i]) * j;
      j *= 16;
    }
  }

  return value;
}


uint32_t getUint32HexValue ( uint8_t* data ) {

  /**
  * @brief Get the numeric value of an uint32_t in unsensitive case ASCII hex
  * @date 20121117
  * @author Adrien van den Bossche <bossche@irit.fr>
  * @return The uint32_t value
  */

  uint32_t value,j;
  uint8_t i, len;

  value = 0;
  j=1;

  for ( i=0; i<8; i++ )
    if ( data[i] == 0 ) break;
  len = i;
  for ( i=0; i<=len; i++ ) {

    if ( data[len-i] != 0 ) {

      value += getValueAsciiHex(data[len-i]) * j;
      j *= 16;
    }
  }

  return value;
}


char dec2char ( uint8_t value ) {

  /**
  * @brief Returns the ASCII value of an uint8_t <16
  * @date 20121106
  * @author Adrien van den Bossche <bossche@irit.fr>
  * @return Returns the ASCII value of an uint8_t <16 or NULL if >=16
  */

  if ((value >= 0) && (value <=9))
    return (char) (value + 0x30);
  else
    if (value < 16)
      return (char) (value + 0x37);
  return 0;
}


uint8_t strIsolateUntilSpace ( uint8_t *src, uint8_t *dest ) {

  /**
  * @brief Isolate the begin of the src string in the dest string until there is no space
  * @date 20121117
  * @author Adrien van den Bossche <bossche@irit.fr>
  * @return The length of the dest string
  */

  uint8_t i=0;

  while ( src[i] != ' ' && src[i] != 0 ) {

    dest[i] = src[i];
    i++;
  }

  dest[i] = 0;
  return i;
}


uint8_t strCmpUntilSpace ( uint8_t *str, uint8_t *motif, uint8_t *endPosition ) {

  /**
  * @brief Compare the motif with the begin of the string until a space or the NULL char is encountered
  * @date 20120828
  * @author Adrien van den Bossche <bossche@irit.fr>
  * @return If the motif begins the string until a space or the NULL char is encountered, the position of the space or NULL is returned, -1 otherwise
  */

  uint8_t i=0;

  if ( motif[0] == 0 ) return false;
  if ( str[0] == 0 ) return false;

  while ( 1 ) {

    if (    ( str[i] == ' ' )
         || ( str[i] == 0   ) ) break;

    if (    ( motif[i] == 0 )
         && (    ( str[i] == ' ' )
              || ( str[i] == 0   ) ) ) break;

    if ( str[i] != motif[i] ) return false;

    i++;
  }

  *endPosition = i;
  return true;
}


void strTrim ( uint8_t *str ) {

  /**
  * @brief Suppress space at the begin of the string
  * @date 20120902
  * @author Adrien van den Bossche <bossche@irit.fr>
  * @return No return
  */

  uint8_t i,j;

  i = 0;
  while ( str[i] == ' ' ) i++;

  if ( i != 0 ) {

    j = i-1;

    do {

      j++;
      str[j-i] = str[j];

    } while ( str[j] != '\0' );
  }
}


uint8_t cmpUi32GreaterOrEqualWithRollover ( uint32_t a, uint32_t b ) {

  if ( a & 0x80000000 ) {

    if ( (int32_t)a >= (int32_t)b ) return true;
    else return false;

  } else {

    if ( a >= b ) return true;
    else return false;
  }
}


uint8_t cmpUi16GreaterOrEqualWithRollover ( uint16_t a, uint16_t b ) {

  if ( a & 0x8000 ) {

    if ( (int16_t)a >= (int16_t)b ) return true;
    else return false;

  } else {

    if ( a >= b ) return true;
    else return false;
  }
}


uint8_t cmpUi8GreaterOrEqualWithRollover ( uint8_t a, uint8_t b ) {

  if ( a & 0x80 ) {

    if ( (int8_t)a >= (int8_t)b ) return true;
    else return false;

  } else {

    if ( a >= b ) return true;
    else return false;
  }
}


uint8_t cmpUi32GreaterWithRollover ( uint32_t a, uint32_t b ) {

  if ( a & 0x80000000 ) {

    if ( (int32_t)a > (int32_t)b ) return true;
    else return false;

  } else {

    if ( a > b ) return true;
    else return false;
  }
}


uint8_t cmpUi16GreaterWithRollover ( uint16_t a, uint16_t b ) {

  if ( a & 0x8000 ) {

    if ( (int16_t)a > (int16_t)b ) return true;
    else return false;

  } else {

    if ( a > b ) return true;
    else return false;
  }
}


uint8_t cmpUi8GreaterWithRollover ( uint8_t a, uint8_t b ) {

  if ( a & 0x80 ) {

    if ( (int8_t)a > (int8_t)b ) return true;
    else return false;

  } else {

    if ( a > b ) return true;
    else return false;
  }
}

void makeRandomBytes ( uint8_t* bytes, uint8_t len ) {

  for (int i=0; i<len; i++)
    bytes[i]=random(255);
}

