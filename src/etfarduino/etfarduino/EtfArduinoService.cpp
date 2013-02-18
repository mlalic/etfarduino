// EtfArduinoService.cpp
// (c) 2012 Marko Lalic

// EtfArduino may be freely distributed under the MIT license.
// For all details and documentation:
// http://github.com/mlalic/etfarduino/

#include "stdafx.h"
#include "EtfArduinoService.h"
#include <algorithm>
#include <new>

LPTSTR const EtfArduinoService::PIPENAME = TEXT("\\\\.\\pipe\\etfarduino-{522590F9-C51E-4711-B6E2-9CECC7C3FD91}");
DWORD const EtfArduinoService::MessageBufferSize = 16;

EtfArduinoService::EtfArduinoService() : hPipe(INVALID_HANDLE_VALUE) {
	// RAII
	OpenPipeConnection();
}

EtfArduinoService::~EtfArduinoService() {
	// RAII
	ClosePipeConnection();
}

bool EtfArduinoService::GetBufferedData(
		UINT	deviceId,
		LPVOID	ptrBuffer,
		DWORD	bufferSize) {
	// Formirati poruku koju treba poslati serveru
	message_t  messageBuffer[MessageBufferSize];
	memset(messageBuffer, 0, sizeof messageBuffer);
	// Prvi je kod komande
	messageBuffer[0] = GET_BUFFERED_DATA;
	// Zatim slijede parametri:
	// Prvi parametar je DeviceId za koji se traze vrijednosti
	messageBuffer[1] = deviceId;
	// Inicijalizirati strukture koje primaju odgovor
	// Za sad se odgovor prvo preuzima u privremeni buffer, a
	// zatim kopira u buffer koji je proslijedio klijent.
	// Ovo je najlakse za promijeniti ako kasnije skontam da nije potreban
	// medjubuffer...
	DWORD numReturnBytes = 0;
	response_t* replyBuffer = 0;
	try {
		replyBuffer = new response_t[bufferSize];
	} catch (std::bad_alloc) {
		// Nemoguce primiti odgovor obzirom da buffer nije alociran.
		return false;
	}
	if (hPipe == 0)
		if (!OpenPipeConnection())
			return false;
	if (!TransactNamedPipe(hPipe, messageBuffer, 2 * sizeof(message_t), replyBuffer, bufferSize * sizeof(response_t), &numReturnBytes, NULL)) {
		// Fail; log maybe?
	}
	// Copy now.
	memcpy(ptrBuffer, replyBuffer, numReturnBytes);
	// Free the memory
	delete[] replyBuffer;
	replyBuffer = 0;
	return true;
}

bool EtfArduinoService::StartAcquisition(UINT deviceId) {
	// Formirati poruku koju treba poslati serveru
	message_t  messageBuffer[MessageBufferSize];
	memset(messageBuffer, 0, sizeof messageBuffer);
	// Prvi je kod komande
	messageBuffer[0] = START_ACQUISITION;
	// Zatim slijede parametri:
	// Prvi parametar je DeviceId za koji treba zapoceti
	messageBuffer[1] = deviceId;
	// Poslati poruku serveru
	if (!SendMessage(messageBuffer, 2)) {
		return false;
	}
	// Ova funkcija ne ocekuje odgovor od kartice
	// Mozda da ocekuje neki ACK ipak?
	return true;
}


bool EtfArduinoService::StopAcquisition(UINT deviceId) {
	// Formirati poruku koju treba poslati serveru
	message_t  messageBuffer[MessageBufferSize];
	memset(messageBuffer, 0, sizeof messageBuffer);
	// Prvi je kod komande
	messageBuffer[0] = STOP_ACQUISITION;
	// Zatim slijede parametri:
	// Prvi parametar je DeviceId za koji treba zapoceti
	messageBuffer[1] = deviceId;
	// Poslati poruku serveru
	if (!SendMessage(messageBuffer, 2)) {
		return false;
	}
	// Ova funkcija ne ocekuje odgovor od kartice
	// Mozda da ocekuje neki ACK ipak?
	return true;
}

