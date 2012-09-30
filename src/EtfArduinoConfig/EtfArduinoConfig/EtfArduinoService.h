// EtfArduinoService.h
// (c) 2012 Marko Lalic

// EtfArduino may be freely distributed under the MIT license.
// For all details and documentation:
// http://github.com/mlalic/etfarduino/

#define __ETFARDUINOSERVICE_H__
#include <stdio.h>		// Mostly for debugging purposes
#include <tchar.h>
#include <windows.h>
#include <vector>

class EtfArduinoService {
public:
	// Constructors
	EtfArduinoService();
	// Destructor
	virtual ~EtfArduinoService();
	// Public member functions
	bool setResponseBufferSize(DWORD responseBufferSize);
	bool GetBufferedData(
		UINT	deviceId,
		LPVOID	ptrBuffer,
		DWORD	bufferSize);
	bool	StartAcquisition(UINT deviceId);
	bool	StopAcquisition(UINT deviceId);
	bool	SetAcquisitionBufferSize(UINT deviceId, DWORD bufferSize);
	bool	SetSampleRate(UINT deviceId, DWORD sampleRate);
	bool	CheckDeviceRegistered(UINT deviceId, LPTSTR serialPortName);
	bool	RegisterDevice(UINT& deviceId, LPCWSTR serialPortName);
	bool	GetRegisteredDeviceIds(std::vector<UINT>& deviceIds);
	unsigned short	GetSingleValue(UINT deviceId);
	bool	SendDigitalValue(UINT deviceId, int line, int value);
	bool	PutSingleValue(UINT deviceId, int channel, int value);
protected:
private:
	// typedefs
	typedef int	message_t;
	typedef unsigned short	response_t;
	// Private members
	HANDLE hPipe;
	// Private helper member functions
	bool OpenPipeConnection();
	bool SendMessage(LPVOID lpvMessage, DWORD bufferSize);
	BOOL GetReplyMessage(
		LPHANDLE	lphPipe,
		LPVOID		lpvMessage,
		DWORD		byteBufferSize,
		LPDWORD		returnBytes);
	bool ClosePipeConnection();
	// Private static members
	static LPTSTR	const	PIPENAME;
	static DWORD	const	MessageBufferSize;
	// Command codes for the EtfArduinoServiceServer
	static message_t const	GET_BUFFERED_DATA = 1;
	static message_t const	START_ACQUISITION = 2;
	static message_t const	STOP_ACQUISITION  = 3;
	static message_t const	GET_SINGLE_VALUE  = 4;
	static message_t const	CHECK_DEVICE_REGISTERED = 5;
	static message_t const	REGISTER_DEVICE = 6;
	static message_t const	GET_REGISTERED_DEVICE_IDS = 7;

	static message_t const	SET_ACQUISITION_BUFFER_SIZE = 10;
	static message_t const	SET_SAMPLE_RATE = 11;

	static message_t const	SEND_DIGITAL_VALUE = 20;

	static message_t const  PUT_SINGLE_VALUE = 30;

	static message_t const	DISCONNECT_CLIENT = 255;
};

#endif