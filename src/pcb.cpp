#include "pcb.h"
#include "blcklist.h"
#include "kernsem.h"
#include "preempt.h"
#include "SCHEDULE.H"
#include "syncprnt.h"
#include "siglists.h"

#include "iostream.h"

const char PCB::INITIALIZED = 1;
const char PCB::READY = 2;
const char PCB::BLOCKED = 3;
const char PCB::FINISHED = 4;
const char PCB::STOPPED = 5;

volatile PCB* PCB::runningPCB;
PCB* PCB::PCBArray[MAX_PCB_NUM];
KernelSem PCB::spaceAvailable(MAX_PCB_NUM);
volatile unsigned int PCB::PCBCount = 0;
volatile unsigned int PCB::activePCBCount = 0;
PCB* PCB::mainPCB;

ID PCB::IDCounter = 0;


PCB::PCB() { // mainPCB constructor
	lock
		PCB::PCBArray[MAIN_PCB_ID] = this;
		PCB::PCBCount++;
		id = MAIN_PCB_ID;
		thread = 0;
		stack = 0;
		state = PCB::READY;
		quantum = defaultTimeSlice;
		timedOut = 0;
		blockedPCBs = 0;
		lckCount = 0;
		receivedSignals = new ReceivedSignalList(this);
		for (int i = 0; i < SIGNAL_NUM; i++) {
			signalHandlers[i] = new SignalHandlerList();
			signalStatusVector[i] = 0;
		}
	unlock
}

PCB::PCB(StackSize stackSize, Time timeSlice, Thread* const ownerThread) {
	spaceAvailable.wait(0);
	lock
		id = PCB::IDCounter;
		while (PCB::PCBArray[id] != 0) id = (id + 1) % MAX_PCB_NUM;
		PCB::PCBArray[id] = this;
		PCB::IDCounter = (id + 1) % MAX_PCB_NUM;
		PCB::PCBCount++;
	unlock
	thread = ownerThread;
	if (stackSize > MAX_STACK_SIZE) stackSize = MAX_STACK_SIZE;
	int StackElemNum = stackSize / sizeof(unsigned);
	lock
		stack = new unsigned[StackElemNum];
	unlock
#ifndef BCC_BLOCK_IGNORE
	// PSW
	stack[StackElemNum - 1] = 0x200;

	// function with run() function of thread
	stack[StackElemNum - 2] = FP_SEG(PCB::wrapper);
	stack[StackElemNum - 3] = FP_OFF(PCB::wrapper);

	// initialize sp, bp and ss
	sp = FP_OFF(stack + StackElemNum - 12);
	ss = FP_SEG(stack + StackElemNum - 12);
	bp = sp;
#endif
	state = PCB::INITIALIZED;
	quantum = timeSlice;
	timedOut = 0;
	lckCount = 0;
	lock
		blockedPCBs = new BlockedOnPCB();
		receivedSignals = new ReceivedSignalList(this);
		// initialize signal structures
		// inherit parent settings
		for (int i = 0; i < SIGNAL_NUM; i++) {
			signalHandlers[i] = SignalHandlerList::clone(PCB::runningPCB->signalHandlers[i]);
			signalStatusVector[i] = ((PCB*)PCB::runningPCB)->signalStatusVector[i];
		}
		signalHandlers[0]->registerHandler(defaultHandler0);
	unlock
	parentPCB = ((PCB*)PCB::runningPCB);
}

PCB::~PCB() {
	lock
		delete blockedPCBs;
		delete[] stack;
		PCB::PCBArray[id] = 0;
		PCB::PCBCount--;
		delete receivedSignals;
		for (int i = 0; i < SIGNAL_NUM; i++) delete signalHandlers[i];
		spaceAvailable.signal();
	unlock
}

void PCB::start() {
	lock
		if (state == PCB::INITIALIZED) {
			state = PCB::READY;
			PCB::activePCBCount++;
			Scheduler::put(this);
		}
	unlock
}

void PCB::waitToComplete() {
	lock
		if (state == PCB::READY || state == PCB::BLOCKED || state == PCB::STOPPED) blockThread((PCB*)PCB::runningPCB);
	unlock
}

Thread* PCB::getThread() {
	return thread;
}

ID PCB::getId() {
	return id;
}

ID PCB::getRunningId() {
	return ((PCB*)PCB::runningPCB)->getId();
}

Thread* PCB::getThreadById(ID id) {
	if (PCB::PCBArray[id]) return PCB::PCBArray[id]->getThread();
	return 0;
}

void PCB::blockThread(PCB* pcb) {
	lock
		blockedPCBs->blockPCB(pcb);
	unlock
}

void PCB::unblockThreads() {
	lock
		blockedPCBs->unblockPCBs();
	unlock
}

void PCB::wrapper() {
	PCB::runningPCB->thread->run();
	lock
		((PCB*)PCB::runningPCB)->unblockThreads();
		PCB::runningPCB->state = PCB::FINISHED;
		PCB::activePCBCount--;
		PCB::runningPCB->parentPCB->signal(1);
		((PCB*)PCB::runningPCB)->signal(2);
		((PCB*)PCB::runningPCB)->processSignals();
		dispatch();
	unlock
}

void PCB::signal(SignalId signal) {
	lock
		if ((signal >= 0 && signal <= 15) && signalHandlers[signal]->getCount () > 0) {
			if (signal != 1 && signal != 2) {
				receivedSignals->addSignal(signal);
			}
			else if ((PCB*)PCB::runningPCB == PCB::mainPCB) {
				receivedSignals->addSignal(signal);
			}
			else if (signal == 1 && (PCB*)PCB::runningPCB->parentPCB == this && ((PCB*)PCB::runningPCB)->state == PCB::FINISHED) {
				receivedSignals->addSignal(signal);
			}
			else if (signal == 2 && (PCB*)PCB::runningPCB == this && ((PCB*)PCB::runningPCB)->state == PCB::FINISHED) {
				receivedSignals->addSignal(signal);
			}
		}
	unlock
}

void PCB::registerHandler(SignalId signal, SignalHandler handler) {
	lock
		signalHandlers[signal]->registerHandler(handler);
	unlock
}

void PCB::unregisterAllHandlers(SignalId id) {
	lock
		signalHandlers[id]->unregisterAllHandlers();
	unlock
}

void PCB::swap(SignalId id, SignalHandler hand1, SignalHandler hand2) {
	lock
		signalHandlers[id]->swap(hand1, hand2);
	unlock
}

void PCB::blockSignal(SignalId signal) {
	lock
		signalStatusVector[signal] = 1;
	unlock
}

void PCB::blockSignalGlobally(SignalId signal) {
	lock
		PCB::mainPCB->blockSignal(signal);
	unlock
}

void PCB::unblockSignal(SignalId signal) {
	lock
		signalStatusVector[signal] = 0;
	unlock
}

void PCB::unblockSignalGlobally(SignalId signal) {
	lock
		PCB::mainPCB->unblockSignal(signal);
	unlock
}

int PCB::isLocked(SignalId signal) {
	return signalStatusVector[signal];
}

void PCB::processSignals() {
	lock
		receivedSignals->processSignals();
	unlock
}

void defaultHandler0() {
	lock
		((PCB*)PCB::runningPCB)->unblockThreads();
		PCB::runningPCB->state = PCB::STOPPED;
		PCB::activePCBCount--;
	    dispatch();
	unlock
}
