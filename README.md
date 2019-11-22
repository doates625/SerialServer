# SerialServer
Class for exchanging and processing serial data packets on Arduinos  
Written by Dan Oates (WPI Class of 2020)

### Description

#### Packet Protocol
This class implements a bare-bones serial packet protocol system on Arduino. A serial server is attached to one serial port and consists of TX and RX message definitions. A message definition consists of:

- Message ID
- Length of data (bytes)
- TX packer function (TX only)
- RX unpacker function (RX only)

The TX packer function takes a pointer to the TX data buffer as an argument and is called to write the buffer before it is sent over serial. The RX unpacker function takes the RX data buffer as an argument and is called after a message of the corresponding ID is received to process the incoming data. A single message ID can have both a TX and RX protocol.
  
A serial message consists of the following:

- Start delimeter (1 byte)
- Message ID (1 byte)
- Data (N bytes)
- Checksum (1 byte)

Servers have a TX method which calls the corresponding TX packer function then sends the packet. There is also a TX method with no arguments which transmits one message for all TX message definitions. The RX method parses the entire incoming serial buffer for messages and calls the corresponding RX unpacker functions for messages with valid start bytes, message IDs, and checksums. Messages with invalid checksums or bytes not received within a timeout are ignored.

#### Macros
This class requires 4 macros be defined by the pre-processor:

- SERIALSERVER_TX_MAX_IDS: Max number of TX message definitions
- SERIALSERVER_RX_MAX_IDS: Max number of RX message definitions
- SERIALSERVER_TX_MAX_LEN: Max data length of TX message
- SERIALSERVER_RX_MAX_LEN: Max data length of RX message

Each of these macros must be defined as an integer greater than 0.

### Dependencies
- [Platform](https://github.com/doates625/Platform.git)
- [Timer](https://github.com/doates625/Timer.git)