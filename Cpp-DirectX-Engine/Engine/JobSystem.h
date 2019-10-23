#pragma once
#include <thread>
#include "Job.h"
#include "WorkStealingQueue.h"

// --------------------------------------------------------
// Work stealing and lockless job system
//
// Based off of: https://blog.molecular-matters.com/2015/08/24/job-system-2-0-lock-free-work-stealing-part-1-basics/
// --------------------------------------------------------
class JobSystem
{
private:
	WorkStealingQueue** jobQueues;
	unsigned workerThreadCount;

	//Job deletion
	std::atomic_int32_t jobToDeleteCount;
	Job* jobsToDelete[];

	// --------------------------------------------------------
	// The main loop that worker threads run to run jobs
	// --------------------------------------------------------
	void WorkerThreadLoop();

	// --------------------------------------------------------
	// Check to see if this job is empty
	// --------------------------------------------------------
	bool IsEmptyJob(Job* job);

	WorkStealingQueue* GetWorkerThreadQueue();
	
	// --------------------------------------------------------
	// Check to see if a job is finished
	// --------------------------------------------------------
	bool HasJobCompleted(const Job* job);

	// --------------------------------------------------------
	// Allocate a new job
	// --------------------------------------------------------
	Job* AllocateJob();

	// --------------------------------------------------------
	// Get a job that needs to be run
	// --------------------------------------------------------
	Job* GetJob();

	// --------------------------------------------------------
	// Execute a job
	// --------------------------------------------------------
	void Execute(Job* job);

	// --------------------------------------------------------
	// Finish executing a job
	// --------------------------------------------------------
	void Finish(Job* job);

public:
	JobSystem();
	~JobSystem();

	Job* CreateJob(JobFunction function);
	Job* CreateJobAsChild(Job* parent, JobFunction function);
	void Run(Job* job);
	void Wait(const Job* job);
};
