#include "semlist.h"
#include "preempt.h"
#include "kernsem.h"

KernelSemList::KernelSemList() : head(0), tail(0), count(0) {}

KernelSemList::~KernelSemList() {
	kernSemNode* old = head;
	kernSemNode* nxt;
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


void KernelSemList::addKernSem(KernelSem* ks) {
	lock
		kernSemNode* newNode = new kernSemNode(ks);
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

void KernelSemList::removeKernSem(KernelSem* ks) {
	kernSemNode* curr = head;
	kernSemNode* prev = 0;
	while (curr) {
		if (curr->kSem == ks) {
			removeNode(prev, curr);
			return;
		}
		prev = curr;
		curr = curr->next;
	}
}

void KernelSemList::removeNode(kernSemNode* prev, kernSemNode* target) {
	if (head != target && tail != target) prev->next = target->next;
	else if (head == target) {
		head = target->next;
		if (tail == target) tail = 0;
	}
	else { tail = prev; prev->next = 0; }
	lock
		delete target;
	unlock
	count--;
}

int KernelSemList::getCount() {
	return count;
}

void KernelSemList::updateTime() {
	kernSemNode* curr = head;
	while (curr) {
		curr->kSem->updateTime();
		curr = curr->next;
	}
}
