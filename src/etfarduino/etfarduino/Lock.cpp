// Lock.cpp
// (c) 2012 Marko Lalic

// EtfArduino may be freely distributed under the MIT license.
// For all details and documentation:
// http://github.com/mlalic/etfarduino/

#include "stdafx.h"
#include "Lock.h"

Lock::Lock(CriticalSection& criticalSection) : criticalSection(criticalSection) {
	criticalSection.enter();
}
Lock::~Lock() {
	criticalSection.leave();
}

void Lock::acquire() {
	criticalSection.enter();
}

void Lock::release() {
	criticalSection.leave();
}
