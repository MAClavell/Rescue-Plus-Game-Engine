#include "WorkStealingQueue.h"

WorkStealingQueue::WorkStealingQueue()
{
	bottom = 0;
	top = 0;
}


WorkStealingQueue::~WorkStealingQueue()
{ }

void WorkStealingQueue::Push(Job* job)
{
	long b = bottom;
	jobs[b & MASK] = job;

	// ensure the job is written before b+1 is published to other threads.
	// on x86/64, a compiler barrier is enough.
	_ReadWriteBarrier();

	bottom = b + 1;
}

Job* WorkStealingQueue::Steal()
{
	long t = top;

	// ensure that top is always read before bottom.
	// loads will not be reordered with other loads on x86, so a compiler barrier is enough.
	_ReadWriteBarrier();

	long b = bottom;
	if (t < b)
	{
		// non-empty queue
		Job* job = jobs[t & MASK];

		// the interlocked function serves as a compiler barrier, and guarantees that the read happens before the CAS.
		if (_InterlockedCompareExchange(&top, t + 1, t) != t)
		{
			// a concurrent steal or pop operation removed an element from the deque in the meantime.
			return nullptr;
		}

		return job;
	}
	else
	{
		// empty queue
		return nullptr;
	}
}

Job* WorkStealingQueue::Pop()
{
	long b = bottom - 1;
	bottom = b;

	long t = top;
	if (t <= b)
	{
		// non-empty queue
		Job* job = jobs[b & MASK];
		if (t != b)
		{
			// there's still more than one item left in the queue
			return job;
		}

		// this is the last item in the queue
		if (_InterlockedCompareExchange(&top, t + 1, t) != t)
		{
			// failed race against steal operation
			job = nullptr;
		}

		bottom = t + 1;
		return job;
	}
	else
	{
		// deque was already empty
		bottom = t;
		return nullptr;
	}
}

