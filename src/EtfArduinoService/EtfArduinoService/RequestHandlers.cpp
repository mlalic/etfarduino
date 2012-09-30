// RequestHandlers.cpp
// (c) 2012 Marko Lalic

// EtfArduino may be freely distributed under the MIT license.
// For all details and documentation:
// http://github.com/mlalic/etfarduino/

#include "stdafx.h"
#include "RequestHandlers.h"
#include <tchar.h>
// -------------------------
// Get Buffered Data Handler
// -------------------------
GetBufferedDataRequestHandler::GetBufferedDataRequestHandler(ArduinoDevice &device,
															 std::tr1::shared_ptr<PipeCommunicator> pipe) :
	RequestHandler(pipe), device(device) {
		// Empty
}

RequestHandler::Status GetBufferedDataRequestHandler::run() {
	DWORD bufferSize = device.GetBufferSize();
	// Get the buffer from the device (thereby taking ownership of it)
	ArduinoDevice::response_t* reply = device.GetNextBuffer();
	// Write the reply to the pipe.
	bool success = 
		pipe->SendMessage(reply, bufferSize * sizeof(ArduinoDevice::response_t));
	delete[] reply;
	reply = 0;
	if (success)
		return Status::OK;
	else
		return Status::FAIL;
}
// -------------------------
// Start Acquisition Handler
// -------------------------
StartAcquisitionRequestHandler::StartAcquisitionRequestHandler(ArduinoDevice& device,
															   std::tr1::shared_ptr<PipeCommunicator> pipe) :
  RequestHandler(pipe), device(device) {
	// Empty
}

RequestHandler::Status StartAcquisitionRequestHandler::run() {
	HANDLE hThread = CreateThread( 
		NULL,              // no security attribute 
		0,                 // default stack size 
		StartAcquisitionRequestHandler::AnalogAcquisitionProc,    // thread proc
		(LPVOID) &device,    // thread parameter 
		0,                 // not suspended 
		NULL);      // no thread ID 
	if (hThread == NULL) {
		// Thread creation failed -- no acquisition will happen.
		// Log the problem.
		printf("Analog Acquisition Thread creation failed.\n");
		return Status::FAIL;
	} else {
		// We don't need to keep track of the thread anymore.
		CloseHandle(hThread);
	}
	return Status::OK;
}

DWORD WINAPI StartAcquisitionRequestHandler::AnalogAcquisitionProc(LPVOID lpvParam) {
	ArduinoDevice& device = *(ArduinoDevice*)lpvParam;
	device.StartAcquisition();
	printf("Stopping analog acquisition thread...\n");
	return 0;
}
// -------------------------
// Stop Acquisition Handler
// -------------------------
StopAcquisitionRequestHandler::StopAcquisitionRequestHandler(ArduinoDevice& device,
															 std::tr1::shared_ptr<PipeCommunicator> pipe) :
  RequestHandler(pipe), device(device) {
	  // Empty
}

RequestHandler::Status StopAcquisitionRequestHandler::run() {
	if (device.StopAcquisition())
		return Status::OK;
	else
		return Status::FAIL;
}
// -------------------------
// Get Single Value Handler
// -------------------------
GetSingleValueRequestHandler::GetSingleValueRequestHandler(ArduinoDevice& device,
														   std::tr1::shared_ptr<PipeCommunicator> pipe) :
  RequestHandler(pipe), device(device) {
	  // Empty
}

RequestHandler::Status GetSingleValueRequestHandler::run() {
	// Elicit the response from the device
	ArduinoDevice::response_t value = device.GetSingleValue();
	// Write the reply to the pipe.
	if (pipe->SendMessage(&value, sizeof(ArduinoDevice::response_t)))
		return Status::OK;
	else
		return Status::FAIL;
}

// -------------------------------
// Check Device Registered Handler
// -------------------------------
CheckDeviceRegisteredRequestHandler::CheckDeviceRegisteredRequestHandler(ArduinoDevice* device,
																		 std::tr1::shared_ptr<PipeCommunicator> pipe) :
  RequestHandler(pipe), device(device) {
	  // Empty
}

