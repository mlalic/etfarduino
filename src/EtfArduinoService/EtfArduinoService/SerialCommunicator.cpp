// SerialCommunicator.cpp
// (c) 2012 Marko Lalic

// EtfArduino may be freely distributed under the MIT license.
// For all details and documentation:
// http://github.com/mlalic/etfarduino/

#include "stdafx.h"
#include "SerialCommunicator.h"
#include <tchar.h>
#include <cstdio>		// Debug
SerialCommunicator::SerialCommunicator(LPCWSTR serialPortName) :
  hSerial(0) {
	memset(SerialCommunicator::serialPortName, 0, sizeof SerialCommunicator::serialPortName);
	for (size_t i = 0; i < _tcsclen(serialPortName) && i < 10; ++i) {
		SerialCommunicator::serialPortName[i] = serialPortName[i];
	}
	InitializeSerial();
}

SerialCommunicator::~SerialCommunicator() {
	// RAII
	if (hSerial != NULL)
		CloseHandle(hSerial);
}

bool SerialCommunicator::Purge() {
	if (hSerial == 0) {
		return false;
	}
	return 
		PurgeComm(hSerial, PURGE_RXCLEAR | PURGE_RXABORT) == TRUE;
}

bool SerialCommunicator::SendMessage(LPCVOID buffer, DWORD bufferSize) {
	if (hSerial == 0) {
		// Serial port not properly initialized.
		return false;
	}
	OVERLAPPED overlappedWrite = {0};
	overlappedWrite.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	if (overlappedWrite.hEvent == NULL)
		return false;

	DWORD written = 0;
	bool success = true;
	if (!WriteFile(hSerial, buffer, bufferSize, &written, &overlappedWrite)) {
		if (GetLastError() != ERROR_IO_PENDING) {
			success = false;
		} else {
			// Write pending, wait as long as it takes
			if (!GetOverlappedResult(hSerial, &overlappedWrite, &written, TRUE)) {
				success = false;
			}
		}
	}
	CloseHandle(overlappedWrite.hEvent);
	if (written != bufferSize) success = false;
	return success;
}


bool SerialCommunicator::RetrieveMessage(LPVOID buffer, DWORD bufferSize, LPDWORD bytesRead) {
	if (hSerial == 0) {
		return false;
	}
	// Init overlapped structs
	bool waitingOnRead = false;
	OVERLAPPED overlappedReader = {0};
	overlappedReader.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	if (overlappedReader.hEvent == NULL)
		return false;

	if (!ReadFile(hSerial, buffer, bufferSize, bytesRead, &overlappedReader)) {
		if (GetLastError() == ERROR_IO_PENDING)
			waitingOnRead = true;
	} else {
		// Operation completed immediately
		CloseHandle(overlappedReader.hEvent);
		return true;
	}
	// Reads are allowed to timeout after 1s
	int const readTimeout = 1000;
	bool success = true;
	if (waitingOnRead) {
		DWORD returnEvent = WaitForSingleObject(overlappedReader.hEvent, readTimeout);
		switch (returnEvent) {
		case WAIT_OBJECT_0:
			if (!GetOverlappedResult(hSerial, &overlappedReader, bytesRead, FALSE)) {
				success = false;
			}
			break;
		case WAIT_TIMEOUT:
			success = false;
			break;
		default:
			success = false;
			break;
		}
	}
	CloseHandle(overlappedReader.hEvent);
	return success;
}

bool SerialCommunicator::InitializeSerial() {
	hSerial =
		CreateFile(serialPortName,
				   GENERIC_READ | GENERIC_WRITE,
				   0,
				   0,
				   OPEN_EXISTING,
				   FILE_FLAG_OVERLAPPED,
				   0);
	if (hSerial == INVALID_HANDLE_VALUE)
	{
		if (GetLastError() == ERROR_FILE_NOT_FOUND) {
			printf("Serial port does not exist\n");
		}
		printf("Problem initializing serial port communication\n");
		hSerial = 0;
		return false;
	}

	DCB dcbSerialParams = {0};
	dcbSerialParams.DCBlength = sizeof(dcbSerialParams);
	dcbSerialParams.BaudRate = CBR_256000;
	dcbSerialParams.ByteSize = 8;
	dcbSerialParams.StopBits = ONESTOPBIT;
	dcbSerialParams.Parity = NOPARITY;
	if(!SetCommState(hSerial, &dcbSerialParams)) {
		printf("Error setting communication parameters.\n");
		CloseHandle(hSerial);
		hSerial = 0;
		return false;
	}
	
	COMMTIMEOUTS timeouts = {0};
	timeouts.ReadIntervalTimeout = 50;
	timeouts.ReadTotalTimeoutConstant = 50;
	timeouts.ReadTotalTimeoutMultiplier = 10;
	timeouts.WriteTotalTimeoutConstant = 50;
	timeouts.WriteTotalTimeoutMultiplier = 10;
	if(!SetCommTimeouts(hSerial, &timeouts)){
		printf("Error setting timeouts\n");
		CloseHandle(hSerial);
		hSerial = 0;
		return false;
	}
	// Discard anything that's possibly in any buffer.
	PurgeComm(hSerial, PURGE_RXCLEAR | PURGE_TXCLEAR | PURGE_RXABORT);
	// Everything was initialized successfully
	return true;
}