bool EtfArduinoService::SetAcquisitionBufferSize(UINT deviceId, DWORD bufferSize) {
	// Formirati poruku koju treba poslati serveru
	message_t  messageBuffer[MessageBufferSize];
	memset(messageBuffer, 0, sizeof messageBuffer);
	// Prvi je kod komande
	messageBuffer[0] = SET_ACQUISITION_BUFFER_SIZE;
	// Zatim slijede parametri:
	// Prvi parametar je DeviceId za koji treba odrediti bufferSize
	messageBuffer[1] = deviceId;
	// Drugi parametar je nova velicina buffera
	messageBuffer[2] = bufferSize;
	// Poslati poruku serveru
	if (!SendMessage(messageBuffer, 3)) {
		return false;
	}
	// Ova funkcija ne ocekuje odgovor od kartice
	// Mozda da ocekuje neki ACK ipak?
	return true;
}

bool EtfArduinoService::SetSampleRate(UINT deviceId, DWORD sampleRate) {
	// Formirati poruku koju treba poslati serveru
	message_t  messageBuffer[MessageBufferSize];
	memset(messageBuffer, 0, sizeof messageBuffer);
	// Prvi je kod komande
	messageBuffer[0] = SET_SAMPLE_RATE;
	// Zatim slijede parametri:
	// Prvi parametar je DeviceId
	messageBuffer[1] = deviceId;
	// Drugi parametar je novi sample rate
	messageBuffer[2] = sampleRate;
	// Poslati poruku serveru
	if (!SendMessage(messageBuffer, 3)) {
		return false;
	}
	// Ova funkcija ne ocekuje odgovor od kartice
	// Mozda da ocekuje neki ACK ipak?
	return true;
}

bool EtfArduinoService::SetInputChannelList(UINT deviceId, std::vector<int> const& channels) {
	// Formirati poruku koju treba poslati serveru
	message_t  messageBuffer[MessageBufferSize];
	memset(messageBuffer, 0, sizeof messageBuffer);
	// The command code is the first part of the message
	messageBuffer[0] = SET_INPUT_CHANNEL_LIST;
	// The first parameter is the DeviceId
	messageBuffer[1] = deviceId;
	// The second parameter is the number of channels
	size_t const sz = channels.size();
	messageBuffer[2] = sz;
	// The channel IDs follow:
	for (size_t i = 0; i < sz; ++i) {
		messageBuffer[3 + i] = channels[i];
	}
	// Send the message to the server
	if (!SendMessage(messageBuffer, 3 + sz)) {
		return false;
	}
	// This function does not expect a response from the device.
	return true;
}

unsigned short EtfArduinoService::GetSingleValue(UINT deviceId) {
	// Formirati poruku koju treba poslati serveru
	message_t  messageBuffer[MessageBufferSize];
	memset(messageBuffer, 0, sizeof messageBuffer);
	// Prvi je kod komande
	messageBuffer[0] = GET_SINGLE_VALUE;
	// Zatim slijede parametri:
	// Prvi parametar je DeviceId
	messageBuffer[1] = deviceId;
	// Odgovor je jedna vrijednost (2 bytea)
	unsigned short value = 0;
	DWORD numReturnBytes;
	if (hPipe == 0)
		if (!OpenPipeConnection())
			return false;
	if (!TransactNamedPipe(hPipe, messageBuffer, 2 * sizeof(message_t), &value, sizeof value, &numReturnBytes, NULL)) {
		// Fail
	}
	if (numReturnBytes != sizeof value) {
		return -1;
	}
	return value;
}

bool EtfArduinoService::CheckDeviceRegistered(UINT deviceId, LPTSTR serialPortName) {
	// Formirati poruku koju treba poslati serveru
	message_t  messageBuffer[MessageBufferSize];
	memset(messageBuffer, 0, sizeof messageBuffer);
	// Prvi je kod komande
	messageBuffer[0] = CHECK_DEVICE_REGISTERED;
	// Zatim slijede parametri:
	// Prvi parametar je DeviceId
	messageBuffer[1] = deviceId;
	// Prepare response buffer
	int response[15];
	memset(response, 0, sizeof response);
	DWORD numReturnBytes = 0;
	if (hPipe == 0)
		if (!OpenPipeConnection())
			return false;
	if (!TransactNamedPipe(hPipe,
						   messageBuffer,
						   2 * sizeof messageBuffer[0],
						   response,
						   sizeof response,
						   &numReturnBytes,
						   NULL)) {
		return false;
	}
	if (numReturnBytes != sizeof response) {
		return false;
	}
	if (response[0] == 1) {
		// The device exists, get its serial port name.
		size_t const maxLength = 9;
		memset(serialPortName, 0, 10 * sizeof serialPortName[0]);
		// It is necessary to read it "manually" because the data type
		// of the buffer is larger than the character data type.
		size_t const offset = 1;
		for (size_t i = 0; i < maxLength; ++i) {
			serialPortName[i] = response[i + offset];
			if (serialPortName[i] == 0)
				break;
		}
	}
	return response[0] == 1;
}

