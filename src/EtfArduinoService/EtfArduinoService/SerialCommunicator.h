// SerialCommunicator.h
// (c) 2012 Marko Lalic

// EtfArduino may be freely distributed under the MIT license.
// For all details and documentation:
// http://github.com/mlalic/etfarduino/

#ifndef __SERIALCOMMUNICATOR_H__
#define __SERIALCOMMUNICATOR_H__
#include <windows.h>
class SerialCommunicator {
public:
	SerialCommunicator(LPCWSTR serialPortName);
	virtual ~SerialCommunicator();

	bool Purge();
	bool SendMessage(LPCVOID buffer, DWORD bufferSize);
	bool RetrieveMessage(LPVOID buffer, DWORD bufferSize, LPDWORD bytesRead);
private:
	HANDLE hSerial;
	TCHAR serialPortName[10];
	// Private member functions
	bool InitializeSerial();
};

#endif