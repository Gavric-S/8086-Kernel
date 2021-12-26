#include "idlethrd.h"
#include "pcb.h"
#include "preempt.h"

#include <iostream.h>

IdleThread* IdleThread::idleThread;
PCB* IdleThread::idlePCB;

IdleThread::IdleThread() : Thread(defaultStackSize, 1) {
	// inicijalizacija idle	niti
	IdleThread::idleThread = this;
	IdleThread::idlePCB = PCB::PCBArray[this->getId()];
	for (int i = 0; i < SIGNAL_NUM; i++) {
		IdleThread::idlePCB->signalStatusVector[i] = 1;
	}
}

IdleThread::~IdleThread() {}

void IdleThread::run() {
	while (1);
}
