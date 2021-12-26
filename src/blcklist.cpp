#include "blcklist.h"
#include "pcb.h"
#include "SCHEDULE.H"
#include "preempt.h"

#include "iostream.h"

BlockedOnSem::BlockedOnSem() : head(0), tail(0), count(0) {}

BlockedOnSem::~BlockedOnSem() {
	blckdNodeSem* old = head;
	blckdNodeSem* nxt;
	while (old) {
		nxt = old->next;
		lock
			delete old;
		unlock
		old = nxt;
	}
	head = 0;
	tail = 0;
	count = 0;
}

void BlockedOnSem::blockPCB(Time maxTimeToWait) {
	lock
		PCB::runningPCB->state = PCB::BLOCKED;
	unlock
	int time;
	if (maxTimeToWait == 0) time = -1;
	else time = maxTimeToWait;
	lock
		blckdNodeSem* newNode = new blckdNodeSem(((PCB*)PCB::runningPCB), time);
	unlock
	if (!head) {
		head = newNode;
		tail = newNode;
	}
	else {
		tail->next = newNode;
		tail = newNode;
	}
	count++;
	dispatch();
}

void BlockedOnSem::unblockPCB() {
	if (head) {
		PCB* tmpPCB = head->pcb;
		tmpPCB->state = PCB::READY;
		tmpPCB->timedOut = 0;
		Scheduler::put(tmpPCB);
		blckdNodeSem *tmp = head;
		head = head->next;
		if (!head) tail = 0;
		lock
			delete tmp;
		unlock
		count--;
	}
}

void BlockedOnSem::updateTime() {
	blckdNodeSem* curr = head;
	blckdNodeSem* prev = 0;
	while (curr != 0) {
		if (curr->quantum > 0) {
			curr->quantum = curr->quantum - 1;
			if (curr->quantum == 0) {
				PCB* tmpPCB = curr->pcb;
				tmpPCB->state = PCB::READY;
				tmpPCB->timedOut = 1;
				Scheduler::put(tmpPCB);
				removeNode(prev, &curr);
			}
			else {
				prev = curr;
				curr = curr->next;
			}
		}
		else {
			prev = curr;
			curr = curr->next;
		}
	}
}

void BlockedOnSem::removeNode(blckdNodeSem* prev, blckdNodeSem** target) {
	if (head != *target && tail != *target) prev->next = (*target)->next;
	else if (head == *target) {
		head = (*target)->next;
		if (tail == *target) tail = 0;
	}
	else { tail = prev; prev->next = 0; }
	blckdNodeSem* next = (*target)->next;
	lock
		delete (*target);
	unlock
	count--;
	*target = next;
}

unsigned BlockedOnSem::getCount() const {
	return count;
}


BlockedOnPCB::BlockedOnPCB() : head(0), tail(0), count(0) {}

BlockedOnPCB::~BlockedOnPCB() {
	blckdNodePCB* old = head;
	blckdNodePCB* next;
	while (old) {
		next = old->next;
		lock
			delete old;
		unlock
		old = next;
	}
	head = 0;
	tail = 0;
	count = 0;
}

void BlockedOnPCB::blockPCB(PCB* pcb) {
	PCB::runningPCB->state = PCB::BLOCKED;
	lock
		blckdNodePCB* newNode = new blckdNodePCB(pcb);
	unlock
	if (!head) {
		head = newNode;
		tail = newNode;
	}
	else {
		tail->next = newNode;
		tail = newNode;
	}
	count++;
	dispatch();
}

void BlockedOnPCB::unblockPCBs() {
	blckdNodePCB* old = head;
	blckdNodePCB* nxt;
	while (old != 0) {
		nxt = old->next;
		PCB* tmpPCB = old->pcb;
		tmpPCB->state = PCB::READY;
		Scheduler::put(tmpPCB);
		lock
			delete old;
		unlock
		old = nxt;
	}
	head = 0;
	tail = 0;
	count = 0;
}

unsigned BlockedOnPCB::getCount() const {
	return count;
}
