#include "JobSystem.h"
#include <thread>
#include <random>
#include <condition_variable>
#include <mutex>
#include "RigidBody.h"

using namespace std;

//Job management
static thread* workerThreads;
static WorkStealingQueue** jobQueues;
static unsigned workerThreadCount;

//Condition variable management
// YES this is a lock in a lockless pool, but we need it for
//	when there's nothing in the queues and when the program terminates
static condition_variable threadCondition;
static mutex mtx;
static bool running = true;
static std::atomic_bool workerThreadsActive = true;

//Job deletion
static std::atomic_uint32_t jobsToDeleteCount;
static Job* jobsToDelete[MAX_JOBS];

//Thread local queue
thread_local static WorkStealingQueue* workQueue = nullptr;

// Yield time to another thread
static void Yield()
{
	this_thread::yield();
}

// Check to see if this job is empty
static bool IsEmptyJob(Job* job)
{
	return job == nullptr || job->function == nullptr;
}

static WorkStealingQueue* GetWorkerThreadQueue()
{
	return workQueue;
}

// Generate a random number
static unsigned int GenerateRandomNumber(unsigned int inclusiveMin,
	unsigned int inclusiveMax)
{
	static thread_local std::mt19937 generator;
	std::uniform_int_distribution<int> distribution(inclusiveMin, inclusiveMax);
	return distribution(generator);
}

// Check to see if a job is finished
static bool HasJobCompleted(const Job* job)
{
	return job->unfinishedJobs == -1;
}

// Allocate a new job
static Job* AllocateJob()
{
	return new Job();
}

// Get a job that needs to be run
static Job* GetJob()
{
	WorkStealingQueue* queue = GetWorkerThreadQueue();

	Job* job = queue->Pop();
	if (IsEmptyJob(job))
	{
		// this is not a valid job because our own queue is empty, so try stealing from some other queue
		// try to steal from the main thread's queue first
		WorkStealingQueue* mainQueue = jobQueues[workerThreadCount];
		if (mainQueue != queue && mainQueue != nullptr)
		{
			// steal a job
			Job* stolenJob = mainQueue->Steal();
			if (!IsEmptyJob(stolenJob))
			{
				return stolenJob;
			}
		}

		//try to steal from another queue
		unsigned int randomIndex = GenerateRandomNumber(0, workerThreadCount - 1);
		WorkStealingQueue* stealQueue = jobQueues[randomIndex];
		if (stealQueue == queue || stealQueue == nullptr)
		{
			// don't try to steal from ourselves
			Yield();
			return nullptr;
		}
		// steal a job
		Job* stolenJob = stealQueue->Steal();
		if (!IsEmptyJob(stolenJob))
		{
			return stolenJob;
		}

		// we couldn't steal a job from the other queue either, so we just yield our time slice for now
		Yield();
		return nullptr;
	}

	return job;
}

// Finish executing a job
static void Finish(Job* job)
{
	const int32_t unfinishedJobs = --(job->unfinishedJobs);
	if (unfinishedJobs == 0)
	{
		const int32_t index = ++jobsToDeleteCount;

		if (index >= MAX_JOBS)
			throw length_error("Allocated too many jobs! Max job count is: " + MAX_JOBS);

		jobsToDelete[index - 1] = job;

		if (job->parent)
		{
			Finish(job->parent);
		}

		job->unfinishedJobs--;
	}
}

// Execute a job
static void Execute(Job* job)
{
	(job->function)(job, job->data);
	Finish(job);
}

// The main loop that worker threads run to run jobs
static void WorkerThreadLoop(unsigned i)
{
	//Create the queue
	workQueue = new WorkStealingQueue();
	jobQueues[i] = workQueue;

	//Yield at first
	Yield();
	
	//Run
	while (workerThreadsActive)
	{
		Job* job = GetJob();
		if (job)
		{
			Execute(job);
		}

		//Lock releases when out of scope
		//TODO: put threads to sleep when not in use
		{
			std::unique_lock<std::mutex> lck(mtx);
			while (!running) threadCondition.wait(lck);
		}
	}
}

// --------------------------------------------------------
// CLASS FUNCTIONS
// --------------------------------------------------------
void JobSystem::Init()
{
	// only create number_of_cores - 1 threads
	workerThreadCount = thread::hardware_concurrency() - 1;
	if (workerThreadCount < 1)
		workerThreadCount = 1;

	//Create queue array and initialize it
	jobQueues = new WorkStealingQueue*[workerThreadCount + 1];
	for (unsigned i = 0; i < workerThreadCount + 1; i++)
	{
		jobQueues[i] = nullptr;
	}

	//Add main thread
	workQueue = new WorkStealingQueue();
	jobQueues[workerThreadCount] = workQueue;

	//Create worker threads
	workerThreads = new std::thread[workerThreadCount];
	for (unsigned i = 0; i < workerThreadCount; i++)
	{
		workerThreads[i] = std::thread(WorkerThreadLoop, i);
	}
}

void JobSystem::Release()
{
	//Lock releases when out of scope
	workerThreadsActive = false;
	{
		//Wake up all the threads
		std::unique_lock<std::mutex> lck(mtx);
		running = true;
		threadCondition.notify_all();
	}

	//Join all threads so they finish then delete array
	for (unsigned i = 0; i < workerThreadCount; i++)
	{
		workerThreads[i].join();
	}
	delete[] workerThreads;

	//Delete all jobs and job queues
	DeleteFinishedJobs();
	for (unsigned i = 0; i < workerThreadCount + 1; i++)
	{
		if (jobQueues[i])
			delete jobQueues[i];
	}
	delete[] jobQueues;
}

Job* JobSystem::CreateJob(JobFunction function)
{
	Job* job = AllocateJob();
	job->function = function;
	job->parent = nullptr;
	job->unfinishedJobs = 1;

	return job;
}

Job* JobSystem::CreateJob(JobFunction function, void* data)
{
	Job* job = CreateJob(function);
	if (sizeof(data) > sizeof(job->data))
	{
		throw length_error("Data being passed into thread is too large. Allocate on heap and pass in pointer instead.");
	}
	memcpy(job->data, data, sizeof(data));
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

Job* JobSystem::CreateJobAsChild(Job* parent, JobFunction function, void* data)
{
	Job* job = CreateJobAsChild(parent, function);
	if (sizeof(data) > sizeof(job->data))
	{
		throw length_error("Data being passed into thread is too large. Allocate on heap and pass in pointer instead.");
	}
	memcpy(job->data, data, sizeof(data));
	return job;
}

void JobSystem::Run(Job* job)
{
	WorkStealingQueue* queue = GetWorkerThreadQueue();
	queue->Push(job);

	//Lock releases when out of scope
	{
		std::unique_lock<std::mutex> lck(mtx);
		if (running == false)
		{
			std::unique_lock<std::mutex> lck(mtx);
			running = true;
			threadCondition.notify_all();
		}
	}
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

void JobSystem::DeleteFinishedJobs()
{
	for(unsigned i = 0; i < jobsToDeleteCount; i++)
	{
		Job* job = jobsToDelete[i];
		if (job)
		{
			delete job;
			jobsToDelete[i] = nullptr;
		}
	}

	jobsToDeleteCount = 0;
}
