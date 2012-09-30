// CriticalSection.h
// (c) 2012 Marko Lalic

// EtfArduino may be freely distributed under the MIT license.
// For all details and documentation:
// http://github.com/mlalic/etfarduino/

#ifndef __CRITICALSECTION_H__
#define __CRITICALSECTION_H__
#include <windows.h>
class CriticalSection
{
public:
	CriticalSection();
	~CriticalSection();

	void enter();
	void leave();
private:
	CRITICAL_SECTION cs;
};

#endif