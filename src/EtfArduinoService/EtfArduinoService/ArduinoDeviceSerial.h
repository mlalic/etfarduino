// ArduinoDeviceSerial.h
// (c) 2012 Marko Lalic

// EtfArduino may be freely distributed under the MIT license.
// For all details and documentation:
// http://github.com/mlalic/etfarduino/

#ifndef __ARDUINODEVICESERIAL_H__
#define __ARDUINODEVICESERIAL_H__

#include "ArduinoDevice.h"
#include <windows.h>
#include <tchar.h>

class ArduinoDeviceSerial : public ArduinoDevice {
public:
	ArduinoDeviceSerial(UINT deviceId, LPCTSTR  serialPortName);
	ArduinoDeviceSerial(UINT deviceId, LPCWSTR  serialPortName, DWORD bufferSize);
	~ArduinoDeviceSerial();

	bool StartAcquisition();
	bool StopAcquisition();
	response_t GetSingleValue(int channelId);
	bool SetSampleRate(int sampleRate);
	bool SetInputChannelList(std::vector<int> const& channels);
	bool SetBufferSize(int bufferSize);
	bool SendDigitalValue(int line, int value);
	bool PutSingleValue(int channel, int value);

	// Returns the next buffer which contains data collected from the Arduino.
	// It is the callers responsibility to free the memory of the returned buffer.
	response_t* GetNextBuffer();

	LPCTSTR GetPortName() const;
private:
	TCHAR serialPortName[10];
	int const tickFrequency;
	volatile LONG acquisitionActive;
	int const maxInputChannels;
	SerialCommunicator serial;
	CriticalSection cs;
	
	// Forbid copying
	ArduinoDeviceSerial(ArduinoDeviceSerial const&);
	ArduinoDeviceSerial& operator=(ArduinoDeviceSerial const&);

	// Statics
	static char const START_ACQUISITION = '1';
	static char const STOP_ACQUISITION = '2';
	static char const SET_SAMPLE_RATE = '3';
	static char const GET_SINGLE_VALUE = '4';
	static char const SEND_DIGITAL_VALUE = '5';
	static char const PUT_SINGLE_VALUE = '6';
	static char const SET_INPUT_CHANNEL_LIST = '7';

};

#endif