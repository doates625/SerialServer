/**
 * @file SerialServer.h
 * @brief Class for exchanging and processing serial data packets on Arduinos
 * @author Dan Oates (WPI Class of 2020)
 */
#include <Platform.h>
using Platform::serial_t;

/**
 * Macro Definition Checks
 */

// Max number of unique TX message IDs
#if !defined(SERIALSERVER_TX_MAX_IDS) || SERIALSERVER_TX_MAX_IDS < 1
	#error Must define SERIALSERVER_TX_MAX_IDS >= 1
#endif

// Max number of unique RX message IDs
#if !defined(SERIALSERVER_RX_MAX_IDS) || SERIALSERVER_RX_MAX_IDS < 1
	#error Must define SERIALSERVER_RX_MAX_IDS >= 1
#endif

// Max data length of TX message
#if !defined(SERIALSERVER_TX_MAX_LEN) || SERIALSERVER_TX_MAX_LEN < 1
	#error Must define SERIALSERVER_TX_MAX_LEN >= 1
#endif

// Max data length of RX message
#if !defined(SERIALSERVER_RX_MAX_LEN) || SERIALSERVER_RX_MAX_LEN < 1
	#error Must define SERIALSERVER_RX_MAX_LEN >= 1
#endif

/**
 * Class Declaration
 */
class SerialServer
{
public:
	SerialServer(serial_t* serial, uint8_t start_byte = 0x00);
	void add_tx(uint8_t msg_id, uint8_t data_len, void (*func)(uint8_t*));
	void add_rx(uint8_t msg_id, uint8_t data_len, void (*func)(uint8_t*));
	void tx(uint8_t msg_id);
	void tx();
	void rx();
protected:
	serial_t* serial;
	uint8_t start_byte;
	uint8_t num_tx;
	uint8_t num_rx;
	uint8_t tx_ids[SERIALSERVER_TX_MAX_IDS];
	uint8_t rx_ids[SERIALSERVER_RX_MAX_IDS];
	uint8_t tx_data_lens[SERIALSERVER_TX_MAX_IDS];
	uint8_t rx_data_lens[SERIALSERVER_RX_MAX_IDS];
	void (*tx_funcs[SERIALSERVER_TX_MAX_IDS])(uint8_t*);
	void (*rx_funcs[SERIALSERVER_RX_MAX_IDS])(uint8_t*);
	uint8_t tx_data[SERIALSERVER_TX_MAX_LEN];
	uint8_t rx_data[SERIALSERVER_RX_MAX_LEN];
	void tx_index(uint8_t tx_i);
	void flush();
private:
	SerialServer(const SerialServer&);
    SerialServer& operator=(const SerialServer&);
};
