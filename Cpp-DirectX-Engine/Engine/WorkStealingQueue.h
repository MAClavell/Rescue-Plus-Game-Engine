#pragma once
#include "Job.h"

#define MAX_JOBS 6144u

// --------------------------------------------------------
// Work stealing queue class for the work stealing job system
// Based on: https://blog.molecular-matters.com/2015/09/25/job-system-2-0-lock-free-work-stealing-part-3-going-lock-free/
// --------------------------------------------------------
class WorkStealingQueue
{
private:
	static const unsigned int MASK = MAX_JOBS - 1u;

	long bottom;
	long top;
	Job* jobs[MAX_JOBS];

public:
	WorkStealingQueue();
	~WorkStealingQueue();

	// --------------------------------------------------------
	// Push a job to the queue
	// --------------------------------------------------------
	void Push(Job* job);

	// --------------------------------------------------------
	// Steal a job from the queue
	// --------------------------------------------------------
	Job* Steal();

	// --------------------------------------------------------
	// Pop a job from the queue
	// --------------------------------------------------------
	Job* Pop();
};

