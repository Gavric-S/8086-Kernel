#include "preempt.h"
#include "pcb.h"
#include "idlethrd.h"
#include "ivtentry.h"

#include <iostream.h>
#include <dos.h>

int userMain (int argc, char* argv[]);

int main(int argc, char* argv[]) {
	subTimerRoutine();

	PCB::mainPCB = new PCB();
	IdleThread::idleThread = new IdleThread();
	PCB::runningPCB = PCB::mainPCB;

    int ret = userMain(argc, argv);

    delete PCB::mainPCB;
    delete IdleThread::idleThread;

    restoreTimerRoutine();

    return ret;
}

