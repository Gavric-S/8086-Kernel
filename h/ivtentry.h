#ifndef IVTENTRY_H_
#define IVTENTRY_H_

#include "event.h"
#include "preempt.h"

#include <dos.h>

class KernelEv;

#define IVT_SIZE 256

#define PREPAREENTRY(entryNum, callOrig)\
extern IVTEntry entry##entryNum;\
void interrupt routine##entryNum(...){\
	if (callOrig == 1) entry##entryNum.callOriginal();\
	entry##entryNum.signal();\
}\
IVTEntry entry##entryNum(entryNum, routine##entryNum);

class IVTEntry {
public:
	IVTEntry(IVTNo num, pInterrupt routine);
	~IVTEntry();

	static IVTEntry *ivt[IVT_SIZE];

	void signal();
	void callOriginal();

	void setKernelEv(KernelEv* event);
private:
	pInterrupt intRoutine;
	pInterrupt originalRoutine;
	IVTNo entryNum;
	KernelEv* myEvent;
};

#endif
