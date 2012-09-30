// PipeCommunicator.h
// (c) 2012 Marko Lalic

// EtfArduino may be freely distributed under the MIT license.
// For all details and documentation:
// http://github.com/mlalic/etfarduino/

#ifndef __PIPECOMMUNICATOR_H__
#define __PIPECOMMUNICATOR_H__
#include <windows.h>
class PipeCommunicator
{
public:
	typedef int message_t;

	static DWORD const bufferSize = 512;

	PipeCommunicator(LPCWSTR pipeName);
	~PipeCommunicator();

	bool Connect();
	bool SendMessage(LPCVOID buffer, DWORD bufferSize);
	bool RetrieveMessage(LPVOID buffer, DWORD bufferSize, LPDWORD bytesRead);
private:
	LPCWSTR pipeName;
	HANDLE hPipe;
	bool connected;
	// Private member functions
	bool InitializeConnection();
};

#endif