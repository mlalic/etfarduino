// RequestHandlers.h
// (c) 2012 Marko Lalic

// EtfArduino may be freely distributed under the MIT license.
// For all details and documentation:
// http://github.com/mlalic/etfarduino/

#ifndef __REQUESTHANDLERS_H__
#define __REQUESTHANDLERS_H__

#include "RequestHandler.h"
#include "ArduinoDevice.h"
#include "DeviceMap.h"
// -------------------------
// Get Buffered Data Handler
// -------------------------
class GetBufferedDataRequestHandler : 
	public RequestHandler {
public:
	GetBufferedDataRequestHandler(ArduinoDevice& device, std::tr1::shared_ptr<PipeCommunicator> pipe);
	Status run();
private:
	ArduinoDevice& device;
};
// -------------------------
// Start Acquisition Handler
// -------------------------
class StartAcquisitionRequestHandler :
	public RequestHandler {
public:
	StartAcquisitionRequestHandler(ArduinoDevice& device, std::tr1::shared_ptr<PipeCommunicator> pipe);
	Status run();
private:
	ArduinoDevice& device;
	static DWORD WINAPI AnalogAcquisitionProc(LPVOID lpvParam);

};
// -------------------------
// Stop Acquisition Handler
// -------------------------
class StopAcquisitionRequestHandler : 
	public RequestHandler {
public:
	StopAcquisitionRequestHandler(ArduinoDevice& device, std::tr1::shared_ptr<PipeCommunicator> pipe);
	Status run();
private:
	ArduinoDevice& device;
};
// -------------------------
// Get Single Value Handler
// -------------------------
class GetSingleValueRequestHandler :
	public RequestHandler {
public:
	GetSingleValueRequestHandler(ArduinoDevice& device, std::tr1::shared_ptr<PipeCommunicator> pipe, int channelId);
	Status run();
private:
	ArduinoDevice& device;
	int const channelId;
};
// -------------------------------
// Check Device Registered Handler
// -------------------------------
class CheckDeviceRegisteredRequestHandler :
	public RequestHandler {
public:
	CheckDeviceRegisteredRequestHandler(ArduinoDevice* device,
										std::tr1::shared_ptr<PipeCommunicator> pipe);
	Status run();
private:
	ArduinoDevice* device;
};
// -----------------------
// Register Device Handler
// -----------------------
class RegisterDeviceRequestHandler :
	public RequestHandler {
public:
	RegisterDeviceRequestHandler(std::tr1::shared_ptr<PipeCommunicator> pipe,
								 DeviceMap& deviceMap,
								 LPCTSTR serialPortName);
	Status run();
private:
	DeviceMap& deviceMap;
	TCHAR serialPortName[10];
};
// ------------------------------
// Get Registered Devices Handler
// ------------------------------
class GetRegisteredDevicesRequestHandler :
	public RequestHandler {
public:
	GetRegisteredDevicesRequestHandler(std::tr1::shared_ptr<PipeCommunicator> pipe,
									   DeviceMap& deviceMap);
	Status run();
private:
	DeviceMap& deviceMap;
};
// -----------------------------------
// Set Acquisition Buffer Size Handler
// -----------------------------------
class SetAcquisitionBufferSizeRequestHandler :
	public RequestHandler {
public:
	SetAcquisitionBufferSizeRequestHandler(
		ArduinoDevice& device,
		std::tr1::shared_ptr<PipeCommunicator> pipe,
		int bufferSize);
	Status run();
private:
	ArduinoDevice& device;
	int const bufferSize;
};
// -----------------------
// Set Sample Rate Handler
// -----------------------
class SetSampleRateRequestHandler :
	public RequestHandler {
public:
	SetSampleRateRequestHandler(
		ArduinoDevice& device,
		std::tr1::shared_ptr<PipeCommunicator> pipe,
		int sampleRate);
	Status run();
private:
	ArduinoDevice& device;
	int const sampleRate;
};
// --------------------------
// Set Input Channels Handler
// --------------------------
class SetInputChannelsRequestHandler :
	public RequestHandler {
public:
	SetInputChannelsRequestHandler(
		ArduinoDevice& device,
		std::tr1::shared_ptr<PipeCommunicator> pipe,
		std::vector<int> const& channels);
	Status run();
private:
	ArduinoDevice& device;
	std::vector<int> channels;
};
// --------------------------
// Send Digital Value Handler
// --------------------------
class SendDigitalValueRequestHandler :
	public RequestHandler {
public:
	SendDigitalValueRequestHandler(
		ArduinoDevice& device,
		std::tr1::shared_ptr<PipeCommunicator> pipe,
		int line,
		int value);
	Status run();
private:
	ArduinoDevice& device;
	int const line;
	int const value;
		
};
// ------------------------
// Put Single Value Handler
// ------------------------
class PutSingleValueRequestHandler :
	public RequestHandler {
public:
	PutSingleValueRequestHandler(
		ArduinoDevice& device,
		std::tr1::shared_ptr<PipeCommunicator> pipe,
		int channel,
		int value);
	Status run();
private:
	ArduinoDevice& device;
	int const channel;
	int const value;
};

// ------------------
// Disconnect Handler
// ------------------
class DisconnectRequestHandler :
	public RequestHandler {
public:
	DisconnectRequestHandler(
		std::tr1::shared_ptr<PipeCommunicator> pipe);
	Status run();
};

#endif