// BufferQueue.cpp
// (c) 2012 Marko Lalic

// EtfArduino may be freely distributed under the MIT license.
// For all details and documentation:
// http://github.com/mlalic/etfarduino/

#include "stdafx.h"
#include "BufferQueue.h"
#include "Lock.h"

BufferQueue::BufferQueue() { 
	// Empty
}

BufferQueue::~BufferQueue() {
	// Release memory held by buffers still owned by the queue
	// If Clear throws, the application will crash since a dtor mustn't throw...
	// Can't think about this right now ...
	Clear();
}

void BufferQueue::Push(unsigned short* value) {
	Lock lock(cs);
	queue.push(value);
}

unsigned short* BufferQueue::Pop() {
	Lock lock(cs);
	unsigned short* value = queue.front();
	queue.pop();
	return value;
}

void BufferQueue::Clear() {
	Lock lock(cs);
	while (!queue.empty()) {
		unsigned short* buffer = queue.front();
		delete[] buffer;
		queue.pop();
	}
}
