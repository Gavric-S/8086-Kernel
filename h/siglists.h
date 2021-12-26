#ifndef SIGLISTS_H_
#define SIGLISTS_H_

#include "thread.h"

typedef struct SignalHandlerNode {
	SignalHandler sigHandler;
	struct SignalHandlerNode* next;

	SignalHandlerNode(SignalHandler sh) : sigHandler(sh), next(0) {}
} sigHandlerNode;

class SignalHandlerList {
public:
	SignalHandlerList();
	~SignalHandlerList();

	void registerHandler(SignalHandler sh);
	void unregisterAllHandlers();
	void swap(SignalHandler hand1, SignalHandler hand2);
	void callHandlers();

	int getCount();

	static SignalHandlerList* clone(SignalHandlerList* toClone);
private:
	sigHandlerNode* head;
	sigHandlerNode* tail;
	int count;
};

typedef struct ReceivedSignalNode {
	SignalId signal;
	struct ReceivedSignalNode* next;

	ReceivedSignalNode(SignalId sig) : signal(sig), next(0) {}
} recSignalNode;

class ReceivedSignalList {
public:
	ReceivedSignalList(PCB* pcb);
	~ReceivedSignalList();

	void addSignal(SignalId sig);
	void processSignals();
private:
	recSignalNode *head;
	recSignalNode *tail;
	int count;
	PCB* myPCB;

	void removeNode(recSignalNode* prev, recSignalNode** target);
};

#endif
