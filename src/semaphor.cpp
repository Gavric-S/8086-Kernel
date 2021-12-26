#include "semaphor.h"
#include "kernsem.h"
#include "preempt.h"

Semaphore* mainSem = new Semaphore(0);

Semaphore::Semaphore(int init) {
	lock
		myImpl = new KernelSem(init);
	unlock
}

Semaphore::~Semaphore() {
	lock
		delete myImpl;
	unlock
}

int Semaphore::wait(Time maxTimeToWait) {
	return myImpl->wait(maxTimeToWait);
}

int Semaphore::signal(int n) {
	return myImpl->signal(n);
}

int Semaphore::val() const {
	return myImpl->val();
}

