#ifndef PREEMPT_H_
#define PREEMPT_H_

class IdleThread;

#define lock --lockCount;
#define unlock ++lockCount;
extern volatile int lockCount;

extern volatile int tickCount;
extern volatile int origTickCount;
extern volatile int explicitDispatch;
extern IdleThread* idleThread;

extern volatile unsigned tsp;
extern volatile unsigned tss;
extern volatile unsigned tbp;

typedef void interrupt (*pInterrupt)(...);
extern pInterrupt oldTimerInt;

// substitutes new interrupt routine
void subTimerRoutine();
// restores old interrupt routine
void restoreTimerRoutine();

void interrupt timer(...);
void dispatch();

//#define lockInt asm{\
//	pushf;\
//	cli;\
//}
//#define unlockInt asm popf;

#endif
