// DeviceMap.h
// (c) 2012 Marko Lalic

// EtfArduino may be freely distributed under the MIT license.
// For all details and documentation:
// http://github.com/mlalic/etfarduino/

#ifndef __DEVICEMAP_H__
#define __DEVICEMAP_H__
#include <map>
#include "ArduinoDevice.h"
#include "Lock.h"
#include <vector>
class DeviceMap
{
public:
	DeviceMap();
	~DeviceMap();
	// Inserts a device into the map and returns its ID
	UINT Insert(LPCWSTR serialPortName);
	void Remove(UINT deviceId);
	ArduinoDevice* Find(UINT deviceId);
	void GetAllRegisteredDevices(std::vector<UINT>& devices);
private:
	std::map<UINT, ArduinoDevice*> devices;
	UINT nextId;
	CriticalSection cs;
};

#endif