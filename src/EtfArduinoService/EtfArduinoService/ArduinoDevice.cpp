// ArduinoDevice.cpp
// (c) 2012 Marko Lalic

// EtfArduino may be freely distributed under the MIT license.
// For all details and documentation:
// http://github.com/mlalic/etfarduino/

#include "stdafx.h"
#include "ArduinoDevice.h"

ArduinoDevice::ArduinoDevice(UINT deviceId) : deviceId(deviceId), bufferSize(DEFAULT_BUFFER_SIZE) {
	// Empty
}

ArduinoDevice::ArduinoDevice(UINT deviceId, DWORD bufferSize) : deviceId(deviceId), bufferSize(bufferSize) {
	// Empty
}

ArduinoDevice::~ArduinoDevice() {
	// Empty
}

DWORD ArduinoDevice::GetBufferSize() const {
	return bufferSize;
}

UINT ArduinoDevice::GetDeviceId() const {
	return deviceId;
}
