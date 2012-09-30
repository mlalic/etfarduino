// dllmain.cpp : Implementation of DllMain.

#include "stdafx.h"
#include <initguid.h>
#include "resource.h"
#include "etfarduino_i.h"
#include "dllmain.h"

CComModule _Module;



// DLL Entry Point
extern "C" BOOL WINAPI DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{
	hInstance;
	return _Module.DllMain(dwReason, lpReserved); 
}
