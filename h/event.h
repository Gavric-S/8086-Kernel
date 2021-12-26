#ifndef EVENT_H_
#define EVENT_H_

typedef unsigned char IVTNo;

class KernelEv;

class Event {
public:
	Event(IVTNo ivtNo);
	~Event();

	void wait();
protected:
	friend class KernelEv;

	void signal();
private:
	KernelEv* myImpl;
};

#include "ivtentry.h"

#endif
