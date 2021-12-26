#ifndef _SEMAPHORE_H_
#define _SEMAPHORE_H_

#include "thread.h"

class KernelSem;

class Semaphore {
public:
	Semaphore(int init=1);
	virtual ~Semaphore();

	virtual int wait (Time maxTimeToWait);
	virtual int signal(int n=0);

	int val () const;
private:
	KernelSem* myImpl;
};

#endif
