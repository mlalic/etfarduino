// ArduinoDeviceSerial.cpp
// (c) 2012 Marko Lalic

// EtfArduino may be freely distributed under the MIT license.
// For all details and documentation:
// http://github.com/mlalic/etfarduino/

#include "stdafx.h"
#include "ArduinoDeviceSerial.h"
#include "MessageWindowSender.h"
#include "Lock.h"
#include <tchar.h>
ArduinoDeviceSerial::ArduinoDeviceSerial(UINT deviceId, LPCTSTR serialPortName) :
  ArduinoDevice(deviceId), tickFrequency(250000), acquisitionActive(0), serial(serialPortName) {
	  printf("Creating arduino device...");
	  memset(ArduinoDeviceSerial::serialPortName,
		     0,
			 sizeof ArduinoDeviceSerial::serialPortName);
	  for (size_t i = 0; i < _tcsclen(serialPortName) || i < 10; ++i) {
		  ArduinoDeviceSerial::serialPortName[i] = serialPortName[i];
	  }
	  printf("Done.\n");
}

ArduinoDeviceSerial::ArduinoDeviceSerial(UINT deviceId, LPCWSTR serialPortName, DWORD bufferSize) :
  ArduinoDevice(deviceId, bufferSize), tickFrequency(250000), acquisitionActive(0),
  serial(serialPortName) {
	  memset(ArduinoDeviceSerial::serialPortName,
		     0,
			 sizeof ArduinoDeviceSerial::serialPortName);
	  for (int i = 0; i < _tcsclen(serialPortName) && i < 10; ++i) {
		  ArduinoDeviceSerial::serialPortName[i] = serialPortName[i];
	  }
}

ArduinoDeviceSerial::~ArduinoDeviceSerial() {
	// Empty
}

bool ArduinoDeviceSerial::StartAcquisition() {
	// Must use Interlocked* API because it is necessary to read/check/set
	// the variable atomically. Volatile does not provide that!
	if (InterlockedCompareExchange(&acquisitionActive, 1, 0) != 0) {
		// Another thread already holds the acquisition active!
		return false;
	}
	// Create a message to be sent
	char buffer[1];
	buffer[0] = '1';
	// Now send it
	// SerialCommunicator serial(serialPortName);
	{
		// Critical section
		Lock lock(cs);
		// Clear anything that may be in the receive buffer
		// because a new data acquisition process is starting
		// and all data in the buffer must be stale.
		serial.Purge();
		if (!serial.SendMessage(buffer, sizeof buffer)) {
			acquisitionActive = 0;
			return false;
		}
	}
	bufferQueue.Clear();
	// Initialize the object used to signal the adaptor that the buffer is full
	MessageWindowSender msgWinSender(deviceId, MessageWindowSender::SUBSYSTEM_AI);
	// This makes the call block for as long as the acquisition stays active.
	// acquisitionActive is a volatile variable which ON VC++08 IS GUARANTEED
	// to have Acquire/Release memory barrier semantics for Read/Write.
	while (acquisitionActive == 1) {
		DWORD bytesRead = 0;
		response_t* values = new response_t[bufferSize];
		if (serial.RetrieveMessage(values, bufferSize * sizeof(response_t), &bytesRead)) {
			// :TODO: Exception safety: push can throw, leaking the values buffer...
			bufferQueue.Push(values);
			printf("----Retrieved: %d\n", values[1]);
			// Send notification to the MessageWindow that this device has filled another buffer.
			msgWinSender.NotifyBufferFull();
		} else {
			delete[] values;
		}
	}
	// Send a stop message to arduino
	buffer[0] = '2';
	bool status = serial.SendMessage(buffer, sizeof buffer);
	acquisitionActive = 0;
	return status;
}

bool ArduinoDeviceSerial::StopAcquisition() {
	// Sets the flag to indicate the acquisition should stop
	if (InterlockedCompareExchange(&acquisitionActive, 2, 1) != 1)
		return false;
	return true;
}

