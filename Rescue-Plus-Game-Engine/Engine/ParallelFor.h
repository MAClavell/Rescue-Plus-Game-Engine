#pragma once
#include "JobSystem.h"
#include "ParallelForSplitters.h"

template <typename T, typename S>
struct parallel_for_job_data
{
	typedef T DataType;
	typedef S SplitterType;

	parallel_for_job_data(DataType* data, unsigned int count, void(*function)(DataType*, unsigned int), const SplitterType& splitter)
		: data(data)
		, count(count)
		, function(function)
		, splitter(splitter)
	{ }

	DataType* data;
	unsigned int count;
	void(*function)(DataType*, unsigned int);
	SplitterType splitter;
};

template <typename JobData>
void parallel_for_job(Job* job, const void* jobData)
{
	const JobData* data = static_cast<const JobData*>(jobData);
	const JobData::SplitterType& splitter = data->splitter;

	if (splitter.Split<JobData::DataType>(data->count))
	{
		// split in two
		const unsigned int leftCount = data->count / 2u;
		const JobData leftData(data->data, leftCount, data->function, splitter);
		Job* left = JobSystem::CreateJobAsChild(job, &parallel_for_job<JobData>, (void*)&leftData);
		JobSystem::Run(left);

		const unsigned int rightCount = data->count - leftCount;
		const JobData rightData(data->data + leftCount, rightCount, data->function, splitter);
		Job* right = JobSystem::CreateJobAsChild(job, &parallel_for_job<JobData>, (void*)&rightData);
		JobSystem::Run(right);
	}
	else
	{
		// execute the function on the range of data
		(data->function)(data->data, data->count);
	}
}

template <typename T, typename S>
Job* parallel_for(T* data, unsigned int count, void(*function)(T*, unsigned int), const S& splitter)
{
	typedef parallel_for_job_data<T, S> JobData;
	const JobData jobData(data, count, function, splitter);

	unsigned long long size = sizeof(JobData);

	Job* job = JobSystem::CreateJob(&parallel_for_job<JobData>, (void*)&jobData);
	return job;
}