#pragma once
class CountSplitter
{
public:
	explicit CountSplitter(unsigned int count)
		: m_count(count)
	{
	}

	template <typename T>
	inline bool Split(unsigned int count) const
	{
		return (count > m_count);
	}

private:
	unsigned int m_count;
};

class DataSizeSplitter
{
public:
	explicit DataSizeSplitter(unsigned int size)
		: m_size(size)
	{
	}

	template <typename T>
	inline bool Split(unsigned int count) const
	{
		return (count * sizeof(T) > m_size);
	}

private:
	unsigned int m_size;
};