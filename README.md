# SimpleWiNo

A set of useful functions to use communications abailities of the WiNos boards.

## General functions

Initialisation of the object SimpleWiNo :

void init();

MAC and PHY engines :

void process();

Write a property :

void set(uint8_t param, uint16_t value);

Read a property :

uint16_t get(uint8_t param);

List of main properties usable :

- NODE_SHORT_ADDRESS : WiNo address (16 bits,0xFFFF for broadcast)
- NODE_PANID : WinNo's cluster's address (16 bits)
- NODE_TXPOWER : Tx power (3 bits, 0-7)
- NODE_CHANNEL : channel (frequency to be used (0-17))
- PHY_DEBUG : activate/deactivate verbosity for PHY layer
- MAC_DEBUG : activate/deactivate verbosity for MAC layer


///////// Primitives de communication /////////

// Charger la couche MAC de transmettre un paquet (N-PDU).
//   payload : le paquet sous forme d'un tableau d'octets
//   len : la longueur du paquet
//   destAddress : l'adresse du destinataire
//   pas de retour
void send(uint16_t destAddress, uint8_t* payload, uint8_t len);

// Tester et récupérer des paquets reçue par la couche MAC
//   payload : le paquet sous forme d'un tableau d'octets
//   len : le pointeur vers la longueur du paquet
//   sourceAddress : le pointeur vers l'adresse de la source
// recv retourne 1 si un paquet a été reçu, 0 sinon
uint8_t recv(uint16_t* sourceAddress, uint8_t* payload, uint8_t* len);


/////// Primitives d'aide à la création et à l'interprétation de messages ///////

// Récupérer un entier 16 bits non-signé depuis un tableau d'octet
uint16_t decodeUi16 ( uint8_t *data );

// Ecrire un entier 16 bits non-signé dans un tableau d'octet
void encodeUi16 ( uint16_t from, uint8_t *to );

// Récupérer un entier 32 bits non-signé depuis un tableau d'octet
uint32_t decodeUi32 ( uint8_t *data );

// Ecrire un entier 32 bits non-signé dans un tableau d'octet
void encodeUi32 ( uint32_t from, uint8_t *to );

// Récupérer un flotant depuis un tableau d'octet
float decodeFloat ( uint8_t *data );

// Ecrire un entier 32 bits non-signé dans un tableau d'octet
void encodeFloat ( float from, uint8_t *to );
