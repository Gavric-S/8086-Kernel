#ifndef IDLETHRD_H_
#define IDLETHRD_H_

#include "thread.h"

class IdleThread : public Thread {
public:
	IdleThread();
	~IdleThread();

	static PCB* idlePCB;
	static IdleThread* idleThread;
protected:
	virtual void run();
};

#endif
