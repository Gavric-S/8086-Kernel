#include "preempt.h"
#include "kernsem.h"
#include "pcb.h"
#include "SCHEDULE.H"
#include "idlethrd.h"
#include "semlist.h"

#include <dos.h>
#include <iostream.h>

pInterrupt oldTimerInt;

volatile int explicitDispatch = 0;
volatile int tickCount = defaultTimeSlice;
volatile int origTickCount;

volatile unsigned tsp;
volatile unsigned tss;
volatile unsigned tbp;

volatile int lockCount = 0;

void subTimerRoutine() {
#ifndef BCC_BLOCK_IGNORE
	asm cli;
		oldTimerInt = getvect(8);
		setvect(8, timer);
		setvect(96, oldTimerInt);
	asm sti;
#endif
}

void restoreTimerRoutine() {
#ifndef BCC_BLOCK_IGNORE
	asm cli;
		setvect(8, oldTimerInt);
	asm sti;
#endif
}

extern void tick();

void interrupt timer(...) {
 	if (!explicitDispatch) {
 		// decrement value of semaphors by one
 		KernelSem::KernSemList.updateTime();
 		tick();
		tickCount--;
	}

	if ((tickCount == 0 && lockCount >= 0) || explicitDispatch) {
		explicitDispatch = 0;
		asm {
			mov tsp, sp
			mov tss, ss
			mov tbp, bp
		}

		PCB::runningPCB->sp = tsp;
		PCB::runningPCB->ss = tss;
		PCB::runningPCB->bp = tbp;
		PCB::runningPCB->lckCount = lockCount;

		if (PCB::runningPCB->state == PCB::READY)
			Scheduler::put((PCB*) PCB::runningPCB);

		PCB::runningPCB = Scheduler::get();
		if (PCB::runningPCB == 0) {
			if (PCB::activePCBCount == 0) {
				PCB::mainPCB->state = PCB::READY;
				PCB::runningPCB = PCB::mainPCB;
			}
			else PCB::runningPCB = IdleThread::idlePCB;
		}

		tsp = PCB::runningPCB->sp;
		tss = PCB::runningPCB->ss;
		tbp = PCB::runningPCB->bp;
		lockCount = PCB::runningPCB->lckCount;

		origTickCount = PCB::runningPCB->quantum;
		tickCount = origTickCount;

		asm {
			mov sp, tsp
			mov ss, tss
			mov bp, tbp
		}

		if (PCB::runningPCB != IdleThread::idlePCB) PCB::runningPCB->processSignals();
	}
	if (tickCount == -1 && origTickCount > 0) tickCount = 1;

	if(!explicitDispatch) asm int 60h;
}

void dispatch(){
	asm cli;
		explicitDispatch = 1;
//		timer();
		asm int 8h;
	asm sti;
}
