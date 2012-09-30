// RequestHandler.h
// (c) 2012 Marko Lalic

// EtfArduino may be freely distributed under the MIT license.
// For all details and documentation:
// http://github.com/mlalic/etfarduino/

#ifndef __REQUESTHANDLER_H__
#define __REQUESTHANDLER_H__
#include "ArduinoDevice.h"
#include "PipeCommunicator.h"
// ABC which defines the interface all RequestHandlers must adhere to.
class RequestHandler
{
public:
	RequestHandler(std::tr1::shared_ptr<PipeCommunicator> pipe);
	virtual ~RequestHandler();
	enum Status { OK, FAIL, EXIT };
	// Pure virtual function
	virtual Status run() = 0;
	
protected:
	std::tr1::shared_ptr<PipeCommunicator> pipe;
};

#endif