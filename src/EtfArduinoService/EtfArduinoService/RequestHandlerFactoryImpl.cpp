// RequestHandlerFactoryImpl.cpp
// (c) 2012 Marko Lalic

// EtfArduino may be freely distributed under the MIT license.
// For all details and documentation:
// http://github.com/mlalic/etfarduino/

#include "stdafx.h"
#include "RequestHandlerFactoryImpl.h"
#include "RequestHandlers.h"
#include <tchar.h>
RequestHandlerFactoryImpl::RequestHandlerFactoryImpl(DeviceMap& devices, std::tr1::shared_ptr<PipeCommunicator> pipe) :
  RequestHandlerFactory(devices), pipe(pipe) {
	  // Empty
}

RequestHandlerFactoryImpl::~RequestHandlerFactoryImpl() {
	// Empty
}

// :TODO: Use std::vectors instead of a plain pointer to a chunk of memory.
std::auto_ptr<RequestHandler> RequestHandlerFactoryImpl::GetRequestHandler(
	RequestHandlerFactory::request_t const* const buffer) {
		RequestHandler* requestHandler = 0;
		// The command code is always the first in the buffer
		request_t const commandCode = buffer[0];
		switch (commandCode) {
		case GET_BUFFERED_DATA: {
			ArduinoDevice* device = devices.Find(buffer[1]);
			if (device == 0)
				break;
			requestHandler = new GetBufferedDataRequestHandler(*device, pipe);
			break;
		}
		case START_ACQUISITION: {
			ArduinoDevice* device = devices.Find(buffer[1]);
			if (device == 0)
				break;
			requestHandler = new StartAcquisitionRequestHandler(*device, pipe);
			break;
		}
		case STOP_ACQUISITION: {
			ArduinoDevice* device = devices.Find(buffer[1]);
			if (device == 0)
				break;
			requestHandler = new StopAcquisitionRequestHandler(*device, pipe);
			break;
		}
		case GET_SINGLE_VALUE: {
			ArduinoDevice* device = devices.Find(buffer[1]);
			if (device == 0)
				break;
			requestHandler = new GetSingleValueRequestHandler(*device, pipe, buffer[2]);
			break;
		}
		case CHECK_DEVICE_REGISTERED: {
			printf("Check device registered handler creation\n");
			ArduinoDevice* device = devices.Find(buffer[1]);
			printf("Found the device, handler created\n");
			requestHandler = new CheckDeviceRegisteredRequestHandler(device, pipe);
			break;
		}
		case REGISTER_DEVICE: {
			// The parameter is a string which represents the 
			// name of the serial port on which this device is running.
			size_t const maxLength = 9;
			TCHAR serialPortName[maxLength + 1];		// +1 for the null terminator
			memset(serialPortName, 0, sizeof serialPortName);
			// It is necessary to read it "manually" because the data type
			// of the buffer is larger than the character data type.
			size_t const offset = 1;	// because it's the first parameter
			for (size_t i = 0; i < maxLength; ++i) {
				serialPortName[i] = buffer[i + offset];
				if (serialPortName[i] == 0)
					break;
			}
			_tprintf(_T("Decoded port name %s\n"), serialPortName);
			requestHandler = new RegisterDeviceRequestHandler(
														pipe,
														devices,
														serialPortName);
			break;
		}
		case GET_REGISTERED_DEVICE_IDS: {
			requestHandler = new GetRegisteredDevicesRequestHandler(pipe, devices);
			break;
		}
		case SET_ACQUISITION_BUFFER_SIZE: {
			ArduinoDevice* device = devices.Find(buffer[1]);
			if (device == 0)
				break;
			requestHandler = new SetAcquisitionBufferSizeRequestHandler(*device, pipe, buffer[2]);
			break;
		}
		case SET_SAMPLE_RATE: {
			ArduinoDevice* device = devices.Find(buffer[1]);
			if (device == 0)
				break;
			requestHandler = new SetSampleRateRequestHandler(*device, pipe, buffer[2]);
			break;
		}
		case SET_INPUT_CHANNEL_LIST: {
			ArduinoDevice* device = devices.Find(buffer[1]);
			if (device == 0)
				break;
			// Form a channels vector
			std::vector<int> channels(buffer[2]);
			for (size_t i = 0; i < channels.size(); ++i) {
				channels[i] = buffer[3 + i];
			}
			requestHandler = new SetInputChannelsRequestHandler(*device, pipe, channels);
			break;
		}
		case SEND_DIGITAL_VALUE: {
			ArduinoDevice* device = devices.Find(buffer[1]);
			if (device == 0)
				break;
			requestHandler = new SendDigitalValueRequestHandler(
										*device,
										pipe,
										buffer[2],
										buffer[3]);
			break;
		}
		case PUT_SINGLE_VALUE: {
			ArduinoDevice* device = devices.Find(buffer[1]);
			if (device == 0)
				break;
			requestHandler = new PutSingleValueRequestHandler(
										*device,
										pipe,
										buffer[2],
										buffer[3]);
			break;
		}
		case DISCONNECT_CLIENT: {
			requestHandler = new DisconnectRequestHandler(pipe);
			break;
		}
		default:
			;
		}
		return std::auto_ptr<RequestHandler>(requestHandler);
}