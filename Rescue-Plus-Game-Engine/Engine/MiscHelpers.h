#pragma once
#include <vector>

template <typename T>
// --------------------------------------------------------
// Remove an item of a specific type from a vector
//
// item - the found item instance (output)
// --------------------------------------------------------
bool RemoveTypeFromVector(std::vector<T>* vec, T** outItem = nullptr)
{
	for (auto iter = vec->begin(); iter != vec->end(); iter++)
	{
		try
		{
			T& c = dynamic_cast<T&>(**iter); // try to cast
			*outItem = c;

			//Swap with the last and pop
			std::iter_swap(iter, vec->end());
			vec->pop_back();
			return true;
		}
		catch (...)
		{ }
	}
	return false;
}