RequestHandler::Status CheckDeviceRegisteredRequestHandler::run() {
	printf("Check device registered handler\n");
	int replyBuffer[15];
	memset(replyBuffer, 0, sizeof replyBuffer);
	if (device != 0) {
		// It is registered!
		replyBuffer[0] = 1;
		// The reply sends the serial port name if the device exists
		LPCTSTR serialPortName = device->GetPortName();
		size_t portNameLength = _tcsclen(serialPortName);
		if (portNameLength > 9) {
			portNameLength = 9;
		}
		std::copy(serialPortName, 
				  serialPortName + portNameLength + 1 + 1,	// +1 for \0, +1 because it needs to point after the last element
				  &replyBuffer[1]);
	}
	// Send the reply
	printf("Sending reply...");
	if (pipe->SendMessage(replyBuffer, sizeof replyBuffer))
		return Status::OK;
	else
		return Status::FAIL;
}
// -----------------------
// Register Device Handler
// -----------------------
RegisterDeviceRequestHandler::RegisterDeviceRequestHandler(std::tr1::shared_ptr<PipeCommunicator> pipe,
														   DeviceMap& deviceMap,
														   LPCTSTR serialPortName) :
  RequestHandler(pipe), deviceMap(deviceMap) {
	  printf("Ctor...");
	  memset(RegisterDeviceRequestHandler::serialPortName,
		     0,
			 sizeof RegisterDeviceRequestHandler::serialPortName);
	  for (size_t i = 0; i < _tcsclen(serialPortName) && i < 10; ++i) {
		  RegisterDeviceRequestHandler::serialPortName[i] = serialPortName[i];
	  }
	  printf("Done.\n");
	  // Empty
}
RequestHandler::Status RegisterDeviceRequestHandler::run() {
	printf("Going for insert...\n");
	UINT deviceId = deviceMap.Insert(serialPortName);
	printf("Inserted...\n");
	_tprintf(_T("Registering device ... %d @ %s\n"), deviceId, serialPortName);
	// Write the result to the pipe
	if (pipe->SendMessage(&deviceId, sizeof deviceId))
		return Status::OK;
	else
		return Status::FAIL;
}
// ------------------------------
// Get Registered Devices Handler
// ------------------------------
GetRegisteredDevicesRequestHandler::GetRegisteredDevicesRequestHandler(std::tr1::shared_ptr<PipeCommunicator> pipe,
																	   DeviceMap& deviceMap) : 
  RequestHandler(pipe), deviceMap(deviceMap) {
	  // Empty
}
RequestHandler::Status GetRegisteredDevicesRequestHandler::run() {
	std::vector<UINT> registeredDevices;
	deviceMap.GetAllRegisteredDevices(registeredDevices);
	// Put the result back to the pipe
	int buffer[15];
	memset(buffer, 0, sizeof buffer);
	int len = registeredDevices.size();
	if (len > 10) len = 10;
	buffer[0] = len;
	std::copy(registeredDevices.begin(), registeredDevices.begin() + len, &buffer[1]);
	if (pipe->SendMessage(buffer,
						  sizeof buffer)) {
		return Status::OK;
	} else {
		return Status::FAIL;
	}
}
// -----------------------------------
// Set Acquisition Buffer Size Handler
// -----------------------------------
SetAcquisitionBufferSizeRequestHandler::SetAcquisitionBufferSizeRequestHandler(ArduinoDevice& device,
																			   std::tr1::shared_ptr<PipeCommunicator> pipe,
																			   int bufferSize) :
  RequestHandler(pipe), device(device), bufferSize(bufferSize) {
	  // Empty
}
RequestHandler::Status SetAcquisitionBufferSizeRequestHandler::run() {
	printf("Setting buffer size\n");
	if (device.SetBufferSize(bufferSize))
		return Status::OK;
	else
		return Status::FAIL;
}

// -----------------------
// Set Sample Rate Handler
// -----------------------
SetSampleRateRequestHandler::SetSampleRateRequestHandler(ArduinoDevice& device,
														 std::tr1::shared_ptr<PipeCommunicator> pipe,
														 int sampleRate) :
  RequestHandler(pipe), device(device), sampleRate(sampleRate) {
	  // Empty
}
RequestHandler::Status SetSampleRateRequestHandler::run() {
	printf("Setting sample rate\n");
	if (device.SetSampleRate(sampleRate))
		return Status::OK;
	else
		return Status::FAIL;
}
// --------------------------
// Send Digital Value Handler
// --------------------------
SendDigitalValueRequestHandler::SendDigitalValueRequestHandler(
	ArduinoDevice& device,
	std::tr1::shared_ptr<PipeCommunicator> pipe,
	int line,
	int value) : RequestHandler(pipe), device(device), line(line), value(value) {
		// Empty
}
RequestHandler::Status SendDigitalValueRequestHandler::run() {
	printf("Send digital value .. %d\n", value);
	if (device.SendDigitalValue(line, value))
		return Status::OK;
	else
		return Status::FAIL;
}

// ------------------------
// Put Single Value Handler
// ------------------------
PutSingleValueRequestHandler::PutSingleValueRequestHandler(
	ArduinoDevice& device,
	std::tr1::shared_ptr<PipeCommunicator> pipe,
	int channel,
	int value) : RequestHandler(pipe), device(device), channel(channel), value(value) {
		// Empty
}
RequestHandler::Status PutSingleValueRequestHandler::run() {
	printf("Put Single Value\n");
	if (device.PutSingleValue(channel, value))
		return Status::OK;
	else
		return Status::FAIL;
}

// ------------------
// Disconnect Handler
// ------------------
DisconnectRequestHandler::DisconnectRequestHandler(std::tr1::shared_ptr<PipeCommunicator> pipe) :
  RequestHandler(pipe) {
	  // Empty
}
RequestHandler::Status DisconnectRequestHandler::run() {
	// Code signaling the caller to terminate its execution!
	return Status::EXIT;
}
