// Lock.h
// (c) 2012 Marko Lalic

// EtfArduino may be freely distributed under the MIT license.
// For all details and documentation:
// http://github.com/mlalic/etfarduino/

#ifndef __LOCK_H__
#define __LOCK_H__
#include <windows.h>
#include "CriticalSection.h"
class Lock
{
public:
	Lock(CriticalSection& criticalSection);
	~Lock();
private:
	void acquire();
	void release();
	CriticalSection& criticalSection;
};

#endif