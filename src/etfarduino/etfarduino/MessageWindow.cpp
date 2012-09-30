// MessageWindow.cpp
// (c) 2012 Marko Lalic

// EtfArduino may be freely distributed under the MIT license.
// For all details and documentation:
// http://github.com/mlalic/etfarduino/

#include "stdafx.h"
#include "MessageWindow.h"
#include "etfarduinoAin.h"
#include "Lock.h"
#include <tchar.h>
#include <cstdio>
#include <cstdlib>			//Debugging

TCHAR const* MessageWindow::className = TEXT("EtfArduinoMessageWindow");
UINT MessageWindow::WM_ETFARDUINO = RegisterWindowMessage(TEXT("WM_ETFARDUINO"));

MessageWindow::MessageWindow() : mHandle(0), hThread(0) {
	// Register the class
	WNDCLASS classStruct = {
		0,								// style
		&MessageWindow::WindowProc,		// WndProc
		0,								// cbClsExtra
		0,								// cbWndExtra
		0,								// hInstance
		0,								// hIcon
		0,								// hCursor
		0,								// hbrBackground
		0,								// lpszMenuName
		className						// lpszClassName
	};
	if (!RegisterClass(&classStruct)) {
		// MessageWindow cannot work if the window has not been successfully registered...
		throw TEXT("Fatal error! Unable to register the MessageWindow class.");
	}
	// Start the message loop thread
	if (!StartMessageLoopThread()) {
		throw TEXT("Fatal error! Message loop thread could not be started.");
	}
}

MessageWindow& MessageWindow::getInstance() {
	// C++ singleton pattern -- static ensures only one instance, safe destruction and lazy init
	// Thread safe on C++11, but VC++'08 does not implement that
	// :TODO: Add a synch mechanism (double checked locking/membar).
	static MessageWindow mInstance;
	return mInstance;
}

MessageWindow::~MessageWindow() {
	// Send the close message to the window so
	// it can gracefully terminate the message loop
	SendMessage(mHandle, WM_CLOSE, 0, 0);
	// Wait 1sec for the thread
	WaitForSingleObject(hThread, 1000);
	CloseHandle(hThread);
	// And then unregister the window class
	UnregisterClass(className, 0);
}

bool MessageWindow::StartMessageLoopThread() {
	// Metoda samo zapocinje thread koji ce formirati window
	// i u kojem ce se izvrsavati message loop
	DWORD dwThreadId;
	hThread = CreateThread( 
		NULL,              // no security attribute 
		0,                 // default stack size 
		&MessageWindow::ThreadProc,    // thread proc
		(LPVOID) this,					// thread parameter 
		0,                 // not suspended 
		&dwThreadId);      // returns thread ID 
	if (hThread == NULL) {
		return false;
	} else {
		// Dati malo vremena da se thread inicijalizira
		// Ili mozda koristiti mutexe za critical regions gdje nastaju race conditions
		Sleep(50);
	}
	return true;
}

LRESULT CALLBACK MessageWindow::WindowProc(HWND msgWindow, UINT msg, WPARAM wp, LPARAM lp) {
	// Handle ETFARDUINO messages
	if (msg == WM_ETFARDUINO) {
		// If the message was sent using SendMessage instead of
		// PostMessage, send an immediate reply to the caller
		// so that it may continue its execution
		if (InSendMessage()) {
			ReplyMessage(TRUE);
		}
		// Preuzeti proslijedjene parametre
		UINT deviceId = (UINT)wp;
		UINT subsystem = (UINT)lp;
		// Provjeriti koji je subsystem u pitanju i handle it...
		if (subsystem == AI_SUBSYSTEM) {
			MessageWindow& msgWin = MessageWindow::getInstance();
			CetfarduinoAin* ain = (CetfarduinoAin*) msgWin.GetDevice(deviceId, subsystem);
			if (ain != 0) {
				ain->ReceivedMessage(wp, lp);
			} else {
				// Sta ako nije pronadjen uredjaj za koji je Message namijenjen...
			}
		}
		return 0;
	}
	// Handle normal window messages
	switch (msg)
	{
	case WM_CLOSE:
		// Only this thread can call DestroyWindow since it's the one 
		// that created it
		DestroyWindow(msgWindow);
		return 0;
	case WM_DESTROY:
		// MessageBox(NULL, TEXT("Good bye"), NULL, MB_OK);
		// Destroy Window
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(msgWindow, msg, wp, lp);
	}
	return 0;
}

bool MessageWindow::CreateMessageWindow() {
	// Kreirati prozor bez ikakvog stila s svrhom koristenja njegovog message queuea
	mHandle = CreateWindow(
		className,			// className
		0,					// title
		WS_CAPTION,			// style
		0, 0, 0, 0,			// x, y, cx, cy
		0,					// parent
		0,					// menu
		0,					// instance
		0);					// create struct
	return true;
}

DWORD WINAPI MessageWindow::ThreadProc(LPVOID param) {
	// Thread treba najprije da registruje prozor kojem ce se
	// proslijedjivati poruke
	MessageWindow& msgWin = *((MessageWindow*)param);
	msgWin.CreateMessageWindow();
	// Sada zapoceti message loop
	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0)) {
		DispatchMessage(&msg);
	}
	return 0;
}

bool MessageWindow::AddDevice(UINT deviceId, UINT subsystem, LPVOID device) {
	Lock lock(cs);
	devices.insert(
		std::pair<DeviceIdentificator, DevicePtr>(DeviceIdentificator(deviceId, subsystem),
												  device));

	return true;
}
bool MessageWindow::RemoveDevice(UINT deviceId, UINT subsystem) {
	Lock lock(cs);
	devices.erase(DeviceIdentificator(deviceId, subsystem));
	return true;
}

MessageWindow::DevicePtr MessageWindow::GetDevice(UINT deviceId, UINT subsystem) {
	Lock lock(cs);
	map_t::iterator it = devices.find(DeviceIdentificator(deviceId, subsystem));
	if (it != devices.end()) {
		return it->second;
	} else {
		return 0;
	}
}