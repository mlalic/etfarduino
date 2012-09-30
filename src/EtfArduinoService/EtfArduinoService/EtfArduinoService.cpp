// EtfArduinoService.cpp : main project file
// (c) 2012 Marko Lalic

// EtfArduino may be freely distributed under the MIT license.
// For all details and documentation:
// http://github.com/mlalic/etfarduino/

#include "stdafx.h"
#include <windows.h> 
#include <tchar.h>

#include "MessageWindowSender.h"
#include "ArduinoDevice.h"
#include "PipeCommunicator.h"
#include "DeviceMap.h"
#include "RequestHandlerFactory.h"
#include "RequestHandlerFactoryImpl.h"

// Forward declaration of the function which serves to service each connected client
DWORD WINAPI InstanceThread(LPVOID);
// Holds all devices which register to the service.
// It is a globally accessable object, but its internals are thread safe.
DeviceMap registeredDevices;

int main()
{
	// Debug purposes -- automatically add a device on COM3 without having to
	// manually do so every time the service is restarted
	// registeredDevices.Insert(_T("COM3"));

	// In the loop, we create an object which creates a new pipe instance and then
	// waits for a client to connect.
	// Once it does, a thread is created to service the client. The thread takes ownership
	// of the PipeCommunicator object.
	// If the client connection fails, the object is destroyed, thereby freeing the resources
	// it was holding, and the loop is repeated infinitely.
	for (;;)
	{
		// :TODO: Exception on allocation
		PipeCommunicator* pipe = new PipeCommunicator(_T("\\\\.\\pipe\\etfarduino-{522590F9-C51E-4711-B6E2-9CECC7C3FD91}"));
		// Wait for the client to connect
		if (pipe->Connect())
		{ 
			// Create a thread for this client. 
			HANDLE hThread = CreateThread( 
				NULL,             // no security attribute 
				0,                // default stack size 
				InstanceThread,   // thread proc
				(LPVOID) pipe,    // thread parameter 
				0,                // not suspended 
				NULL);			  // no thread ID
			if (hThread == NULL) {
				// Thread creation failed, pipe ownership still in this thread!
				printf("CreateThread failed, GLE=%d.\n", GetLastError()); 
				delete pipe;
			} else {
				CloseHandle(hThread); 
			}
		} else {
			// The client could not connect, so close the pipe. 
			printf("Client couldn't connect\n");
			delete pipe;
		}
	} 
	return 0; 
}

// This function serves the clients which connect to the pipe.
// It reads requests and handles them apropriately.
// It may send responses over the open pipe connection.
// When the client sends a message indicating that it's disconnecting,
// the thread closes the open pipe connection.
DWORD WINAPI InstanceThread(LPVOID lpvParam)
{
	// First check whether the parameter is valid.
	if (lpvParam == NULL) {
		printf( "\nERROR - Pipe Server Failure:\n");
		printf( "   InstanceThread got an unexpected NULL value in lpvParam.\n");
		printf( "   InstanceThread exitting.\n");
		return -1;
	}
	// Print verbose messages. In production code, this should be for debugging only.
	printf("InstanceThread created, receiving and processing messages.\n");
	// The thread's parameter is a handle to a pipe object instance.
	std::tr1::shared_ptr<PipeCommunicator> pipe((PipeCommunicator*) lpvParam);
	// Allocate a buffer for reading the requests.
	std::vector<PipeCommunicator::message_t> request(PipeCommunicator::bufferSize);
	PipeCommunicator::message_t* pchRequest = &request[0];
	// Factory which will produce all handlers for communication over this pipe
	std::auto_ptr<RequestHandlerFactory> factory(
		new RequestHandlerFactoryImpl(
			registeredDevices,
			pipe));
	for (;;) {
		DWORD cbBytesRead = 0;
		bool success = 
			pipe->RetrieveMessage(pchRequest, PipeCommunicator::bufferSize * sizeof(PipeCommunicator::message_t), &cbBytesRead);

		if (success) {
			printf("Retrieved a message %d\n", pchRequest[0]);
		} else {
			printf("Couldn't retrieve message %d\n", GetLastError());
		}

		if (!success || cbBytesRead == 0) {
			// There's no way to decode the message as it wasn't successfully retrieved.
			if (GetLastError() == ERROR_BROKEN_PIPE) {
				break;
			}
			continue;
		}
		std::auto_ptr<RequestHandler> handler(factory->GetRequestHandler(pchRequest));
		if (handler.get() != 0) {
			RequestHandler::Status const status = 
				handler->run();
			printf("Handled message %d\n", pchRequest[0]);
			if (status == RequestHandler::Status::EXIT) break;
		} else {
			// Unknown command - no handler exists
			printf("Unknown command - no handler exists\n");
		}
	}
	printf("InstanceThread exitting.\n");
	return 0;
}
