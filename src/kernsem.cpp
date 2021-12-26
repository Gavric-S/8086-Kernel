#include "kernsem.h"
#include "preempt.h"
#include "pcb.h"
#include "semlist.h"

#include <iostream.h>
#include "blcklist.h"

KernelSemList KernelSem::KernSemList;

ID KernelSem::IDCounter = 0;

KernelSem::KernelSem(int init) : v(init) {
	lock
		blockedPCBs = new BlockedOnSem();
		KernelSem::KernSemList.addKernSem(this);
	unlock
}

KernelSem::~KernelSem() {
	lock
		delete blockedPCBs;
		KernelSem::KernSemList.removeKernSem(this);
	unlock
}

int KernelSem::wait(Time maxTimeToWait) {
	lock
		int ret = 1;
		v = v - 1;
		if (v < 0) {
			blockedPCBs->blockPCB(maxTimeToWait);
			if (PCB::runningPCB->timedOut == 1) {
				ret = 0;
				PCB::runningPCB->timedOut = 0;
			}
		}
	unlock
	return ret;
}

int KernelSem::signal(int n) {
	lock
	if (n == 0) {
		v = v + 1;
		if (v <= 0) blockedPCBs->unblockPCB();
		unlock
		return n;
	}
	else if (n > 0) {
		v = v + n;
		if (n >= blockedPCBs->getCount()) {
			unsigned ret = blockedPCBs->getCount();
			for (int i = 0; i < ret; i++) blockedPCBs->unblockPCB();
			unlock
			return ret;
		}
		else {
			for (int i = 0; i < n; i++) blockedPCBs->unblockPCB();
			unlock
			return n;
		}
	}
	else {
		unlock
		return n;
	}
}

int KernelSem::val() const {
	return v;
}

void KernelSem::updateTime() {
	lock
		blockedPCBs->updateTime();
	unlock
}
