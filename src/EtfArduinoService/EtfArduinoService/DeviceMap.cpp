// DeviceMap.cpp
// (c) 2012 Marko Lalic

// EtfArduino may be freely distributed under the MIT license.
// For all details and documentation:
// http://github.com/mlalic/etfarduino/

#include "stdafx.h"
#include "DeviceMap.h"
#include "Lock.h"
#include "ArduinoDeviceSerial.h"
DeviceMap::DeviceMap() : nextId(0) {
	// Empty
}

DeviceMap::~DeviceMap() {
	Lock lock(cs);
	// Need to dispose of all ArduinoDevice that this map owns.
	for (std::map<UINT, ArduinoDevice*>::iterator it = devices.begin();
			it != devices.end();
			++it) {
				delete it->second;
	}
}

UINT DeviceMap::Insert(LPCWSTR serialPortName) {
	Lock lock(cs);

	std::pair<std::map<UINT, ArduinoDevice*>::iterator, bool> ret = 
		devices.insert(std::pair<UINT, ArduinoDevice*>(nextId, new ArduinoDeviceSerial(nextId, serialPortName)));

	++nextId;
	return nextId - 1;
}

ArduinoDevice* DeviceMap::Find(UINT deviceId) {
	// Whole function is a critical section since the dereferencing of the iterator
	// must be protected from happening after a (concurrently executed) Remove of the element
	Lock lock(cs);
	std::map<UINT, ArduinoDevice*>::iterator it = devices.find(deviceId);
	if (it == devices.end()) {
		// Not found
		return 0;
	}
	return it->second;
}

void DeviceMap::Remove(UINT deviceId) {
	Lock lock(cs);
	std::map<UINT, ArduinoDevice*>::const_iterator it = devices.find(deviceId);
	if (it != devices.end())
		delete it->second;
	devices.erase(it);
}

void DeviceMap::GetAllRegisteredDevices(std::vector<UINT>& deviceIds) {
	Lock lock(cs);
	for (std::map<UINT, ArduinoDevice*>::const_iterator it = devices.begin();
			it != devices.end();
			++it) {
				deviceIds.push_back(it->first);
	}
}