// BufferQueue.h
// (c) 2012 Marko Lalic

// EtfArduino may be freely distributed under the MIT license.
// For all details and documentation:
// http://github.com/mlalic/etfarduino/

#ifndef __BUFFERQUEUE_H__
#define __BUFFERQUEUE_H__
#include <queue>
#include "CriticalSection.h"
class BufferQueue
{
public:
	typedef unsigned short* buffer_t;

	BufferQueue();
	~BufferQueue();

	void Push(buffer_t value);
	buffer_t Pop();
	void Clear();
private:
	std::queue<buffer_t> queue;
	CriticalSection cs;
};

#endif