ArduinoDeviceSerial::response_t ArduinoDeviceSerial::GetSingleValue() {
	if (acquisitionActive) {
		return -1;
	}
	char buffer[1];
	buffer[0] = '4';
	{
		// Critical section
		Lock lock(cs);
		// Upon acquiring the lock need to check if acquisition is
		// still inactive (it might have started while the thread
		// was waiting for the lock) (double checked locking)
		if (acquisitionActive) {
			return -1;
		}
		// If acquisition hasn't yet started, the lock guarantees
		// that this operation will not overlap with the acquisition.
		// The acquisition may only start after the lock is released.
//		SerialCommunicator serial(serialPortName);
		if (!serial.SendMessage(buffer, sizeof buffer)) {
			return -1;
		}
		DWORD bytesRead = 0;
		response_t value = 0;
		serial.RetrieveMessage(&value, sizeof(response_t), &bytesRead);
		return value;
	}
}

ArduinoDeviceSerial::response_t* ArduinoDeviceSerial::GetNextBuffer() {
	// No need to copy the data to another array since after this call
	// the ownership of the buffer will be transferred to the caller
	// anyway. (The caller will have to free some memory in any case.)
	response_t* buffer = bufferQueue.Pop();
	return buffer;
}

bool ArduinoDeviceSerial::SetSampleRate(int sampleRate) {
	if (acquisitionActive) {
		return false;
	}
	// Convert the sample rate into a sampling period
	// The internal clock has a resolution of 4us (for 16Mhz boards) so the
	// period always has to be a multiple of 4.
	// The algorithm is to find the factor by which to multiply 4us
	// using round(tickFrequency / SR)
	int const factor = (static_cast<double>(tickFrequency) / sampleRate) + 0.5;
	int const period = 4 * factor;
	// Send the period
	char buffer[1 + 4];
	buffer[0] = '3';
	buffer[1] = period;			// Truncated to least significant byte
	buffer[2] = period >> 8;
	buffer[3] = period >> 16;
	buffer[4] = period >> 24;
	{
		// Critical section
		Lock lock(cs);
		// Double checked locking
		if (acquisitionActive) {
			return false;
		}
//		SerialCommunicator serial(serialPortName);
		if (!serial.SendMessage(buffer, sizeof buffer)) {
			return false;
		}
	}
	return true;
}

bool ArduinoDeviceSerial::SetBufferSize(int bufferSize) {
	if (acquisitionActive) {
		return false;
	}
	// Even this needs a lock since a context switch
	// right before this is executed can lead to
	// a start of acquisition with the wrong buffer size, since
	// the original set buffer size from the calling process is
	// fully async, meaning it does not wait for any confirmation
	// before continuing and is free to call start right after!
	{
		Lock lock(cs);
		if (acquisitionActive) {
			return false;
		}
		ArduinoDeviceSerial::bufferSize = bufferSize;
	}
	return true;
}

bool ArduinoDeviceSerial::SendDigitalValue(int line, int value) {
	char buffer[3];
	buffer[0] = '5';
	buffer[1] = line;
	buffer[2] = value ? 1 : 0;
	printf("Buffer: ");
	for (int i = 0; i < 3; ++i) printf("%d ", buffer[i]); printf("\n");
	{
		// Critical section
		Lock lock(cs);
		if (!serial.SendMessage(buffer, sizeof buffer))
			return false;
	}
	return true;
}

bool ArduinoDeviceSerial::PutSingleValue(int channel, int value) {
	unsigned char buffer[3];
	buffer[0] = '6';
	buffer[1] = channel;
	buffer[2] = value;
	{
		// Critical section
		Lock lock(cs);
		if (!serial.SendMessage(buffer, sizeof buffer))
			return false;
	}
	return true;
}

LPCTSTR ArduinoDeviceSerial::GetPortName() const {
	return serialPortName;
}