#pragma once
#include <atomic>

struct Job;
typedef void(*JobFunction) (Job*, const void*);

// --------------------------------------------------------
// Job class for the work stealing job system
// Based on: https://blog.molecular-matters.com/2015/08/24/job-system-2-0-lock-free-work-stealing-part-1-basics/
// --------------------------------------------------------
struct Job
{
	JobFunction function;
	Job* parent;
	std::atomic_int32_t unfinishedJobs;
	char padding[];
};