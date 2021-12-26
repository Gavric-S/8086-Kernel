#ifndef KERNELEV_H_
#define KERNELEV_H_

#include "event.h"

class PCB;

class KernelEv {
public:
	KernelEv(IVTNo ivtNo);
	~KernelEv();

	void wait();
	void signal();
private:
	int v;
	IVTNo ivtNum;
	PCB* blockedPCB;
	PCB* ownerPCB;

	void blockPCB();
	void unblockPCB();
};

#endif
