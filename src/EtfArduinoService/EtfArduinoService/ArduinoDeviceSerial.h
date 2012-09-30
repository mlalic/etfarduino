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
	response_t GetSingleValue();
	bool SetSampleRate(int sampleRate);
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
	SerialCommunicator serial;
	CriticalSection cs;
	ArduinoDeviceSerial(ArduinoDeviceSerial const&);
	ArduinoDeviceSerial& operator=(ArduinoDeviceSerial const&);

};

#endif