bool EtfArduinoService::RegisterDevice(UINT& deviceId, LPCWSTR serialPortName) {
	// Formirati poruku koju treba poslati serveru
	message_t  messageBuffer[MessageBufferSize];
	memset(messageBuffer, 0, sizeof messageBuffer);
	// Command code
	messageBuffer[0] = REGISTER_DEVICE;
	// First parameter: name of the serial port
	size_t maxPortNameLength = 9;
	size_t portNameLength = _tcslen(serialPortName);
	if (portNameLength > maxPortNameLength) {
		return false;
	}
	std::copy(serialPortName, 
			  serialPortName + portNameLength + 1 + 1,	// +1 for \0, +1 because it needs to point after the last element
			  &messageBuffer[1]);
	// The response is the id of the device!
	// We write it directly to the out parameter deviceId
	DWORD numReturnBytes;
	if (hPipe == 0)
		if (!OpenPipeConnection()) {
			return false;
		}
	if (!TransactNamedPipe(hPipe,
						   messageBuffer,
						   (1 + portNameLength + 1) * sizeof messageBuffer[0],
						   &deviceId,
						   sizeof deviceId,
						   &numReturnBytes,
						   NULL)) {
		// Failed to get the response
		return false;
	}
	if (numReturnBytes != sizeof deviceId) {
		return false;
	}
	return true;
}

bool EtfArduinoService::GetRegisteredDeviceIds(std::vector<UINT>& deviceIds) {
	// Form the message to be sent to the server
	message_t  messageBuffer[MessageBufferSize];
	memset(messageBuffer, 0, sizeof messageBuffer);
	// Command code
	messageBuffer[0] = GET_REGISTERED_DEVICE_IDS;
	// No parameters for this command
	// Prepare response buffer
	int response[15];
	memset(response, 0, sizeof response);
	DWORD numReturnBytes = 0;
	if (hPipe == 0)
		if (!OpenPipeConnection())
			return false;
	if (!TransactNamedPipe(hPipe,
						   messageBuffer,
						   1 * sizeof messageBuffer[0],
						   response,
						   sizeof response,
						   &numReturnBytes,
						   NULL)) {
		return false;
	}
	if (numReturnBytes != sizeof response) {
		return false;
	}
	int total = response[0];
	if (total > 10) total = 10;
	for (int i = 0; i < total; ++i) {
		deviceIds.push_back(response[1 + i]);
	}
	return true;
}

bool EtfArduinoService::SendDigitalValue(UINT deviceId, int line, int value) {
	message_t  messageBuffer[MessageBufferSize];
	memset(messageBuffer, 0, sizeof messageBuffer);
	// Command code
	messageBuffer[0] = SEND_DIGITAL_VALUE;
	// Parameters
	messageBuffer[1] = deviceId;
	messageBuffer[2] = line;
	messageBuffer[3] = value;
	// Poslati poruku serveru
	if (!SendMessage(messageBuffer, 4)) {
		return false;
	}
	return true;
}

bool EtfArduinoService::PutSingleValue(UINT deviceId, int channel, int value) {
	message_t  messageBuffer[MessageBufferSize];
	memset(messageBuffer, 0, sizeof messageBuffer);
	// Command code
	messageBuffer[0] = PUT_SINGLE_VALUE;
	// Parameters
	messageBuffer[1] = deviceId;
	messageBuffer[2] = channel;
	messageBuffer[3] = value;
	// Poslati poruku serveru
	if (!SendMessage(messageBuffer, 4)) {
		return false;
	}
	return true;
}



