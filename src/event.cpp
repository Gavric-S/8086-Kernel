#include "event.h"
#include "kernelev.h"
#include "preempt.h"

Event::Event(IVTNo ivtNo) {
	lock
		myImpl = new KernelEv(ivtNo);
	unlock
}

Event::~Event() {
	lock
		delete myImpl;
	unlock
}

void Event::wait() {
	myImpl->wait();
}

void Event::signal() {
	myImpl->signal();
}
