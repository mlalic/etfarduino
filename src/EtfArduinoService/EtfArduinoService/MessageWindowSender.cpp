// MessageWindowSender.cpp
// (c) 2012 Marko Lalic

// EtfArduino may be freely distributed under the MIT license.
// For all details and documentation:
// http://github.com/mlalic/etfarduino/

#include "stdafx.h"
#include "MessageWindowSender.h"
#include <tchar.h>
#include <cstdio>
#include <cstdlib>			//Debugging
TCHAR const* MessageWindowSender::className = TEXT("EtfArduinoMessageWindow");
UINT MessageWindowSender::WM_ETFARDUINO = RegisterWindowMessage(TEXT("WM_ETFARDUINO"));

MessageWindowSender::MessageWindowSender(UINT deviceId, int subsystem) :
  mHandle(0),
  deviceId(deviceId),
  subsystem(subsystem) {
	// :TODO: Maybe block on a semaphore which is signalled from the process which
    // is supposed to create the window only when the window is created to avoid
	// a possible race condition.
	// Try to find the window and get its handle.
	mHandle = FindWindow(className, 0);
}

MessageWindowSender::~MessageWindowSender() {

}

bool MessageWindowSender::NotifyBufferFull() {
	if (mHandle == 0) {
		printf("Null handle -- not cool\n");
		return false;
	}
	/*if (!SendMessage(mHandle, WM_ETFARDUINO, (WPARAM) deviceId, (LPARAM) subsystem)) {
		int gle = GetLastError();
		// printf("...NOT SENT! GLE = %d\n", GetLastError());
		return false;
	}*/
	if (!PostMessage(mHandle, WM_ETFARDUINO, (WPARAM) deviceId, (LPARAM) subsystem)) {
		return false;
	}
	return true;
}
