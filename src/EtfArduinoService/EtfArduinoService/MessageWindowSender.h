// MessageWindowSender.h
// (c) 2012 Marko Lalic

// EtfArduino may be freely distributed under the MIT license.
// For all details and documentation:
// http://github.com/mlalic/etfarduino/

#ifndef __MESSAGEWINDOWSENDER_H__
#define __MESSAGEWINDOWSENDER_H__
#include <windows.h>
class MessageWindowSender {
public:
	// Constructors
	MessageWindowSender(UINT deviceId, int subsystem);
	// Destructor
	virtual ~MessageWindowSender();
	// Public member functions
	bool NotifyBufferFull();
	// Public static members
	static int const SUBSYSTEM_AI = 1;
	static int const SUBSYSTEM_AO = 2;
private:
	// Private members
	HWND mHandle;
	UINT const deviceId;
	int const subsystem;
	// Private static members
	static TCHAR const* className;
	static UINT WM_ETFARDUINO;
};

#endif