#ifndef SEMLIST_H_
#define SEMLIST_H_

class KernelSem;

typedef struct KernelSemNode {
	KernelSem* kSem;
	struct KernelSemNode* next;

	KernelSemNode(KernelSem* ks) : kSem(ks), next(0) {}
} kernSemNode;

class KernelSemList {
public:
	KernelSemList();
	~KernelSemList();

	void addKernSem(KernelSem* ks);
	void removeKernSem(KernelSem* ks);

	int getCount();

	void updateTime();
private:
	kernSemNode* head;
	kernSemNode* tail;
	int count;

	void removeNode(kernSemNode* prev, kernSemNode* curr);
};

#endif
