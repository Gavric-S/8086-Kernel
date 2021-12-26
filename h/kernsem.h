#ifndef _KERNELSEM_H_
#define _KERNELSEM_H_

#include "thread.h"

#define MAX_SEM_NUM 1024

class BlockedOnSem;
class KernelSemList;

class KernelSem {
public:
	KernelSem(int init);
	~KernelSem();

	static KernelSemList KernSemList;

	int wait(Time maxTimeToWait);
	int signal(int n=0);
	void updateTime();

	int val() const;
private:
	int v;
	BlockedOnSem* blockedPCBs;
	static ID IDCounter;
};

#endif
