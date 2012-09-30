// RequestHandlerFactory.h
// (c) 2012 Marko Lalic

// EtfArduino may be freely distributed under the MIT license.
// For all details and documentation:
// http://github.com/mlalic/etfarduino/

#ifndef __REQUESTHANDLERFACTORY_H__
#define __REQUESTHANDLERFACTORY_H__
#include "RequestHandler.h"
#include "DeviceMap.h"
// ABC
class RequestHandlerFactory {
public:
	typedef int request_t;
	RequestHandlerFactory(DeviceMap& devices);
	virtual ~RequestHandlerFactory();
	// Pure virtual member function
	virtual std::auto_ptr<RequestHandler> GetRequestHandler(request_t const* const buffer) = 0;
protected:
	// Request codes. All concrete Factories will use them!
	static request_t const GET_BUFFERED_DATA = 1;
	static request_t const START_ACQUISITION = 2;
	static request_t const STOP_ACQUISITION = 3;
	static request_t const GET_SINGLE_VALUE = 4;
	static request_t const CHECK_DEVICE_REGISTERED = 5;
	static request_t const REGISTER_DEVICE = 6;
	static request_t const GET_REGISTERED_DEVICE_IDS = 7;


	static request_t const SET_ACQUISITION_BUFFER_SIZE	= 10;
	static request_t const SET_SAMPLE_RATE = 11;

	static request_t const SEND_DIGITAL_VALUE = 20;

	static request_t const PUT_SINGLE_VALUE = 30;

	static request_t const DISCONNECT_CLIENT = 255;
	DeviceMap& devices;
private:
};

#endif