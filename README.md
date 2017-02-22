# SimpleWiNo

A set of useful functions to use communications abilities of the WiNos boards.

## General functions

Initialisation of the object SimpleWiNo :

```c
void init();
```

MAC and PHY engines :

```c
void process();
```

Write a property :

```c
void set(uint8_t param, uint16_t value);
```

Read a property :

```c
uint16_t get(uint8_t param);
```

List of main properties usable :

- NODE_SHORT_ADDRESS : WiNo address (16 bits,0xFFFF for broadcast)
- NODE_PANID : WinNo's cluster's address (16 bits)
- NODE_TXPOWER : Tx power (3 bits, 0-7)
- NODE_CHANNEL : channel (frequency to be used (0-17))
- PHY_DEBUG : activate/deactivate verbosity for PHY layer
- MAC_DEBUG : activate/deactivate verbosity for MAC layer


## Let's communicate !
 
Ask MAC layer to send a data packet (N-PDU) with :
- payload : packet itself as an octets table
- len : payload length
- destAddress : recipient address

```c
void send(uint16_t destAddress, uint8_t* payload, uint8_t len);
```

Test and receive packets arrived on the MAC layer
- payload : packet itself as an octets table
- len : pointeur to packet length
- sourceAddress : pointeur to source address
- return 1 if a packet has been received, 0 else

```c
uint8_t recv(uint16_t* sourceAddress, uint8_t* payload, uint8_t* len);
```

## Going deeper : create and read messages

Obtain an unisgned 16 bits integer from an octet table :

```c
uint16_t decodeUi16 ( uint8_t *data );
```

Write an unsigned 16 bits integer in an octet table :

```c
void encodeUi16 ( uint16_t from, uint8_t *to );
```

Obtain an unsigned 32 bits integer from an octet table :

```c
uint32_t decodeUi32 ( uint8_t *data );
```

Write an unsigned 32 bits integer in an octet table :

```c
void encodeUi32 ( uint32_t from, uint8_t *to );
```

Obtain a float from a table :

```c
float decodeFloat ( uint8_t *data );
```

Write a float to a table :

```c
void encodeFloat ( float from, uint8_t *to );
```