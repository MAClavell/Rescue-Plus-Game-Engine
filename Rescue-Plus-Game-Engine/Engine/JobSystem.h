#pragma once
#include "Job.h"
#include "WorkStealingQueue.h"

// --------------------------------------------------------
// Work stealing and lockless job system
//
// Based off of: https://blog.molecular-matters.com/2015/08/24/job-system-2-0-lock-free-work-stealing-part-1-basics/
// --------------------------------------------------------
class JobSystem
{
public:

	// --------------------------------------------------------
	// Initialize values
	// --------------------------------------------------------
	static void Init();

	// --------------------------------------------------------
	// Deinitialize values
	// --------------------------------------------------------
	static void Release();

	//TODO: memcpy data
	static Job* CreateJob(JobFunction function);
	static Job* CreateJob(JobFunction function, void* data);
	static Job* CreateJobAsChild(Job* parent, JobFunction function);
	static Job* CreateJobAsChild(Job* parent, JobFunction function, void* data);
	static void Run(Job* job);
	static void Wait(const Job* job);
	static void DeleteFinishedJobs();
};

static void EmptyJob(Job*, const void*) { };