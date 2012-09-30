// ArduinoDevice.h
// (c) 2012 Marko Lalic

// EtfArduino may be freely distributed under the MIT license.
// For all details and documentation:
// http://github.com/mlalic/etfarduino/

#ifndef __ARDUINODEVICE_H__
#define __ARDUINODEVICE_H__
#include <windows.h>
#include <queue>
#include "BufferQueue.h"
#include "SerialCommunicator.h"
#include "CriticalSection.h"

class ArduinoDevice {
public:
	typedef unsigned short response_t;

	ArduinoDevice(UINT deviceId);
	ArduinoDevice(UINT deviceId, DWORD bufferSize);
	virtual ~ArduinoDevice();

	virtual bool StartAcquisition() = 0;
	virtual bool StopAcquisition() = 0;
	virtual response_t GetSingleValue() = 0;
	virtual bool SetSampleRate(int sampleRate) = 0;
	virtual bool SetBufferSize(int bufferSize) = 0;
	virtual bool SendDigitalValue(int line, int value) = 0;
	virtual bool PutSingleValue(int channel, int value) = 0;

	// Returns the next buffer which contains data collected from the Arduino.
	// It is the callers responsibility to free the memory of the returned buffer.
	virtual response_t* GetNextBuffer() = 0;

	DWORD GetBufferSize() const;
	UINT  GetDeviceId()   const;
	virtual LPCTSTR GetPortName() const = 0;

protected:
	// :TODO: Refactor to move everything to private:
	//   only bufferQueue needs more work...
	UINT deviceId;
	DWORD bufferSize;
	BufferQueue bufferQueue;
	// Statics
	static DWORD const DEFAULT_BUFFER_SIZE = 64;
private:
	// Disallow copying!
	ArduinoDevice(ArduinoDevice const& rhs);
	ArduinoDevice& operator=(ArduinoDevice const& rhs);

};

#endif