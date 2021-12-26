#include "ivtentry.h"
#include "kernelev.h"

IVTEntry *IVTEntry::ivt[IVT_SIZE];

IVTEntry::IVTEntry(IVTNo num, pInterrupt routine) : entryNum(num), intRoutine(routine), myEvent(0) {
#ifndef BCC_BLOCK_IGNORE
	asm cli;
		originalRoutine = getvect(entryNum);
		setvect(entryNum, intRoutine);
		ivt[entryNum] = this;
	asm sti;
#endif
}

IVTEntry::~IVTEntry() {
#ifndef BCC_BLOCK_IGNORE
	asm cli;
		setvect(entryNum, originalRoutine);
		ivt[entryNum] = 0;
	asm sti;
#endif
}

void IVTEntry::signal() {
	if (myEvent != 0) myEvent->signal();
}

void IVTEntry::callOriginal() {
	(*originalRoutine)();
}

void IVTEntry::setKernelEv(KernelEv* event) {
	lock
		myEvent = event;
	unlock
}
