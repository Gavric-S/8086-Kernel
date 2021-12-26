#include "kernelev.h"
#include "pcb.h"
#include "preempt.h"
#include "SCHEDULE.H"
#include "ivtentry.h"

KernelEv::KernelEv(IVTNo ivtNo) : v(0), blockedPCB(0), ownerPCB((PCB*)PCB::runningPCB), ivtNum(ivtNo) {
	lock
		if (IVTEntry::ivt[ivtNum] != 0) IVTEntry::ivt[ivtNum]->setKernelEv(this);
	unlock
}

KernelEv::~KernelEv() {
	lock
		if (IVTEntry::ivt[ivtNum] != 0) IVTEntry::ivt[ivtNum]->setKernelEv(0);
	unlock
}

void KernelEv::wait() {
	lock
		if (((PCB*)PCB::runningPCB) == ownerPCB) {
			if (v == 0) blockPCB();
			else v = 0;
		}
	unlock
}

void KernelEv::signal() {
	lock
		if (blockedPCB == 0) v = 1;
		else { unblockPCB(); v = 0; }
	unlock
}

void KernelEv::blockPCB() {
	blockedPCB = (PCB*)PCB::runningPCB;
	blockedPCB->state = PCB::BLOCKED;
	dispatch();
}

void KernelEv::unblockPCB() {
	blockedPCB->state = PCB::READY;
	Scheduler::put(blockedPCB);
	blockedPCB = 0;
}
