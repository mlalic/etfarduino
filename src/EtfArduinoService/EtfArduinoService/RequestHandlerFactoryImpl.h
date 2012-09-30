// RequestHandlerFactoryImp.h
// (c) 2012 Marko Lalic

// EtfArduino may be freely distributed under the MIT license.
// For all details and documentation:
// http://github.com/mlalic/etfarduino/

#ifndef __REQUESTHANDLERFACTORYIMPL_H__
#define __REQUESTHANDLERFACTORYIMPL_H__
#include "RequestHandlerFactory.h"
#include "PipeCommunicator.h"

// A concrete factory class which produces Handlers which send their replies over
// named pipes.
class RequestHandlerFactoryImpl :
	public RequestHandlerFactory
{
public:
	RequestHandlerFactoryImpl(DeviceMap& devices, std::tr1::shared_ptr<PipeCommunicator> pipe);
	~RequestHandlerFactoryImpl();
	std::auto_ptr<RequestHandler> GetRequestHandler(request_t const* const buffer);
private:
	std::tr1::shared_ptr<PipeCommunicator> pipe;
};

#endif