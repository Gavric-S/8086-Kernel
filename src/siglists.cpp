#include "preempt.h"
#include "siglists.h"
#include "pcb.h"

#include <iostream.h>

SignalHandlerList::SignalHandlerList() : head(0), tail(0), count(0) {}

SignalHandlerList::~SignalHandlerList() {
	unregisterAllHandlers();
}

void SignalHandlerList::registerHandler(SignalHandler sh) {
	lock
		sigHandlerNode* newNode = new sigHandlerNode(sh);
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
}

void SignalHandlerList::unregisterAllHandlers() {
	sigHandlerNode* old = head;
	sigHandlerNode* nxt;
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

void SignalHandlerList::swap(SignalHandler hand1, SignalHandler hand2) {
	sigHandlerNode* nodeHand1 = head;
	sigHandlerNode* nodeHand2 = head;
	while (nodeHand1 && nodeHand2) {
		if (nodeHand1->sigHandler == hand1 && nodeHand2->sigHandler == hand2) break;
		if (nodeHand1->sigHandler != hand1) nodeHand1 = nodeHand1->next;
		if (nodeHand2->sigHandler != hand2) nodeHand2 = nodeHand2->next;
	}
	if (nodeHand1 && nodeHand2) {
		SignalHandler tmp = nodeHand1->sigHandler;
		nodeHand1->sigHandler = nodeHand2->sigHandler;
		nodeHand2->sigHandler = tmp;
	}
}

void SignalHandlerList::callHandlers() {
	sigHandlerNode* curr = head;
	while (curr) {
		curr->sigHandler();
		curr = curr->next;
	}
}

SignalHandlerList* SignalHandlerList::clone(SignalHandlerList* toClone) {
	SignalHandlerList* toReturn = new SignalHandlerList();
	sigHandlerNode* curr = toClone->head;
	while (curr) {
		toReturn->registerHandler(curr->sigHandler);
		curr = curr->next;
	}
	return toReturn;
}

int SignalHandlerList::getCount() {
	return count;
}

ReceivedSignalList::ReceivedSignalList(PCB* pcb) : head(0), tail(0), count(0), myPCB(pcb) {}

ReceivedSignalList::~ReceivedSignalList() {
	recSignalNode* old = head;
	recSignalNode* nxt;
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

void ReceivedSignalList::addSignal(SignalId sig) {
	lock
		recSignalNode* newNode = new recSignalNode(sig);
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
}

void ReceivedSignalList::processSignals() {
	recSignalNode* curr = head;
	recSignalNode* prev = 0;
	while (curr != 0) {
		if (PCB::mainPCB->isLocked(curr->signal) == 0 && myPCB->isLocked(curr->signal) == 0) {
			myPCB->signalHandlers[curr->signal]->callHandlers();
			removeNode(prev, &curr);
		}
		else {
			prev = curr;
			curr = curr->next;
		}
	}
}

void ReceivedSignalList::removeNode(recSignalNode* prev, recSignalNode** target) {
	if (head != *target && tail != *target) prev->next = (*target)->next;
	else if (head == *target) {
		head = (*target)->next;
		if (tail == *target) tail = 0;
	}
	else { tail = prev; prev->next = 0; }
	recSignalNode* next = (*target)->next;
	lock
		delete (*target);
	unlock
	count--;
	*target = next;
}

