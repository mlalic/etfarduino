// CriticalSection.cpp
// (c) 2012 Marko Lalic

// EtfArduino may be freely distributed under the MIT license.
// For all details and documentation:
// http://github.com/mlalic/etfarduino/

#include "stdafx.h"
#include "CriticalSection.h"

CriticalSection::CriticalSection() {
	InitializeCriticalSection(&cs);
}

CriticalSection::~CriticalSection() {
	DeleteCriticalSection(&cs);
}

void CriticalSection::enter() {
	EnterCriticalSection(&cs);
}

void CriticalSection::leave() {
	LeaveCriticalSection(&cs);
}
