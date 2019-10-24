#include "JobSystem.h"
#include <random>
#include <thread>

using namespace std;

thread_local static WorkStealingQueue* workQueue = nullptr;

JobSystem::JobSystem()
{
	// only create number_of_cores - 1 threads
	workerThreadCount = thread::hardware_concurrency() - 1;
	if (workerThreadCount < 1)
		workerThreadCount = 1;
	
	//Create threads
	jobQueues = new WorkStealingQueue*[workerThreadCount];
	for (unsigned i = 0; i < workerThreadCount; i++)
	{
		std::thread t(&JobSystem::WorkerThreadLoop, this, i);
	}
}

JobSystem::~JobSystem()
{
	delete[] jobQueues;
}

// The main loop that worker threads run to run jobs
void JobSystem::WorkerThreadLoop(unsigned i)
{
	//Create the queue
	workQueue = new WorkStealingQueue();
	jobQueues[i] = workQueue;

	//Yield at first
	//TODO FIND A YIELD THAT WORKS

	//Run
	bool workerThreadActive = true;
	while (workerThreadActive)
	{
		Job* job = GetJob();
		if (job)
		{
			Execute(job);
		}
	}
}

// Check to see if this job is empty
bool JobSystem::IsEmptyJob(Job* job)
{
	return job == nullptr || job->function == nullptr;
}

WorkStealingQueue* JobSystem::GetWorkerThreadQueue()
{
	return workQueue;
}

// Generate a random number
unsigned int JobSystem::GenerateRandomNumber(unsigned int inclusiveMin, 
	unsigned int exclusiveMax)
{
	static thread_local std::mt19937 generator;
	std::uniform_int_distribution<int> distribution(inclusiveMin, exclusiveMax);
	return distribution(generator);
}

// Check to see if a job is finished
bool JobSystem::HasJobCompleted(const Job* job)
{
	return job->unfinishedJobs == 0;
}

// Allocate a new job
Job* JobSystem::AllocateJob()
{
	return new Job();
}

// Get a job that needs to be run
Job* JobSystem::GetJob()
{
	WorkStealingQueue* queue = GetWorkerThreadQueue();

	Job* job = queue->Pop();
	if (IsEmptyJob(job))
	{
		// this is not a valid job because our own queue is empty, so try stealing from some other queue
		unsigned int randomIndex = GenerateRandomNumber(0, workerThreadCount + 1);
		WorkStealingQueue* stealQueue = jobQueues[randomIndex];
		if (stealQueue == queue)
		{
			// don't try to steal from ourselves
			_YIELD_PROCESSOR;
			return nullptr;
		}

		Job* stolenJob = stealQueue->Steal();
		if (IsEmptyJob(stolenJob))
		{
			// we couldn't steal a job from the other queue either, so we just yield our time slice for now
			_YIELD_PROCESSOR;
			return nullptr;
		}

		return stolenJob;
	}

	return job;
}

// Execute a job
void JobSystem::Execute(Job* job)
{
	(job->function)(job, job->data);
	Finish(job);
}

// Finish executing a job
void JobSystem::Finish(Job* job)
{
	const int32_t unfinishedJobs = job->unfinishedJobs--;
	if (unfinishedJobs == 0)
	{
		const int32_t index = jobToDeleteCount++;
		jobsToDelete[index - 1] = job;

		if (job->parent)
		{
			Finish(job->parent);
		}

		job->unfinishedJobs--;
	}
}

Job* JobSystem::CreateJob(JobFunction function)
{
	Job* job = AllocateJob();
	job->function = function;
	job->parent = nullptr;
	job->unfinishedJobs = 1;

	return job;
}

Job* JobSystem::CreateJobAsChild(Job* parent, JobFunction function)
{
	parent->unfinishedJobs++;

	Job* job = AllocateJob();
	job->function = function;
	job->parent = parent;
	job->unfinishedJobs = 1;

	return job;
}

void JobSystem::Run(Job* job)
{
	WorkStealingQueue* queue = GetWorkerThreadQueue();
	queue->Push(job);
}

void JobSystem::Wait(const Job* job)
{
	// wait until the job has completed. in the meantime, work on any other job.
	while (!HasJobCompleted(job))
	{
		Job* nextJob = GetJob();
		if (nextJob)
		{
			Execute(nextJob);
		}
	}
}