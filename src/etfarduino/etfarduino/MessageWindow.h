// MessageWindow.h
// (c) 2012 Marko Lalic

// EtfArduino may be freely distributed under the MIT license.
// For all details and documentation:
// http://github.com/mlalic/etfarduino/

#ifndef __MESSAGEWINDOW_H__
#define __MESSAGEWINDOW_H__
#include "stdafx.h"
#include "CriticalSection.h"
#include <map>
class MessageWindow {
public:
	// Destructor
	virtual ~MessageWindow();
	// Public member functions
	bool AddDevice(UINT deviceId, UINT subsystem, LPVOID device);
	bool RemoveDevice(UINT deviceId, UINT subsystem);
	// Public static member functions
	static MessageWindow& getInstance();
private:
	// Internal structures to be used
	typedef LPVOID DevicePtr;
	struct DeviceIdentificator {
		UINT deviceId;
		UINT subsystem;		// AI, AO[, DI?]
		DeviceIdentificator(UINT deviceId, UINT subsystem) : deviceId(deviceId), subsystem(subsystem) { }
		bool operator< (DeviceIdentificator const& rhs) const { return deviceId < rhs.deviceId; }
	};
	typedef std::map<DeviceIdentificator, DevicePtr> map_t;
	// Constructors
	MessageWindow();
	// Private members
	HWND mHandle;
	HANDLE hThread;
	map_t devices;
	CriticalSection cs;
	// Private member functions
	bool StartMessageLoopThread();
	bool CreateMessageWindow();
	DevicePtr GetDevice(UINT deviceId, UINT subsystem);
	// Private static members
	static TCHAR const* className;
	// A Window Message registered with the OS
	static UINT WM_ETFARDUINO;
	// Subsystem identifier constants
	static UINT const AI_SUBSYSTEM = 1;
	static UINT const AO_SUBSYSTEM = 2;
	// Private static member functions
	static LRESULT CALLBACK WindowProc(HWND msgWindow, UINT msg, WPARAM wp, LPARAM lp);
	static DWORD WINAPI ThreadProc(LPVOID param);
};

#endif