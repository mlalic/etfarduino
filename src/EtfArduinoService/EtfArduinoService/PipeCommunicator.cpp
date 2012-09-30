// PipeCommunicator.cpp
// (c) 2012 Marko Lalic

// EtfArduino may be freely distributed under the MIT license.
// For all details and documentation:
// http://github.com/mlalic/etfarduino/

#include "stdafx.h"
#include "PipeCommunicator.h"
#include <cstdio>		// Debug
PipeCommunicator::PipeCommunicator(LPCWSTR pipeName) :
	pipeName(pipeName), hPipe(0), connected(false) {
		// Create a new pipe instance
		InitializeConnection();
		printf("Created a pipe instance\n");
}

PipeCommunicator::~PipeCommunicator() {
	// RAII
	printf("Disconnecting pipe...\n");
	if (connected) {
		DisconnectNamedPipe(hPipe);
	}
	if (hPipe != NULL)
		CloseHandle(hPipe);
}

bool PipeCommunicator::InitializeConnection() {
	hPipe = CreateNamedPipe( 
		pipeName,						// pipe name 
		PIPE_ACCESS_DUPLEX,				// read/write access 
		PIPE_TYPE_MESSAGE |				// message type pipe 
			PIPE_READMODE_MESSAGE |		// message-read mode 
			PIPE_WAIT,					// blocking mode 
		PIPE_UNLIMITED_INSTANCES,		// max. instances  
		bufferSize,						// output buffer size 
		bufferSize,						// input buffer size 
		0,								// client time-out 
		NULL);							// default security attribute 

	if (hPipe == INVALID_HANDLE_VALUE) 
	{
		// CreateNamedPipe failed!
		hPipe = 0;
		return false;
	}
	return true;
}

bool PipeCommunicator::Connect() {
	printf("Waiting for client...\n");
	if (hPipe == 0) {
		return false;
	}
	if (connected)
		return false;
	connected = ConnectNamedPipe(hPipe, NULL) == TRUE ? 
		true : (GetLastError() == ERROR_PIPE_CONNECTED);
	return connected;
}

bool PipeCommunicator::SendMessage(LPCVOID buffer, DWORD bufferSize) {
	if (hPipe == 0) {
		return false;
	}
	if (!connected) {
		return false;
	}
	DWORD written = 0;
	printf("Writing to pipe...");
	if (!WriteFile(hPipe, buffer, bufferSize, &written, NULL)) {
		return false;
	}
	if (written != bufferSize) {
		return false;
	}
	printf("Flushing the buffer...");
	// Flush the buffer
	if (!FlushFileBuffers(hPipe)) {
		return false;
	}
	printf("done.\n");
	return true;
}
bool PipeCommunicator::RetrieveMessage(LPVOID buffer, DWORD bufferSize, LPDWORD bytesRead) {
	if (hPipe == 0) {
		return false;
	}
	if (!connected) {
		return false;
	}
	if (!ReadFile(hPipe, buffer, bufferSize, bytesRead, NULL)) {
		return false;
	}
	return true;
}
