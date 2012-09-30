// RequestHandlerFactory.cpp
// (c) 2012 Marko Lalic

// EtfArduino may be freely distributed under the MIT license.
// For all details and documentation:
// http://github.com/mlalic/etfarduino/

#include "stdafx.h"
#include "RequestHandlerFactory.h"

RequestHandlerFactory::RequestHandlerFactory(DeviceMap& devices) : 
  devices(devices) {
	// Empty
}

RequestHandlerFactory::~RequestHandlerFactory() {
	// Empty
}
