/**
 * @file SerialServer.cpp
 * @author Dan Oates (WPI Class of 2020)
 */
#include "SerialServer.h"
#include <CppUtil.h>
#include <Timer.h>
using Platform::serial_t;
using CppUtil::find;

/**
 * @brief Constructor for Serial Server
 * @param serial Pointer to serial port interface
 * @param start_byte Message start byte
 * @param timeout Inter-byte read timeout [s]
 */
SerialServer::SerialServer(serial_t* serial, uint8_t start_byte, float timeout)
{
	this->serial = serial;
	this->start_byte = start_byte;
	this->num_tx = 0;
	this->num_rx = 0;
	this->timeout = timeout;
}

/**
 * @brief Adds transmission message to server
 * @param msg_id Message ID
 * @param data_len Data length [bytes]
 * @param func Data packing function
 */
void SerialServer::add_tx(
	uint8_t msg_id,
	uint8_t data_len,
	void (*func)(uint8_t*))
{
	tx_ids[num_tx] = msg_id;
	tx_data_lens[num_tx] = data_len;
	tx_funcs[num_tx] = func;
	num_tx++;
}

/**
 * @brief Adds receiver message to server
 * @param msg_id Message ID
 * @param data_len Data length [bytes]
 * @param func Data unpacking function
 */
void SerialServer::add_rx(
	uint8_t msg_id,
	uint8_t data_len,
	void (*func)(uint8_t*))
{
	rx_ids[num_rx] = msg_id;
	rx_data_lens[num_rx] = data_len;
	rx_funcs[num_rx] = func;
	num_rx++;
}

/**
 * @brief Transmits message with given ID
 * @param msg_id Message ID to transmit
 */
void SerialServer::tx(uint8_t msg_id)
{
	bool found_id = false;
	uint8_t tx_i = find(tx_ids, msg_id, num_tx, found_id);
	if (found_id)
	{
		tx_index(tx_i);
	}
}

/**
 * @brief Transmits all messages in server once
 */
void SerialServer::tx()
{
	for (uint8_t tx_i = 0; tx_i < num_tx; tx_i++)
	{
		tx_index(tx_i);
	}
}

/**
 * @brief Processes all incoming serial messages
 */
void SerialServer::rx()
{
	while (serial->available())
	{
		// Check for start byte
		if (serial->read() == start_byte)
		{
			// Find message by ID
			if (!wait())
			{
				flush();
				return;
			}
			uint8_t msg_id = serial->read();
			bool found_id = false;
			uint8_t rx_i = find(rx_ids, msg_id, num_rx, found_id);
			if (found_id)
			{
				// Read message data and calculate checksum
				uint8_t checksum = 0x00;
				for (uint8_t d = 0; d < rx_data_lens[rx_i]; d++)
				{
					if (!wait())
					{
						flush();
						return;
					}
					uint8_t byte = serial->read();
					rx_data[d] = byte;
					checksum += byte;
				}

				// Process packet if checksum is correct
				if (!wait())
				{
					flush();
					return;
				}
				if (serial->read() == checksum)
				{
					rx_funcs[rx_i](rx_data);
				}
			}
			else
			{
				flush();
				return;
			}
		}
		else
		{
			flush();
			return;
		}
	}
}

/**
 * @brief Transmits message with given index
 * @param tx_i Index of message in server (NOT message ID)
 */
void SerialServer::tx_index(uint8_t tx_i)
{
	// Call data packing function
	tx_funcs[tx_i](tx_data);

	// Write start byte and message ID
	serial->write(start_byte);
	serial->write(tx_ids[tx_i]);

	// Write data and calculate checksum
	uint8_t checksum = 0x00;
	for (uint8_t d = 0; d < tx_data_lens[tx_i]; d++)
	{
		uint8_t byte = tx_data[d];
		serial->write(byte);
		checksum += byte;
	}

	// Write checksum
	serial->write(checksum);
}

/**
 * @brief Waits for byte in input buffer
 * @return True if received within timeout, false otherwise
 */
bool SerialServer::wait()
{
	Timer timer;
	timer.start();
	while (!serial->available())
	{
		if (timer.read_ms() > 100)
		{
			return false;
		}
	}
	return true;
}

/**
 * @brief Flushes input serial buffer
 */
void SerialServer::flush()
{
	while (serial->available())
	{
		serial->read();
	}
}
