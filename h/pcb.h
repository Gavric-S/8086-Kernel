#ifndef PCB_H_
#define PCB_H_

#include <dos.h>

#include "thread.h"

#define MAX_PCB_NUM 2048
#define MAIN_PCB_ID 1023
#define MAX_STACK_SIZE 65535
#define SIGNAL_NUM 16

class KernelSem;
class BlockedOnPCB;
class SignalHandlerList;
class ReceivedSignalList;

class PCB {
public:

	PCB();
	PCB(StackSize stackSize, Time timeSlice, Thread* const ownerThread);
	~PCB();

	unsigned sp;
	unsigned ss;
	unsigned bp;
	unsigned state;
	int quantum;
	unsigned* stack;
	int timedOut;
	int lckCount;
	PCB* parentPCB;

	SignalHandlerList* signalHandlers[SIGNAL_NUM];
	int signalStatusVector[SIGNAL_NUM];

	static const char FINISHED;
	static const char INITIALIZED;
	static const char READY;
	static const char BLOCKED;
	static const char STOPPED;

	static volatile PCB* runningPCB;
	static PCB* PCBArray[MAX_PCB_NUM];
	static KernelSem spaceAvailable;
	static volatile unsigned int PCBCount;
	static volatile unsigned int activePCBCount;
	static PCB* mainPCB;

	void start();
	void waitToComplete();

	ID getId();

	static ID getRunningId();
	static Thread* getThreadById(ID);

	void unblockThreads();
	void blockThread(PCB* pcb);

	void signal(SignalId signal);
	void registerHandler(SignalId signal, SignalHandler handler);
	void unregisterAllHandlers(SignalId id);
	void swap(SignalId id, SignalHandler hand1, SignalHandler hand2);
	void blockSignal(SignalId signal);
	static void blockSignalGlobally(SignalId signal);
	void unblockSignal(SignalId signal);
	static void unblockSignalGlobally(SignalId signal);

	int isLocked(SignalId sig);
	void processSignals();

private:
	Thread* getThread();

	static void wrapper();

	ID id;
	BlockedOnPCB* blockedPCBs;
	static ID IDCounter;
	Thread* thread;

	ReceivedSignalList* receivedSignals;
};

extern void defaultHandler0();

#endif
