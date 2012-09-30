// RequestHandler.cpp
// (c) 2012 Marko Lalic

// EtfArduino may be freely distributed under the MIT license.
// For all details and documentation:
// http://github.com/mlalic/etfarduino/

#include "stdafx.h"
#include "RequestHandler.h"

RequestHandler::RequestHandler(std::tr1::shared_ptr<PipeCommunicator> pipe) :
	pipe(pipe) {
		// Empty
}

RequestHandler::~RequestHandler() {
	// Nothing to do here.
}
