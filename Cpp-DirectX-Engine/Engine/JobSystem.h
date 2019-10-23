#pragma once
#include "Job.h"

class JobSystem
{
private:
	WorkStealingQueue jobQueues[];
	
	//Job deletion
	std::atomic_int32_t jobToDeleteCount;
	Job* jobsToDelete[];

	Job* AllocateJob();
	WorkStealingQueue* GetWorkerThreadQueue();

public:
	Job* CreateJob(JobFunction function);
	Job* CreateJobAsChild(Job* parent, JobFunction function);
	void Run(Job* job);
	Job* GetJob();
	void Execute(Job* job);
	void Wait(const Job* job);
	void Finish(Job* job);
};