bool EtfArduinoService::OpenPipeConnection() {
	for (;;)
	{ 
		hPipe = CreateFile( 
			PIPENAME,		// pipe name 
			GENERIC_READ |  // read and write access 
				GENERIC_WRITE, 
			0,              // no sharing 
			NULL,           // default security attributes
			OPEN_EXISTING,  // opens existing pipe 
			0,              // default attributes 
			NULL);          // no template file 
 
		// Break if the pipe handle is valid. 
 
		if (hPipe != INVALID_HANDLE_VALUE)
			break; 
 
		// Exit if an error other than ERROR_PIPE_BUSY occurs. 
 
		if (GetLastError() != ERROR_PIPE_BUSY) {
			hPipe = 0;
			return false;
		}
 
		// All pipe instances are busy, so wait for 20 seconds. 
	 
		if (!WaitNamedPipe(PIPENAME, 20000)) { 
			printf("Could not open pipe: 20 second wait timed out."); 
			hPipe = 0;
			return false;
		} 
	} 
 
	// The pipe connected; change the readmode
	DWORD dwMode = PIPE_READMODE_MESSAGE; 
	BOOL fSuccess = SetNamedPipeHandleState(
		hPipe,	// pipe handle 
		&dwMode,	// new pipe mode 
		NULL,		// don't set maximum bytes 
		NULL);		// don't set maximum time 
	if (!fSuccess) {
		CloseHandle(hPipe);
		hPipe = 0;
		return false;
	}
	return true;
}


bool EtfArduinoService::SendMessage(LPVOID lpvMessage, DWORD bufferSize) {
	if (hPipe == 0)
		if (!OpenPipeConnection())
			return false;
	// Izracunati velicinu u byteima...
	// Sam bufferSize je predivdjen da bude velicina niza koji je proslijedjen,
	// tako da messageBuffer ce biti promijenjen iz void* u pointer odgovarajuceg tipa
	// cim specificiram struktura koja ce ustvari biti koristena za slanje
	DWORD cbToWrite = bufferSize * sizeof(message_t);
	DWORD cbWritten = 0;
	BOOL fSuccess = WriteFile( 
		hPipe,               // pipe handle 
		lpvMessage,             // message 
		cbToWrite,              // message length 
		&cbWritten,             // bytes written 
		NULL);                  // not overlapped 
	// Make sure the message got through the pipe before returning.
	FlushFileBuffers(hPipe);
	if (!fSuccess) {
		_tprintf(TEXT("WriteFile to pipe failed. GLE=%d\n"), GetLastError()); 
		return false;
	}
	if (cbWritten != cbToWrite) {
		// Nije sve zapisano!
		return false;
	}
	return true;
}



BOOL EtfArduinoService::GetReplyMessage(LPHANDLE lphPipe, LPVOID lpvMessage, DWORD byteBufferSize, LPDWORD returnBytes) {
	if (hPipe == 0)
		if (!OpenPipeConnection())
			return false;
	BOOL fSuccess = FALSE;
	DWORD cbRead = 0;
	do {
		// Read from the pipe. 
		fSuccess = ReadFile( 
			*lphPipe,									// pipe handle 
			lpvMessage,									// buffer to receive reply 
			byteBufferSize,								// size of buffer 
			&cbRead,									// number of bytes read 
			NULL);										// not overlapped 
		if (!fSuccess && GetLastError() != ERROR_MORE_DATA) {
			break;
		}
		if (cbRead == 0) continue;
	} while (!fSuccess);  // repeat loop if ERROR_MORE_DATA 

	if (!fSuccess) {
		_tprintf( TEXT("ReadFile from pipe failed. GLE=%d\n"), GetLastError() );
		return FALSE;
	}
	*returnBytes = cbRead;
	return TRUE;
}

bool EtfArduinoService::ClosePipeConnection() {
	// First send a message to the server notifying it that
	// the client is disconnecting.
	message_t  messageBuffer[MessageBufferSize];
	memset(messageBuffer, 0, sizeof messageBuffer);
	// Prvi je kod komande
	messageBuffer[0] = DISCONNECT_CLIENT;
	// Poslati poruku serveru
	SendMessage(messageBuffer, 1);
	if (!CloseHandle(hPipe)) {
		return false;
	}
	hPipe = 0;
	return true;
}