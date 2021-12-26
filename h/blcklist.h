#ifndef BLCKLIST_H_
#define BLCKLIST_H_

#include "thread.h"

typedef struct BlockedNodeSem {
	PCB* pcb;
	int quantum;
	struct BlockedNodeSem* next;

	BlockedNodeSem(PCB* toBlock, int maxTimeToWait) : pcb(toBlock), next(0), quantum(maxTimeToWait) {}
} blckdNodeSem;

class BlockedOnSem {
public:
	BlockedOnSem();
	~BlockedOnSem();

	void blockPCB(Time maxTimeToWait);
	void unblockPCB();
	void updateTime();

	unsigned getCount() const;
private:
	blckdNodeSem* head;
	blckdNodeSem* tail;
	unsigned count;

	void removeNode(blckdNodeSem* prev, blckdNodeSem** target);
};

typedef struct BlockedNodePCB {
	PCB* pcb;
	struct BlockedNodePCB* next;

	BlockedNodePCB(PCB* toBlock) : pcb(toBlock), next(0) {}
} blckdNodePCB;

class BlockedOnPCB {
public:
	BlockedOnPCB();
	~BlockedOnPCB();

	void blockPCB(PCB* pcb);
	void unblockPCBs();

	unsigned getCount() const;
private:
	blckdNodePCB* head;
	blckdNodePCB* tail;
	unsigned count;
};

#endif
